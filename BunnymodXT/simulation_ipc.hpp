#pragma once

#include <string>

namespace simulation_ipc {
	// Frame data sent from the simulator client to the server.
	struct SimulatedFrame {
		unsigned generation;
		size_t number;

		HLStrafe::PlayerData player_data;
		HLStrafe::CurrentState strafe_state;
		float fraction;
		float normalz;
		bool next_frame_is_0ms;

		float health;
		float armor;
	};

	// Console command received from the server to be run.
	extern std::string command_to_run;

	// Initializes this game as the server if not already initialized.
	// Returns a non-empty error message on error.
	//
	// If already initialized as client, returns an error and does nothing.
	std::string initialize_server_if_needed();
	// Returns `true` if the server part was initialized.
	bool is_server_initialized();
	// Sends a console command to the connected client.
	void send_command_to_client(const std::string &command);
	// Receives and handles messages from the client.
	void receive_messages_from_client();

	// Initializes this game as the client, after closing the existing client
	// connection, if any. Returns a non-empty error message on error.
	//
	// If already initialized as server, returns an error and does nothing.
	std::string initialize_client();
	// Returns `true` if the client part was initialized.
	bool is_client_initialized();
	// Receives and handles messages from the server.
	void receive_messages_from_server();
	// Sends a simulated frame data to the server.
	void send_simulated_frame_to_server(SimulatedFrame frame);

	// Locks the mutex for accessing the simulation .hltas file.
	//
	// Used for preventing data races between the server writing the file and
	// the client reading it.
	void maybe_lock_mutex();
	// Unlocks the mutex for accessing the simulation .hltas file.
	//
	// Used for preventing data races between the server writing the file and
	// the client reading it.
	void maybe_unlock_mutex();
}
