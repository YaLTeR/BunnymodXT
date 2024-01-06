#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include "../shared.hpp"
#include "../cvars.hpp"
#include "../interprocess.hpp"

namespace Interprocess
{
	mqd_t mq = static_cast<mqd_t>(-1);
	mqd_t mq_bunnysplit = static_cast<mqd_t>(-1);

	void Initialize()
	{
		mq = mq_open(
			"/" MQ_NAME,
			O_WRONLY | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
			nullptr);

		if (mq == static_cast<mqd_t>(-1)) {
			EngineDevWarning("Error opening the message queue: %s\n", strerror(errno));
			EngineWarning("TASView integration is not available.\n");
			return;
		}

		EngineDevMsg("Opened the message queue.\n");
	}

	void Shutdown()
	{
		if (mq != static_cast<mqd_t>(-1)) {
			mq_close(mq);
			mq_unlink("/" MQ_NAME);
			EngineDevMsg("Closed the message queue.\n");
		}

		mq = static_cast<mqd_t>(-1);
	}

	void Write(const std::vector<char>& data)
	{
		if (mq == static_cast<mqd_t>(-1))
			return;

		if (mq_send(mq, data.data(), data.size(), 0) == -1) {
			// This shouldn't really happen.
			// Perhaps the message queue was force-deleted or something.
			EngineDevWarning("mq_send failed with %d (%s).\n", errno, strerror(errno));
			Shutdown();
			Initialize();
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
			auto ms = static_cast<long long>(1000 / CVars::_bxt_bunnysplit_time_update_frequency.GetFloat());
			static auto last_time = std::chrono::steady_clock::now() - std::chrono::milliseconds(ms + 1);
			auto now = std::chrono::steady_clock::now();
			if (now >= last_time + std::chrono::milliseconds(ms)) {
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
		int32_t size = static_cast<int32_t>(map.size());

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

	void InitBunnySplitPipe()
	{
		mq_bunnysplit = mq_open(
				"/" BUNNYSPLIT_MQ_NAME,
				O_WRONLY | O_CREAT | O_NONBLOCK,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
				nullptr);
		
		if (mq_bunnysplit == static_cast<mqd_t>(-1)) {
			EngineDevWarning("Error opening the BunnySplit message queue: %s\n", strerror(errno));
			EngineWarning("BunnySplit integration is not available.\n");
			return;
		}

		EngineDevMsg("Opened the BunnySplit message queue.\n");
	}

	void ShutdownBunnySplitPipe()
	{
		if (mq_bunnysplit != static_cast<mqd_t>(-1)) {
			mq_close(mq_bunnysplit);
			mq_unlink("/" BUNNYSPLIT_MQ_NAME);
			EngineDevMsg("Closed the BunnySplit message queue.\n");
		}

		mq_bunnysplit = static_cast<mqd_t>(-1);
	}

	void WriteBunnySplit(const std::vector<char>& data)
	{
		if (mq_bunnysplit == static_cast<mqd_t>(-1))
			return;

		if (mq_send(mq_bunnysplit, data.data(), data.size(), 0) == -1) {
			// This shouldn't really happen.
			// Perhaps the message queue was force-deleted or something.
			EngineDevWarning("[mq_bunnysplit] mq_send failed with %d (%s).\n", errno, strerror(errno));
			ShutdownBunnySplitPipe();
			InitBunnySplitPipe();
		}
	}
}
