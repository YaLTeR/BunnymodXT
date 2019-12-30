#pragma once

struct EditedInput {
	std::vector<HLTAS::Frame> frame_bulks;
	// Frame numbers of the first frame of each frame bulk, and the total frame count as the last entry.
	std::vector<size_t> frame_bulk_starts;
	std::vector<Vector> positions;
	std::vector<float> fractions;
	std::vector<float> normalzs;

	void simulate();
	void save();
};
