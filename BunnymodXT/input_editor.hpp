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
	std::vector<HLStrafe::PlayerData> saved_player;
	// CurrentState after simulating every frame bulk.
	std::vector<HLStrafe::CurrentState> saved_state;

	HLStrafe::MovementVars initial_movement_vars;

	// Clears and initializes EditedInput with the current player data.
	void initialize();
	void simulate(SimulateFrameBulks what);
	void save();
	void mark_as_stale(size_t frame_bulk_index);
};
