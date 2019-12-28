#include "stdafx.hpp"

#include "input_editor.hpp"
#include "modules.hpp"

void EditedInput::simulate() {
	auto& hw = HwDLL::GetInstance();

	positions.clear();
	fractions.clear();
	normalzs.clear();

	auto player = hw.GetPlayerData();
	positions.push_back(player.Origin);
	fractions.push_back(1);
	normalzs.push_back(0);

	const auto movement_vars = hw.GetMovementVars();
	const auto frametime = movement_vars.Frametime;

	auto strafe_state = hw.StrafeState;
	strafe_state.Jump = hw.currentKeys.Jump.IsDown();
	strafe_state.Duck = hw.currentKeys.Duck.IsDown();

	hw.StartTracing();

	const auto trace_func = std::bind(
		&HwDLL::UnsafePlayerTrace,
		&hw,
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	);

	for (const auto& frame_bulk : frame_bulks) {
		for (size_t frame = 0; frame < frame_bulk.GetRepeats(); ++frame) {
			auto processed_frame = HLStrafe::MainFunc(
				player,
				movement_vars,
				frame_bulk,
				strafe_state,
				hw.Buttons,
				hw.ButtonsPresent,
				trace_func
			);

			player = processed_frame.NewPlayerData;

			positions.push_back(player.Origin);
			fractions.push_back(processed_frame.fractions[0]);
			normalzs.push_back(processed_frame.normalzs[0]);
		}
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
