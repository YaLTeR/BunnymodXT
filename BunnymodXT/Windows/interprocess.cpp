#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include "../shared.hpp"
#include "../cvars.hpp"
#include "../interprocess.hpp"

namespace Interprocess
{
	static HANDLE pipe_tasview = INVALID_HANDLE_VALUE;
	static HANDLE pipe_bunnysplit = INVALID_HANDLE_VALUE;
	static OVERLAPPED overlapped;
	static bool writing_to_pipe;

	static void InitTASViewPipe()
	{
		pipe_tasview = CreateNamedPipe(
			"\\\\.\\pipe\\" MQ_NAME,
			PIPE_ACCESS_OUTBOUND,
			PIPE_TYPE_MESSAGE | PIPE_REJECT_REMOTE_CLIENTS,
			1,
			256 * 1000,
			0,
			0,
			NULL);
		if (pipe_tasview == INVALID_HANDLE_VALUE) {
			EngineDevWarning("Error opening the TASView pipe: %d\n", GetLastError());
			EngineWarning("TASView integration is not available.\n");
			return;
		}

		EngineDevMsg("Opened the TASView pipe.\n");
	}

	static void InitBunnySplitPipe()
	{
		pipe_bunnysplit = CreateNamedPipe(
			"\\\\.\\pipe\\" BUNNYSPLIT_PIPE_NAME,
			PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_MESSAGE | PIPE_REJECT_REMOTE_CLIENTS,
			1,
			256 * 1000,
			0,
			0,
			NULL);
		if (pipe_bunnysplit == INVALID_HANDLE_VALUE) {
			EngineDevWarning("Error opening the BunnySplit pipe: %d\n", GetLastError());
			EngineWarning("BunnySplit integration is not available.\n");
			return;
		}
		EngineDevMsg("Opened the BunnySplit pipe.\n");

		std::memset(&overlapped, 0, sizeof(overlapped));
		overlapped.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		if (overlapped.hEvent == NULL) {
			EngineDevWarning("Error creating an event for overlapped: %d. Closing the BunnySplit pipe.\n", GetLastError());
			EngineWarning("BunnySplit integration is not available.\n");
			CloseHandle(pipe_bunnysplit);
			pipe_bunnysplit = INVALID_HANDLE_VALUE;
		}
	}

	void Initialize()
	{
		InitTASViewPipe();
		InitBunnySplitPipe();
	}

	static void ShutdownTASViewPipe()
	{
		if (pipe_tasview != INVALID_HANDLE_VALUE) {
			CloseHandle(pipe_tasview);
			EngineDevMsg("Closed the TASView pipe.\n");
		}
		pipe_tasview = INVALID_HANDLE_VALUE;
	}

	static void ShutdownBunnySplitPipe()
	{
		if (pipe_bunnysplit != INVALID_HANDLE_VALUE) {
			CloseHandle(pipe_bunnysplit);
			EngineDevMsg("Closed the BunnySplit pipe.\n");
		}
		pipe_bunnysplit = INVALID_HANDLE_VALUE;

		CloseHandle(overlapped.hEvent);
		std::memset(&overlapped, 0, sizeof(overlapped));
	}

	void Shutdown()
	{
		ShutdownTASViewPipe();
		ShutdownBunnySplitPipe();
	}

	void Write(const std::vector<char>& data) {
		if (pipe_tasview == INVALID_HANDLE_VALUE)
			return;

		if (!ConnectNamedPipe(pipe_tasview, NULL)) {
			auto err = GetLastError();
			if (err == ERROR_NO_DATA) {
				// Client has disconnected.
				DisconnectNamedPipe(pipe_tasview);
				return Write(data);
			} else if (err != ERROR_PIPE_CONNECTED) {
				// Some weird error with pipe?
				// Try remaking it.
				EngineDevWarning("ConnectNamedPipe failed with %d.\n", err);
				ShutdownTASViewPipe();
				InitTASViewPipe();
				return Write(data);
			}
		}

		// Write data.
		DWORD bytesWritten;
		if (!WriteFile(pipe_tasview, data.data(), data.size(), &bytesWritten, NULL) || bytesWritten != data.size()) {
			// Once again, some weird error with pipe?
			EngineDevWarning("WriteFile failed with %d.\n", GetLastError());
			ShutdownTASViewPipe();
			InitTASViewPipe();
		}
	}

	static void WriteBunnySplit(const std::vector<char>& data)
	{
		if (pipe_bunnysplit == INVALID_HANDLE_VALUE)
			return;

		if (writing_to_pipe) {
			if (WaitForSingleObject(overlapped.hEvent, INFINITE) != WAIT_OBJECT_0) {
				// Some weird error?
				EngineDevWarning("WaitForSingleObject failed with %d.\n", GetLastError());
				DisconnectNamedPipe(pipe_bunnysplit);
				return WriteBunnySplit(data);
			}
			writing_to_pipe = false;
		}

		if (!ConnectNamedPipe(pipe_bunnysplit, &overlapped)) {
			auto err = GetLastError();
			if (err == ERROR_NO_DATA) {
				// Client has disconnected.
				DisconnectNamedPipe(pipe_bunnysplit);
				return WriteBunnySplit(data);
			} else if (err == ERROR_IO_PENDING) {
				// Waiting for someone to connect.
				return;
			} else if (err != ERROR_PIPE_CONNECTED) {
				// Some weird error with pipe?
				// Try remaking it.
				EngineDevWarning("ConnectNamedPipe failed with %d.\n", err);
				ShutdownBunnySplitPipe();
				InitBunnySplitPipe();
				return WriteBunnySplit(data);
			}
		}

		if (!WriteFile(pipe_bunnysplit, data.data(), data.size(), NULL, &overlapped)) {
			auto err = GetLastError();
			if (err == ERROR_IO_PENDING) {
				// Started writing.
				writing_to_pipe = true;
				return;
			} else {
				EngineDevWarning("WriteFile failed with %d.\n", err);
				DisconnectNamedPipe(pipe_bunnysplit);
				return;
			}
		}
	}

