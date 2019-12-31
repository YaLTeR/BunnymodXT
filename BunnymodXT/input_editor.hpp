#pragma once

enum class SimulateFrameBulks {
	ALL_EXCEPT_LAST,
	LAST
};

struct EditedInput {
	std::vector<HLTAS::Frame> frame_bulks;
	// Frame numbers of the first frame of each frame bulk, and the total frame count as the last entry.
	std::vector<size_t> frame_bulk_starts;
	std::vector<Vector> positions;
	std::vector<float> fractions;
	std::vector<float> normalzs;

	HLStrafe::PlayerData saved_player;
	HLStrafe::CurrentState saved_state;

	void simulate(SimulateFrameBulks what);
	void save();
};
