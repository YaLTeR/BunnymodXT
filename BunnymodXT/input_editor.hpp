#pragma once

struct EditedInput {
	std::vector<HLTAS::Frame> frame_bulks;
	std::vector<Vector> positions;
	std::vector<float> fractions;
	std::vector<float> normalzs;

	void simulate();
	void save();
};
