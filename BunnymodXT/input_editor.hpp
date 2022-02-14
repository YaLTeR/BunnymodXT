#pragma once

#include "BunnymodXT/simulation_ipc.hpp"

struct EditedInput {
	// Per-frame-bulk data.

	std::vector<HLTAS::Frame> frame_bulks;
	// Frame numbers of the first frame of each frame bulk, and the total frame count as the last entry.
	std::vector<size_t> frame_bulk_starts;

	// Per-frame data.

	std::vector<HLStrafe::PlayerData> player_datas;
	std::vector<float> player_health_datas;
	std::vector<float> player_armor_datas;
	// Every strafe state must be good to go for simulating subsequent frame bulks without any changes.
	std::vector<HLStrafe::CurrentState> strafe_states;
	std::vector<std::array<float, 4>> fractions;
	std::vector<std::array<float, 4>> normalzs;
	std::vector<char> next_frame_is_0mss; // char instead of bool because C++ vector bool bitset.
	std::vector<float> frametimes;
	std::vector<std::vector<float>> target_yaw_overrides;
	std::vector<size_t> active_target_yaw_override_indices; // 0 means none.

	HLStrafe::MovementVars initial_movement_vars;

	size_t first_predicted_frame;
	unsigned current_generation;
	size_t first_frame_counter_value;
	std::chrono::steady_clock::time_point run_in_second_game_at;
	bool received_data_from_second_game;

	// Clears and initializes EditedInput with the current player data.
	void initialize();
	void simulate();
	HLTAS::ErrorDescription save(const std::string &filename) const;
	void mark_as_stale(size_t frame_bulk_index);

	// Sets the repeats for the frame bulk at the given index.
	//
	// Invalidates the cached positions in an optimal way to reduce unnecessary simulations.
	void set_repeats(size_t frame_bulk_index, unsigned repeats);

	void schedule_run_in_second_game();
	void run_script_in_second_game();
	void received_simulated_frame(const simulation_ipc::SimulatedFrame &frame);
};
