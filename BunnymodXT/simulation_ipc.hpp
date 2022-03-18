#pragma once

#include <string>

namespace simulation_ipc {
	// Frame data sent from the simulator client to the server. Must be plain old data.
	struct SimulatedFrame {
		unsigned generation;
		size_t number;

		HLStrafe::PlayerData player_data;
		HLStrafe::CurrentState strafe_state;
		std::array<float, 4> fractions;
		std::array<float, 4> normalzs;
		bool next_frame_is_0ms;
		float frametime;

		float health;
		float armor;
	};

	enum ServerToClientMessageType {
		CONSOLE_COMMAND_WITH_SCRIPT = 0,
	};

	struct ServerToClientMessage {
		ServerToClientMessageType type;

		char command[1024];

		// 1 MiB should cover reasonably large scripts.
		// If it starts with a NULL byte then there's no script, simply run the command.
		char script[1 * 1024 * 1024];
	};

	// On the client, the last received message. On the server, the message to send.
	extern ServerToClientMessage message;

	// Initializes this game as the server if not already initialized.
	// Returns a non-empty error message on error.
	//
	// If already initialized as client, returns an error and does nothing.
	std::string initialize_server_if_needed();
	// Returns `true` if the server part was initialized.
	bool is_server_initialized();
	// Writes a console command to message.
	bool write_command(const std::string &command);
	// Sends message to the client.
	void send_message_to_client();
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
