#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include "../shared.hpp"
#include "../cvars.hpp"
#include "../interprocess.hpp"

namespace Interprocess
{
	mqd_t mq = static_cast<mqd_t>(-1);

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
		if (!CVars::bxt_interprocess_enable.GetBool())
			return;

		std::vector<char> buf(10);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::TIME);
		AddTimeToBuffer(buf.data() + 2, time);

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
	}

	void WriteMapChange(const Time& time, const std::string& map)
	{
	}

	void WriteTimerReset(const Time& time)
	{
	}

	void WriteTimerStart(const Time& time)
	{
	}

	void WriteBSALeapOfFaith(const Time& time)
	{
	}

	std::string ReadLastSplitDelta() {
		return "-";
	}
}
