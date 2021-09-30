#include "stdafx.hpp"

#include "input_editor.hpp"
#include "modules.hpp"

void EditedInput::initialize() {
	auto& hw = HwDLL::GetInstance();

	auto player = hw.GetPlayerData();
	player_datas.push_back(player);
	fractions.push_back(1);
	normalzs.push_back(0);
	frame_bulk_starts.push_back(0);
	next_frame_is_0mss.push_back(0);

	// TODO: replace with hw.StrafeState when the
	// one-frame-being-run-from-tas-editor-enable-frame-bulk issue is solved.
	auto strafe_state = hw.PrevStrafeState;
	strafe_states.push_back(strafe_state);

	initial_movement_vars = hw.GetMovementVars();
}

void EditedInput::simulate(SimulateFrameBulks what) {
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

	auto start = std::chrono::steady_clock::now();

	for (size_t index = first_frame_bulk; index < frame_bulks.size(); ++index) {
		if (what == SimulateFrameBulks::ALL_EXCEPT_LAST && index == frame_bulks.size() - 1)
			break;

		const auto& frame_bulk = frame_bulks[index];

		const auto host_frametime = std::strtof(frame_bulk.Frametime.c_str(), nullptr);
		const auto frametime = static_cast<float>(static_cast<float>(std::floor(host_frametime * 1000)) * 0.001);
		movement_vars.Frametime = frametime;

		size_t frame = player_datas.size() - 1 - total_frames;
		for (; frame < frame_bulk.GetRepeats(); ++frame) {
			// Break early if already simulating for more than a set number of milliseconds
			// to keep the FPS high.
			auto now = std::chrono::steady_clock::now();
			auto simulating_for = now - start;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(simulating_for).count()
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
		}

		// If we broke out early.
		if (frame < frame_bulk.GetRepeats())
			break;

		total_frames += frame_bulk.GetRepeats();
		frame_bulk_starts.push_back(total_frames);
	}

	hw.StopTracing();
}

void EditedInput::save() {
	auto& hw = HwDLL::GetInstance();

	const auto frame_count = hw.input.GetFrames().size();
	if (frame_count == 0)
		return;

	const auto last_frame = hw.input.GetFrames()[frame_count - 1];
	hw.input.RemoveFrame(frame_count - 1);

	for (const auto& frame_bulk : frame_bulks) {
		hw.input.PushFrame(frame_bulk);
	}
	hw.input.PushFrame(last_frame);

	auto err = hw.input.Save(hw.hltas_filename);
	if (err.Code == HLTAS::ErrorCode::OK)
		hw.ORIG_Con_Printf("Saved the script: %s\n", hw.hltas_filename.c_str());
	else
		hw.ORIG_Con_Printf("Error saving the script: %s\n", HLTAS::GetErrorMessage(err).c_str());
}

void EditedInput::mark_as_stale(size_t frame_bulk_index) {
	if (frame_bulk_index > frame_bulk_starts.size())
		return;

	frame_bulk_starts.erase(frame_bulk_starts.begin() + frame_bulk_index + 1, frame_bulk_starts.end());

	auto first_frame = std::min(*(frame_bulk_starts.cend() - 1), player_datas.size() - 1);
	player_datas.erase(player_datas.begin() + first_frame + 1, player_datas.end());
	strafe_states.erase(strafe_states.begin() + first_frame + 1, strafe_states.end());
	fractions.erase(fractions.begin() + first_frame + 1, fractions.end());
	normalzs.erase(normalzs.begin() + first_frame + 1, normalzs.end());
	next_frame_is_0mss.erase(next_frame_is_0mss.begin() + first_frame + 1, next_frame_is_0mss.end());
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

	// Invalidate all later frames, if they exist.
	auto last_frame = frame_bulk_starts[frame_bulk_index] + std::min(repeats, old_repeats);
	if (last_frame >= player_datas.size() - 2)
		return;

	player_datas.erase(player_datas.begin() + last_frame + 1, player_datas.end());
	strafe_states.erase(strafe_states.begin() + last_frame + 1, strafe_states.end());
	fractions.erase(fractions.begin() + last_frame + 1, fractions.end());
	normalzs.erase(normalzs.begin() + last_frame + 1, normalzs.end());
	next_frame_is_0mss.erase(next_frame_is_0mss.begin() + last_frame + 1, next_frame_is_0mss.end());

	// Update the frame count.
	if (frame_bulk_index + 1 < frame_bulk_starts.size())
		frame_bulk_starts[frame_bulk_index + 1] = last_frame;
}

bool EditedInput::simulated_all_frames() const {
	// If we have the total frame count, then we simulated all frames.
	return frame_bulk_starts.size() == frame_bulks.size() + 1;
}
