#pragma once

namespace Interprocess
{
	struct Time
	{
		uint32_t hours;
		uint8_t minutes;
		uint8_t seconds;
		uint16_t milliseconds;
	};

	void Initialize();
	void Shutdown();
	void Write(const std::vector<char>& data);

	void WriteTime(const Time& time);
	void WriteClip(float normal_z, float vel_in[3], float vel_out[3]);
	void WriteWater();
	void WriteFrametimeRemainder(double frametime_remainder);

	void WriteGameEnd(const Time& time);
	void WriteMapChange(const Time& time, const std::string& map);
	void WriteTimerReset(const Time& time);
	void WriteTimerStart(const Time& time);
	void WriteBSALeapOfFaith(const Time& time);
}
