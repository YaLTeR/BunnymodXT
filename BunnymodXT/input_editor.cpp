#include "stdafx.hpp"

#include "input_editor.hpp"
#include "modules.hpp"

void EditedInput::initialize() {
	auto& hw = HwDLL::GetInstance();

	auto player = hw.GetPlayerData();
	saved_player.push_back(player);
	positions.push_back(player.Origin);
	fractions.push_back(1);
	normalzs.push_back(0);
	frame_bulk_starts.push_back(0);

	auto strafe_state = hw.StrafeState;
	strafe_state.Jump = hw.currentKeys.Jump.IsDown();
	strafe_state.Duck = hw.currentKeys.Duck.IsDown();
	saved_state.push_back(strafe_state);

	initial_movement_vars = hw.GetMovementVars();
}

void EditedInput::simulate(SimulateFrameBulks what) {
	// Erase all stale state.
	auto first_frame_bulk = saved_player.size() - 1;

	// Return early if we don't need to simulate anything.
	if (first_frame_bulk == frame_bulks.size())
		return;

	auto& hw = HwDLL::GetInstance();
	auto player = saved_player[first_frame_bulk];
	auto strafe_state = saved_state[first_frame_bulk];
	auto movement_vars = initial_movement_vars;

	hw.StartTracing(true);

	const auto trace_func = std::bind(
		&HwDLL::UnsafePlayerTrace,
		&hw,
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	);

	size_t total_frames = frame_bulk_starts[frame_bulk_starts.size() - 1];

	for (size_t index = first_frame_bulk; index < frame_bulks.size(); ++index) {
		if (what == SimulateFrameBulks::ALL_EXCEPT_LAST && index == frame_bulks.size() - 1)
			break;

		const auto& frame_bulk = frame_bulks[index];

		const auto host_frametime = std::strtof(frame_bulk.Frametime.c_str(), nullptr);
		movement_vars.Frametime = static_cast<float>(static_cast<float>(std::floor(host_frametime * 1000)) * 0.001);

		for (size_t frame = 0; frame < frame_bulk.GetRepeats(); ++frame) {
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

			player = processed_frame.NewPlayerData;

			positions.push_back(player.Origin);
			fractions.push_back(processed_frame.fractions[0]);
			normalzs.push_back(processed_frame.normalzs[0]);
		}

		total_frames += frame_bulk.GetRepeats();
		frame_bulk_starts.push_back(total_frames);

		saved_player.push_back(player);
		saved_state.push_back(strafe_state);
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
	frame_bulk_starts.erase(frame_bulk_starts.begin() + frame_bulk_index + 1, frame_bulk_starts.end());
	saved_player.erase(saved_player.begin() + frame_bulk_index + 1, saved_player.end());
	saved_state.erase(saved_state.begin() + frame_bulk_index + 1, saved_state.end());

	auto first_frame = frame_bulk_starts[frame_bulk_starts.size() - 1];
	positions.erase(positions.begin() + first_frame + 1, positions.end());
	fractions.erase(fractions.begin() + first_frame + 1, fractions.end());
	normalzs.erase(normalzs.begin() + first_frame + 1, normalzs.end());
}
