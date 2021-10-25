#include "stdafx.hpp"

#include "input_editor.hpp"
#include "modules.hpp"
#include "cvars.hpp"
#include "simulation_ipc.hpp"

using namespace std::chrono_literals;

void EditedInput::initialize() {
	auto& hw = HwDLL::GetInstance();

	auto player = hw.GetPlayerData();
	player_datas.push_back(player);
	fractions.push_back(1);
	normalzs.push_back(0);
	frame_bulk_starts.push_back(0);
	next_frame_is_0mss.push_back(0);
	player_health_datas.push_back(0);
	player_armor_datas.push_back(0);

	// TODO: replace with hw.StrafeState when the
	// one-frame-being-run-from-tas-editor-enable-frame-bulk issue is solved.
	auto strafe_state = hw.PrevStrafeState;
	strafe_states.push_back(strafe_state);

	initial_movement_vars = hw.GetMovementVars();
	frametimes.push_back(initial_movement_vars.Frametime);

	first_predicted_frame = 1;
	current_generation = 0;
	first_frame_counter_value = 0;
}

void EditedInput::simulate() {
	auto start = std::chrono::steady_clock::now();
	if (run_in_second_game_at.time_since_epoch().count() > 0) {
		run_in_second_game_at = std::chrono::steady_clock::time_point();
		run_script_in_second_game();
	}

	simulation_ipc::receive_messages_from_client();

	auto first_frame_bulk = frame_bulk_starts.size() - 1;

	// Return early if we don't need to simulate anything.
	if (first_frame_bulk == frame_bulks.size())
		return;

	auto& hw = HwDLL::GetInstance();
	auto movement_vars = initial_movement_vars;

	hw.StartTracing(true);

	const auto trace_func = std::bind(
		&HwDLL::UnsafePlayerTrace,
		&hw,
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	);

	size_t total_frames = frame_bulk_starts[first_frame_bulk];

	auto player = *(player_datas.cend() - 1);
	auto strafe_state = *(strafe_states.cend() - 1);
	auto next_frame_is_0ms = *(next_frame_is_0mss.cend() - 1);

	// This check is needed because simulating non-movement frame bulks
	// advances the simulated frame bulk counter, but doesn't save any state
	// changes. The state changes only save after a movement frame. Therefore,
	// we don't want to early-exit after simulating a non-movement frame bulk
	// and before simulating the next movement frame.
	bool safe_to_exit_early = false;

	for (size_t index = first_frame_bulk; index < frame_bulks.size(); ++index) {
		const auto& frame_bulk = frame_bulks[index];

		if (!frame_bulk.IsMovement()) {
			// TODO: Buttons frame.
			if (frame_bulk.LgagstMinSpeedPresent) {
				strafe_state.LgagstMinSpeed = frame_bulk.GetLgagstMinSpeed();
			} else if (frame_bulk.StrafingAlgorithmPresent) {
				strafe_state.Algorithm = frame_bulk.GetAlgorithm();
			} else if (frame_bulk.AlgorithmParametersPresent) {
				strafe_state.Parameters = frame_bulk.GetAlgorithmParameters();
			} else if (frame_bulk.ChangePresent) {
				switch (frame_bulk.GetChangeTarget()) {
				case HLTAS::ChangeTarget::YAW:
					strafe_state.ChangeYawFinalValue = frame_bulk.GetChangeFinalValue();
					strafe_state.ChangeYawOver = frame_bulk.GetChangeOver();
					break;
				case HLTAS::ChangeTarget::PITCH:
					strafe_state.ChangePitchFinalValue = frame_bulk.GetChangeFinalValue();
					strafe_state.ChangePitchOver = frame_bulk.GetChangeOver();
					break;
				case HLTAS::ChangeTarget::TARGET_YAW:
					strafe_state.ChangeTargetYawFinalValue = frame_bulk.GetChangeFinalValue();
					strafe_state.ChangeTargetYawOver = frame_bulk.GetChangeOver();
					break;
				default:
					assert(false);
					break;
				}
			}

			safe_to_exit_early = false;
			frame_bulk_starts.push_back(total_frames);
			continue;
		}

		const auto host_frametime = std::strtof(frame_bulk.Frametime.c_str(), nullptr);
		const auto frametime = static_cast<float>(static_cast<float>(std::floor(host_frametime * 1000)) * 0.001);
		movement_vars.Frametime = frametime;

		size_t frame = player_datas.size() - 1 - total_frames;
		for (; frame < frame_bulk.GetRepeats(); ++frame) {
			// Break early if already simulating for more than a set number of milliseconds
			// to keep the FPS high.
			auto now = std::chrono::steady_clock::now();
			auto simulating_for = now - start;
			if (safe_to_exit_early && std::chrono::duration_cast<std::chrono::milliseconds>(simulating_for).count()
					> CVars::bxt_tas_editor_simulate_for_ms.GetInt())
				break;

			// TODO:
			// - assumes frametime0ms is high enough to always give zero frametime
			//   (this is how it should be set up anyway)
			if (next_frame_is_0ms)
				movement_vars.Frametime = 0;
			else
				movement_vars.Frametime = frametime;

			auto processed_frame = HLStrafe::MainFunc(
				player,
				movement_vars,
				frame_bulk,
				strafe_state,
				hw.Buttons,
				hw.ButtonsPresent,
				trace_func,
				hw.hlstrafe_version
			);

			// printf(
			//     "origin %.8f %.8f %.8f => %.8f %.8f %.8f vel %.8f %.8f %.8f => %.8f %.8f %.8f\n",
			//     player.Origin[0],
			//     player.Origin[1],
			//     player.Origin[2],
			//     processed_frame.NewPlayerData.Origin[0],
			//     processed_frame.NewPlayerData.Origin[1],
			//     processed_frame.NewPlayerData.Origin[2],
			//     player.Velocity[0],
			//     player.Velocity[1],
			//     player.Velocity[2],
			//     processed_frame.NewPlayerData.Velocity[0],
			//     processed_frame.NewPlayerData.Velocity[1],
			//     processed_frame.NewPlayerData.Velocity[2]
			// );

			player = processed_frame.NewPlayerData;
			next_frame_is_0ms = processed_frame.NextFrameIs0ms;

			// PredictThis is needed because 0ms frames are batched client-side. Since we're
			// re-using the HLStrafe prediction, here they are already predicted.
			strafe_state.PredictThis = HLStrafe::State0ms::NOTHING;

			player_datas.push_back(player);
			strafe_states.push_back(strafe_state);
			fractions.push_back(processed_frame.fractions[0]);
			normalzs.push_back(processed_frame.normalzs[0]);
			next_frame_is_0mss.push_back(next_frame_is_0ms);
			frametimes.push_back(movement_vars.Frametime);

			// push back 0s because there's no data
			player_health_datas.push_back(0);
			player_armor_datas.push_back(0);

			safe_to_exit_early = true;
		}

		// If we broke out early.
		if (frame < frame_bulk.GetRepeats())
			break;

		total_frames += frame_bulk.GetRepeats();
		frame_bulk_starts.push_back(total_frames);
	}

	hw.StopTracing();
}