	static size_t AddTimeToBuffer(char* buf, const Time& time)
	{
		std::memcpy(buf, &time.hours, sizeof(time.hours));
		std::memcpy(buf + sizeof(time.hours), &time.minutes, sizeof(time.minutes));
		std::memcpy(buf + sizeof(time.hours) + sizeof(time.minutes), &time.seconds, sizeof(time.seconds));
		std::memcpy(buf + sizeof(time.hours) + sizeof(time.minutes) + sizeof(time.seconds), &time.milliseconds, sizeof(time.milliseconds));
		return sizeof(time.hours) + sizeof(time.minutes) + sizeof(time.seconds) + sizeof(time.milliseconds);
	}

	void WriteTime(const Time& time)
	{
		std::vector<char> buf(10);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::TIME);
		AddTimeToBuffer(buf.data() + 2, time);

		if (CVars::_bxt_bunnysplit_time_update_frequency.GetFloat() > 0.0f) {
			static auto last_time = std::chrono::steady_clock::now() - std::chrono::milliseconds(static_cast<long long>(1000 / CVars::_bxt_bunnysplit_time_update_frequency.GetFloat()) + 1);
			auto now = std::chrono::steady_clock::now();
			if (now >= last_time + std::chrono::milliseconds(static_cast<long long>(1000 / CVars::_bxt_bunnysplit_time_update_frequency.GetFloat()))) {
				WriteBunnySplit(buf);
				last_time = now;
			}
		} else {
			WriteBunnySplit(buf);
		}

		if (!CVars::bxt_interprocess_enable.GetBool())
			return;

		Write(buf);
	}

	void WriteClip(float normal_z, float vel_in[3], float vel_out[3])
	{
		if (!CVars::bxt_interprocess_enable.GetBool())
			return;

		std::vector<char> buf(30);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::CLIP);
		std::memcpy(buf.data() + 2, &normal_z, sizeof(normal_z));
		std::memcpy(buf.data() + 6, vel_in, sizeof(*vel_in) * 3);
		std::memcpy(buf.data() + 18, vel_out, sizeof(*vel_out) * 3);
		Write(buf);
	}

	void WriteWater()
	{
		if (!CVars::bxt_interprocess_enable.GetBool())
			return;

		std::vector<char> buf = { 2, static_cast<char>(MessageType::WATER) };
		Write(buf);
	}

	void WriteFrametimeRemainder(double frametime_remainder)
	{
		if (!CVars::bxt_interprocess_enable.GetBool())
			return;

		std::vector<char> buf(10);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::FRAMETIME_REMAINDER);
		std::memcpy(buf.data() + 2, &frametime_remainder, sizeof(frametime_remainder));
		Write(buf);
	}

	void WriteGameEnd(const Time& time)
	{
		std::vector<char> buf(11);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::EVENT);
		buf[2] = static_cast<char>(EventType::GAMEEND);
		AddTimeToBuffer(buf.data() + 3, time);

		WriteBunnySplit(buf);
	}

	void WriteMapChange(const Time& time, const std::string& map)
	{
		uint32_t size = static_cast<uint32_t>(map.size());

		std::vector<char> buf(15 + size);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::EVENT);
		buf[2] = static_cast<char>(EventType::MAPCHANGE);
		auto time_size = AddTimeToBuffer(buf.data() + 3, time);

		std::memcpy(buf.data() + 3 + time_size, &size, sizeof(size));
		std::memcpy(buf.data() + 3 + time_size + 4, map.data(), size);

		WriteBunnySplit(buf);
	}

	void WriteTimerReset(const Time& time)
	{
		std::vector<char> buf(11);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::EVENT);
		buf[2] = static_cast<char>(EventType::TIMER_RESET);
		AddTimeToBuffer(buf.data() + 3, time);

		WriteBunnySplit(buf);
	}

	void WriteTimerStart(const Time& time)
	{
		std::vector<char> buf(11);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::EVENT);
		buf[2] = static_cast<char>(EventType::TIMER_START);
		AddTimeToBuffer(buf.data() + 3, time);

		WriteBunnySplit(buf);
	}

	void WriteBSALeapOfFaith(const Time& time)
	{
		std::vector<char> buf(11);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::EVENT);
		buf[2] = static_cast<char>(EventType::BS_ALEAPOFFAITH);
		AddTimeToBuffer(buf.data() + 3, time);

		WriteBunnySplit(buf);
	}
}
