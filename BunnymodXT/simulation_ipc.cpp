#include "stdafx.hpp"

#include "simulation_ipc.hpp"
#include "modules/HwDLL.hpp"

#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

using boost::interprocess::message_queue;
using boost::interprocess::named_mutex;
using boost::interprocess::interprocess_exception;
using std::unique_ptr;
using std::make_unique;
using std::cerr;
using std::endl;
using namespace std::literals;

#define SERVER_TO_CLIENT_MQ_NAME "bxt-simulation-server-to-client"
#define CLIENT_TO_SERVER_MQ_NAME "bxt-simulation-client-to-server"
#define MUTEX_NAME "bxt-simulation-mutex"

namespace simulation_ipc {
	std::string command_to_run;

	enum ServerToClientMessageType {
		CONSOLE_COMMAND = 0,
	};

	struct ServerToClientMessage {
		ServerToClientMessageType type;

		char command[1024];
	};

	enum ClientToServerMessageType {
		SIMULATED_FRAME = 0,
	};

	struct ClientToServerMessage {
		ClientToServerMessageType type;

		SimulatedFrame frame;
	};

	static unique_ptr<message_queue> server_tx;
	static unique_ptr<message_queue> server_rx;
	static unique_ptr<message_queue> client_tx;
	static unique_ptr<message_queue> client_rx;
	static unique_ptr<named_mutex> mutex;

	void remove() {
		server_tx.reset();
		server_rx.reset();
		mutex.reset();
		message_queue::remove(SERVER_TO_CLIENT_MQ_NAME);
		message_queue::remove(CLIENT_TO_SERVER_MQ_NAME);
		named_mutex::remove(MUTEX_NAME);
	}

	void close_client() {
		client_rx.reset();
		client_tx.reset();
		mutex.reset();
	}

	void initialize_server_if_needed() {
		if (is_client_initialized())
			close_client();

		if (is_server_initialized())
			return;

		remove();

		try {
			server_tx = make_unique<message_queue>(
				boost::interprocess::create_only,
				SERVER_TO_CLIENT_MQ_NAME,
				10,
				sizeof(ServerToClientMessage));
		} catch (interprocess_exception &ex) {
			remove();
			cerr << "error creating server-to-client message queue: " << ex.what() << endl;
			return;
		}

		try {
			server_rx = make_unique<message_queue>(
				boost::interprocess::create_only,
				CLIENT_TO_SERVER_MQ_NAME,
				10000,
				sizeof(ClientToServerMessage));
		} catch (interprocess_exception &ex) {
			remove();
			cerr << "error creating client-to-server message queue: " << ex.what() << endl;
			return;
		}

		try {
			mutex = make_unique<named_mutex>(
				boost::interprocess::create_only,
				MUTEX_NAME);
		} catch (interprocess_exception &ex) {
			remove();
			cerr << "error creating mutex: " << ex.what() << endl;
			return;
		}
	}

	bool is_server_initialized() {
		return !!server_tx;
	}

	void send_command_to_client(const std::string &command) {
		if (!is_server_initialized()) {
			cerr << "server is not initialized" << endl;
			return;
		}

		if (command.length() >= 1024) {
			cerr << "command is too long (" << command.length() << " >= 1024)" << endl;
			return;
		}

		if (command.empty()) {
			cerr << "command is empty" << endl;
			return;
		}

		if (command[command.length() - 1] != '\n') {
			cerr << "command does not end in \\n" << endl;
			return;
		}

		ServerToClientMessage message {
			ServerToClientMessageType::CONSOLE_COMMAND,
			"",
		};

		strncpy(message.command, command.c_str(), 1023);

		try {
			server_tx->try_send(&message, sizeof(message), 0);
		} catch (interprocess_exception &ex) {
			remove();
			cerr << "error sending message from server: " << ex.what() << endl;
			return;
		}
	}

	void receive_messages_from_client() {
		if (!is_server_initialized())
			return;

		auto& hw = HwDLL::GetInstance();

		try {
			while (true) {
				ClientToServerMessage message;
				message_queue::size_type recvd_size;
				unsigned int priority;
				if (!server_rx->try_receive(&message, sizeof(message), recvd_size, priority)) {
					// The queue is empty.
					return;
				}

				if (recvd_size != sizeof(message)) {
					remove();
					cerr << "server received message of incorrect size: expected " << sizeof(message)
						 << ", got " << recvd_size << endl;
					return;
				}

				switch (message.type) {
				case ClientToServerMessageType::SIMULATED_FRAME:
					hw.tas_editor_input.received_simulated_frame(message.frame);
					break;
				default:
					remove();
					cerr << "server received message with unexpected type: " << message.type << endl;
					return;
				}
			}
		} catch (interprocess_exception &ex) {
			remove();
			cerr << "error receiving message on the server: " << ex.what() << endl;
			return;
		}
	}

	std::string initialize_client() {
		if (is_server_initialized())
			return "already initialized as server"s;

		close_client();

		try {
			client_rx = make_unique<message_queue>(
				boost::interprocess::open_only,
				SERVER_TO_CLIENT_MQ_NAME);
		} catch (interprocess_exception &ex) {
			close_client();
			return "error opening server-to-client message queue: "s + ex.what();
		}

		try {
			client_tx = make_unique<message_queue>(
				boost::interprocess::open_only,
				CLIENT_TO_SERVER_MQ_NAME);
		} catch (interprocess_exception &ex) {
			close_client();
			return "error opening client-to-server message queue: "s + ex.what();
		}

		try {
			mutex = make_unique<named_mutex>(
				boost::interprocess::open_only,
				MUTEX_NAME);
		} catch (interprocess_exception &ex) {
			close_client();
			return "error opening mutex: "s + ex.what();
		}

		return std::string();
	}

	bool is_client_initialized() {
		return !!client_tx;
	}

	void receive_messages_from_server() {
		if (!is_client_initialized())
			return;

		try {
			while (true) {
				ServerToClientMessage message;
				message_queue::size_type recvd_size;
				unsigned int priority;
				if (!client_rx->try_receive(&message, sizeof(message), recvd_size, priority)) {
					// The queue is empty.
					return;
				}

				if (recvd_size != sizeof(message)) {
					cerr << "client received message of incorrect size: expected " << sizeof(message)
						 << ", got " << recvd_size << endl;
					close_client();
					return;
				}

				switch (message.type) {
				case ServerToClientMessageType::CONSOLE_COMMAND:
					command_to_run = message.command;
					break;
				default:
					close_client();
					cerr << "client received message with unexpected type: " << message.type << endl;
					return;
				}
			}
		} catch (interprocess_exception &ex) {
			close_client();
			cerr << "error receiving message on the client: " << ex.what() << endl;
			return;
		}
	}

	void send_simulated_frame_to_server(SimulatedFrame frame) {
		if (!is_client_initialized()) {
			return;
		}

		ClientToServerMessage message {
			ClientToServerMessageType::SIMULATED_FRAME,
			frame
		};

		try {
			client_tx->send(&message, sizeof(message), 0);
		} catch (interprocess_exception &ex) {
			close_client();
			cerr << "error sending message from client: " << ex.what() << endl;
			return;
		}
	}

	void maybe_lock_mutex() {
		if (mutex)
			mutex->lock();
	}

	void maybe_unlock_mutex() {
		if (mutex)
			mutex->unlock();
	}
}