HLTAS::ErrorDescription EditedInput::save(const std::string &filename) const {
	auto& hw = HwDLL::GetInstance();

	auto input = hw.input; // Make a copy to mess with.
	const auto frame_count = input.GetFrames().size();
	if (frame_count != 0) {
		const auto last_frame = input.GetFrames()[frame_count - 1];
		input.RemoveFrame(frame_count - 1);

		for (const auto& frame_bulk : frame_bulks) {
			input.PushFrame(frame_bulk);
		}
		input.PushFrame(last_frame);
	}

	simulation_ipc::maybe_lock_mutex();
	auto err = input.Save(filename);
	simulation_ipc::maybe_unlock_mutex();
	return err;
}

void EditedInput::mark_as_stale(size_t frame_bulk_index) {
	if (frame_bulk_index >= frame_bulks.size())
		return;

	// If there are non-movement frame bulks in front of the stale one, we should run them too,
	// as their state modifications are not cached.
	for (; frame_bulk_index > 0 && !frame_bulks[frame_bulk_index - 1].IsMovement(); --frame_bulk_index)
		;

	if (frame_bulk_index >= frame_bulk_starts.size())
		return;

	frame_bulk_starts.erase(frame_bulk_starts.begin() + frame_bulk_index + 1, frame_bulk_starts.end());

	auto first_frame = std::min(*(frame_bulk_starts.cend() - 1), player_datas.size() - 1);
	player_health_datas.erase(player_health_datas.begin() + first_frame + 1, player_health_datas.end());
	player_armor_datas.erase(player_armor_datas.begin() + first_frame + 1, player_armor_datas.end());
	player_datas.erase(player_datas.begin() + first_frame + 1, player_datas.end());
	strafe_states.erase(strafe_states.begin() + first_frame + 1, strafe_states.end());
	fractions.erase(fractions.begin() + first_frame + 1, fractions.end());
	normalzs.erase(normalzs.begin() + first_frame + 1, normalzs.end());
	next_frame_is_0mss.erase(next_frame_is_0mss.begin() + first_frame + 1, next_frame_is_0mss.end());
	frametimes.erase(frametimes.begin() + first_frame + 1, frametimes.end());
	first_predicted_frame = std::min(first_predicted_frame, first_frame + 1);

	schedule_run_in_second_game();
}

