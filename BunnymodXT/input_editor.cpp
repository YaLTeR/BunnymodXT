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
	fractions.push_back({1, 0, 0, 0});
	normalzs.push_back({0, 0, 0, 0});
	frame_bulk_starts.push_back(0);
	next_frame_is_0mss.push_back(0);
	player_health_datas.push_back(0);
	player_armor_datas.push_back(0);
	target_yaw_overrides.emplace_back();
	active_target_yaw_override_indices.push_back(0);
	pushables.emplace_back();

	// TODO: replace with hw.StrafeState when the
	// one-frame-being-run-from-tas-editor-enable-frame-bulk issue is solved.
	auto strafe_state = hw.PrevStrafeState;
	strafe_states.push_back(strafe_state);

	initial_movement_vars = hw.GetMovementVars();
	frametimes.push_back(initial_movement_vars.Frametime);

	first_predicted_frame = 1;
	current_generation = 0;
	first_frame_counter_value = 0;
	received_data_from_second_game = false;
}

void EditedInput::simulate() {
	auto start = std::chrono::steady_clock::now();
	if (run_in_second_game_at.time_since_epoch().count() > 0 && start > run_in_second_game_at) {
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

	const auto point_contents_func = std::bind(
		*(ClientDLL::GetInstance().pEngfuncs->PM_PointContents),
		std::placeholders::_1,
		nullptr
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

	std::vector<float> target_yaw_override_to_push;

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
				case HLTAS::ChangeTarget::TARGET_YAW_OFFSET:
					strafe_state.ChangeTargetYawOffsetValue = frame_bulk.GetChangeFinalValue();
					strafe_state.ChangeTargetYawOffsetOver = frame_bulk.GetChangeOver();
					break;
				default:
					assert(false);
					break;
				}
			} else if (!frame_bulk.TargetYawOverride.empty()) {
				target_yaw_override_to_push = frame_bulk.TargetYawOverride;
				strafe_state.TargetYawOverride = target_yaw_override_to_push[0];
				strafe_state.TargetYawOverrideActive = true;
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
				point_contents_func,
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

			// Update the target yaw override state.
			if (strafe_state.TargetYawOverrideActive) {
				if (target_yaw_override_to_push.empty()) {
					// This is an old override.
					const auto override_index = active_target_yaw_override_indices.back();
					const auto& override = target_yaw_overrides[override_index];
					// The yaw advances every frame, so compute the yaw index using the number of frames since the override.
					const auto yaw_index = player_datas.size() + 1 - override_index;
					if (yaw_index >= override.size()) {
						// The override has run out, or (the `>` case) the override was before the edited area.
						strafe_state.TargetYawOverrideActive = false;
						active_target_yaw_override_indices.push_back(0);
					} else {
						// Prepare the next yaw override.
						strafe_state.TargetYawOverride = override[yaw_index];
						active_target_yaw_override_indices.push_back(override_index);
					}
				} else {
					// This is a new override.
					if (target_yaw_override_to_push.size() == 1) {
						// It had just 1 entry and we just used it.
						strafe_state.TargetYawOverrideActive = false;
					} else {
						// There are more entries, prepare to use the next one.
						strafe_state.TargetYawOverride = target_yaw_override_to_push[1];
					}

					// Push its index.
					active_target_yaw_override_indices.push_back(target_yaw_overrides.size());
				}
			} else {
				// Target yaw override is inactive.
				active_target_yaw_override_indices.push_back(0);
			}

			// Push either new or empty target yaw override.
			target_yaw_overrides.emplace_back(std::move(target_yaw_override_to_push));
			target_yaw_override_to_push.clear();

			player_datas.push_back(player);
			strafe_states.push_back(strafe_state);
			fractions.push_back(std::array<float, 4>{
				processed_frame.fractions[0], processed_frame.fractions[1], processed_frame.fractions[2], processed_frame.fractions[3]
			});
			normalzs.push_back(std::array<float, 4>{
				processed_frame.normalzs[0], processed_frame.normalzs[1], processed_frame.normalzs[2], processed_frame.normalzs[3]
			});
			next_frame_is_0mss.push_back(next_frame_is_0ms);
			frametimes.push_back(movement_vars.Frametime);

			// push back 0s because there's no data
			player_health_datas.push_back(0);
			player_armor_datas.push_back(0);
			pushables.emplace_back();

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
	target_yaw_overrides.erase(target_yaw_overrides.begin() + first_frame + 1, target_yaw_overrides.end());
	active_target_yaw_override_indices.erase(active_target_yaw_override_indices.begin() + first_frame + 1, active_target_yaw_override_indices.end());
	pushables.erase(pushables.begin() + first_frame + 1, pushables.end());
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
	if (last_frame >= player_datas.size() - 1)
		return;

	player_health_datas.erase(player_health_datas.begin() + last_frame + 1, player_health_datas.end());
	player_armor_datas.erase(player_armor_datas.begin() + last_frame + 1, player_armor_datas.end());
	player_datas.erase(player_datas.begin() + last_frame + 1, player_datas.end());
	strafe_states.erase(strafe_states.begin() + last_frame + 1, strafe_states.end());
	fractions.erase(fractions.begin() + last_frame + 1, fractions.end());
	normalzs.erase(normalzs.begin() + last_frame + 1, normalzs.end());
	next_frame_is_0mss.erase(next_frame_is_0mss.begin() + last_frame + 1, next_frame_is_0mss.end());
	frametimes.erase(frametimes.begin() + last_frame + 1, frametimes.end());
	target_yaw_overrides.erase(target_yaw_overrides.begin() + last_frame + 1, target_yaw_overrides.end());
	pushables.erase(pushables.begin() + last_frame + 1, pushables.end());
	active_target_yaw_override_indices.erase(active_target_yaw_override_indices.begin() + last_frame + 1, active_target_yaw_override_indices.end());

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
		if (simulation_ipc::write_command("echo Cannot run the TAS because it is missing the load_command property.\n"))
			simulation_ipc::send_message_to_client();
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

	auto input = hw.input; // Make a copy to mess with.
	const auto frame_count = input.GetFrames().size();
	if (frame_count != 0) {
		const auto last_frame = input.GetFrames()[frame_count - 1];
		input.RemoveFrame(frame_count - 1);

		for (const auto& frame_bulk: frame_bulks) {
			input.PushFrame(frame_bulk);
		}
		input.PushFrame(last_frame);
	}

	frame_bulks.pop_back();
	frame_bulks[0].Commands = tas_editor_command;

	// Send the message to the client.
	std::ostringstream oss;
	oss << "_bxt_tas_script_generation " << current_generation
	    << ";sensitivity 0;volume 0;MP3Volume 0;bxt_tas_norefresh_until_last_frames 1";

	if (input.ToString(simulation_ipc::message.script, sizeof(simulation_ipc::message.script)).Code == HLTAS::ErrorCode::OK) {
		oss << ";_bxt_tas_client_load_received_script\n";
	} else {
		// Couldn't save to the buffer, maybe the script was too large. Try saving to a file.
		simulation_ipc::message.script[0] = 0;

		const char filename[] = "_bxt-tas-server-script-to-run.hltas";

		simulation_ipc::maybe_lock_mutex();
		auto err = input.Save(filename);
		simulation_ipc::maybe_unlock_mutex();

		if (err.Code != HLTAS::ErrorCode::OK) {
			hw.ORIG_Con_Printf("Error saving the script: %s\n", HLTAS::GetErrorMessage(err).c_str());
			return;
		}

		oss << ";bxt_tas_loadscript " << filename << '\n';
	}

	if (simulation_ipc::write_command(oss.str()))
		simulation_ipc::send_message_to_client();
}

void EditedInput::schedule_run_in_second_game() {
	current_generation++;
	run_in_second_game_at = std::chrono::steady_clock::now() + 100ms;
}

void EditedInput::received_simulated_frame(const simulation_ipc::SimulatedFrame &frame) {
	received_data_from_second_game = true;

	if (frame.generation != current_generation)
		return;

	if (frame.number <= first_frame_counter_value)
		return;

	auto frame_number = frame.number - first_frame_counter_value;
	if (frame_number >= player_datas.size())
		return;

	player_datas[frame_number] = frame.player_data;
	strafe_states[frame_number] = frame.strafe_state;
	fractions[frame_number] = frame.fractions;
	normalzs[frame_number] = frame.normalzs;
	next_frame_is_0mss[frame_number] = frame.next_frame_is_0ms;
	player_health_datas[frame_number] = frame.health;
	player_armor_datas[frame_number] = frame.armor;
	frametimes[frame_number] = frame.frametime;
	pushables[frame_number] = frame.pushables;

	first_predicted_frame = std::max(first_predicted_frame, frame_number + 1);
}
