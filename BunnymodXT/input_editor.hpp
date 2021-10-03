#pragma once

#include "BunnymodXT/simulation_ipc.hpp"

struct EditedInput {
	// Per-frame-bulk data.

	std::vector<HLTAS::Frame> frame_bulks;
	// Frame numbers of the first frame of each frame bulk, and the total frame count as the last entry.
	std::vector<size_t> frame_bulk_starts;

	// Per-frame data.

	std::vector<HLStrafe::PlayerData> player_datas;
	std::vector<HLStrafe::CurrentState> strafe_states;
	std::vector<float> fractions;
	std::vector<float> normalzs;
	std::vector<char> next_frame_is_0mss; // char instead of bool because C++ vector bool bitset.

	HLStrafe::MovementVars initial_movement_vars;

	size_t first_predicted_frame;
	unsigned current_generation;
	size_t first_frame_counter_value;
	std::chrono::steady_clock::time_point run_in_second_game_at;

	// Clears and initializes EditedInput with the current player data.
	void initialize();
	void simulate();
	HLTAS::ErrorDescription save(const std::string &filename) const;
	void mark_as_stale(size_t frame_bulk_index);

	// Sets the repeats for the frame bulk at the given index.
	//
	// Invalidates the cached positions in an optimal way to reduce unnecessary simulations.
	void set_repeats(size_t frame_bulk_index, unsigned repeats);

	// Returns true if all frames were simulated.
	bool simulated_all_frames() const;

	void schedule_run_in_second_game();
	void run_script_in_second_game();
	void received_simulated_frame(const simulation_ipc::SimulatedFrame &frame);
};
