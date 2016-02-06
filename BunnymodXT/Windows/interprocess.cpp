#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include "../shared.hpp"
#include "../interprocess.hpp"

namespace Interprocess
{
	HANDLE pipe = INVALID_HANDLE_VALUE;

	void Initialize()
	{
		pipe = CreateNamedPipe(
			"\\\\.\\pipe\\" MQ_NAME,
			PIPE_ACCESS_OUTBOUND,
			PIPE_TYPE_MESSAGE | PIPE_REJECT_REMOTE_CLIENTS,
			1,
			256 * 1000,
			0,
			0,
			NULL);
		if (pipe == INVALID_HANDLE_VALUE) {
			EngineDevWarning("Error opening the pipe: %d\n", GetLastError());
			EngineWarning("TASView integration is not available.\n");
			return;
		}

		EngineDevMsg("Opened the pipe.\n");
	}

	void Shutdown()
	{
		if (pipe != INVALID_HANDLE_VALUE) {
			CloseHandle(pipe);
			EngineDevMsg("Closed the pipe.\n");
		}
		pipe = INVALID_HANDLE_VALUE;
	}

	void Write(const std::vector<char>& data) {
		if (pipe == INVALID_HANDLE_VALUE)
			return;

		if (!ConnectNamedPipe(pipe, NULL)) {
			auto err = GetLastError();
			if (err == ERROR_NO_DATA) {
				// Client has disconnected.
				DisconnectNamedPipe(pipe);
				return Write(data);
			} else if (err != ERROR_PIPE_CONNECTED) {
				// Some weird error with pipe?
				// Try remaking it.
				EngineDevWarning("ConnectNamedPipe failed with %d.\n", err);
				Shutdown();
				Initialize();
				return Write(data);
			}
		}

		// Write data.
		DWORD bytesWritten;
		if (!WriteFile(pipe, data.data(), data.size(), &bytesWritten, NULL) || bytesWritten != data.size()) {
			// Once again, some weird error with pipe?
			EngineDevWarning("WriteFile failed with %d.\n", GetLastError());
			Shutdown();
			Initialize();
		}
	}

	void WriteTime(int hours, int minutes, int seconds, int milliseconds)
	{
		std::vector<char> buf(18);
		buf[0] = 18;
		buf[1] = static_cast<char>(MessageType::TIME);
		std::memcpy(buf.data() + 2, &hours, sizeof(hours));
		std::memcpy(buf.data() + 6, &minutes, sizeof(minutes));
		std::memcpy(buf.data() + 10, &seconds, sizeof(seconds));
		std::memcpy(buf.data() + 14, &milliseconds, sizeof(milliseconds));
		Write(buf);
	}

	void WriteClip(float normal_z, float vel_in[3], float vel_out[3])
	{
		std::vector<char> buf(30);
		buf[0] = 30;
		buf[1] = static_cast<char>(MessageType::CLIP);
		std::memcpy(buf.data() + 2, &normal_z, sizeof(normal_z));
		std::memcpy(buf.data() + 6, vel_in, sizeof(*vel_in) * 3);
		std::memcpy(buf.data() + 18, vel_out, sizeof(*vel_out) * 3);
		Write(buf);
	}

	void WriteWater()
	{
		std::vector<char> buf = { 2, static_cast<char>(MessageType::WATER) };
		Write(buf);
	}

	void WriteFrametimeRemainder(double frametime_remainder)
	{
		std::vector<char> buf(10);
		buf[0] = 10;
		buf[1] = static_cast<char>(MessageType::FRAMETIME_REMAINDER);
		std::memcpy(buf.data() + 2, &frametime_remainder, sizeof(frametime_remainder));
		Write(buf);
	}
}
