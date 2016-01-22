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
}