void EditedInput::set_repeats(size_t frame_bulk_index, unsigned repeats) {
	auto& frame_bulk = frame_bulks[frame_bulk_index];
	auto old_repeats = frame_bulk.GetRepeats();

	// If the repeat count is the same, no need to change anything.
	if (old_repeats == repeats)
		return;

	frame_bulk.SetRepeats(repeats);

	// If we haven't simulated up to this frame bulk yet, no need to invalidate anything.
	if (frame_bulk_index >= frame_bulk_starts.size())
		return;

	if (repeats > old_repeats) {
		// When extending the frame bulk, delete the total frame count and/or any later frame bulk starts.
		if (frame_bulk_index + 1 < frame_bulk_starts.size())
			frame_bulk_starts.erase(frame_bulk_starts.begin() + frame_bulk_index + 1, frame_bulk_starts.end());
	} else {
		// When reducing the frame bulk, delete only any later frame bulk starts.
		if (frame_bulk_index + 2 < frame_bulk_starts.size())
			frame_bulk_starts.erase(frame_bulk_starts.begin() + frame_bulk_index + 2, frame_bulk_starts.end());
	}

	auto last_frame = frame_bulk_starts[frame_bulk_index] + std::min(repeats, old_repeats);

	first_predicted_frame = std::min(first_predicted_frame, last_frame + 1);
	schedule_run_in_second_game();

	// Invalidate all later frames, if they exist.
	if (last_frame >= player_datas.size() - 2)
		return;

	player_health_datas.erase(player_health_datas.begin() + last_frame + 1, player_health_datas.end());
	player_armor_datas.erase(player_armor_datas.begin() + last_frame + 1, player_armor_datas.end());
	player_datas.erase(player_datas.begin() + last_frame + 1, player_datas.end());
	strafe_states.erase(strafe_states.begin() + last_frame + 1, strafe_states.end());
	fractions.erase(fractions.begin() + last_frame + 1, fractions.end());
	normalzs.erase(normalzs.begin() + last_frame + 1, normalzs.end());
	next_frame_is_0mss.erase(next_frame_is_0mss.begin() + last_frame + 1, next_frame_is_0mss.end());
	frametimes.erase(frametimes.begin() + last_frame + 1, frametimes.end());

	// Update the frame count.
	if (frame_bulk_index + 1 < frame_bulk_starts.size())
		frame_bulk_starts[frame_bulk_index + 1] = last_frame;
}

void EditedInput::run_script_in_second_game() {
	auto& hw = HwDLL::GetInstance();

	if (!simulation_ipc::is_server_initialized())
		return;

	if (frame_bulks.empty())
		return;

	const auto& properties = hw.input.GetProperties();
	if (properties.find(std::string("load_command")) == properties.cend()) {
		simulation_ipc::send_command_to_client("echo Cannot run the TAS because it is missing the load_command property.\n");
		return;
	}

	assert(frame_bulks[0].IsMovement());
	auto tas_editor_command = frame_bulks[0].Commands;
	frame_bulks[0].Commands.clear();

	HLTAS::Frame final_frame;
	final_frame.Commands = "toggleconsole";
	final_frame.SetRepeats(1);
	final_frame.Frametime = frame_bulks[0].Frametime;
	frame_bulks.push_back(final_frame);

	std::string filename("_bxt-tas-server-script-to-run.hltas");
	auto err = save(filename);
	if (err.Code != HLTAS::ErrorCode::OK) {
		hw.ORIG_Con_Printf("Error saving the script: %s\n", HLTAS::GetErrorMessage(err).c_str());
		return;
	}

	frame_bulks.pop_back();
	frame_bulks[0].Commands = tas_editor_command;

	std::ostringstream oss;
	oss << "_bxt_tas_script_generation " << ++current_generation
	    << ";sensitivity 0;volume 0;MP3Volume 0;bxt_tas_norefresh_until_last_frames 1;bxt_tas_loadscript " << filename << '\n';
	simulation_ipc::send_command_to_client(oss.str());
}

void EditedInput::schedule_run_in_second_game() {
	run_in_second_game_at = std::chrono::steady_clock::now() + 100ms;
}

void EditedInput::received_simulated_frame(const simulation_ipc::SimulatedFrame &frame) {
	if (frame.generation != current_generation)
		return;

	if (frame.number <= first_frame_counter_value)
		return;

	auto frame_number = frame.number - first_frame_counter_value;
	if (frame_number >= player_datas.size())
		return;

	player_datas[frame_number] = frame.player_data;
	strafe_states[frame_number] = frame.strafe_state;
	fractions[frame_number] = frame.fraction;
	normalzs[frame_number] = frame.normalz;
	next_frame_is_0mss[frame_number] = frame.next_frame_is_0ms;
	player_health_datas[frame_number] = frame.health;
	player_armor_datas[frame_number] = frame.armor;
	frametimes[frame_number] = frame.frametime;

	first_predicted_frame = std::max(first_predicted_frame, frame_number + 1);
}
