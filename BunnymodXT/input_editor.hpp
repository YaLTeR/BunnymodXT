#pragma once

enum class SimulateFrameBulks {
	ALL,
	ALL_EXCEPT_LAST
};

struct EditedInput {
	std::vector<HLTAS::Frame> frame_bulks;
	// Frame numbers of the first frame of each frame bulk, and the total frame count as the last entry.
	std::vector<size_t> frame_bulk_starts;
	std::vector<Vector> positions;
	std::vector<float> fractions;
	std::vector<float> normalzs;

	// PlayerData after simulating every frame bulk.
	std::vector<HLStrafe::PlayerData> player_datas;
	// CurrentState after simulating every frame bulk.
	std::vector<HLStrafe::CurrentState> strafe_states;

	// Last PlayerData and CurrentState.
	//
	// These will generally be the same as the last entry in player_datas, but they will be newer
	// during incremental simulation.
	HLStrafe::PlayerData saved_player;
	HLStrafe::CurrentState saved_state;
	// The next frame to simulate in the current frame bulk.
	size_t saved_frame;
	bool saved_next_frame_is_0ms;

	HLStrafe::MovementVars initial_movement_vars;

	// Clears and initializes EditedInput with the current player data.
	void initialize();
	void simulate(SimulateFrameBulks what);
	void save();
	void mark_as_stale(size_t frame_bulk_index);
};
