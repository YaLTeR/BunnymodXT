#pragma once

namespace Interprocess
{
	struct Time
	{
		int hours;
		int minutes;
		int seconds;
		int milliseconds;
	};

	void Initialize();
	void Shutdown();
	void Write(const std::vector<char>& data);

	void WriteTime(const Time& time);
	void WriteClip(float normal_z, float vel_in[3], float vel_out[3]);
	void WriteWater();
	void WriteFrametimeRemainder(double frametime_remainder);

	void WriteGameEnd(const Time& time);
	void WriteMapChange(const Time& time, std::string map);
	void WriteTimerReset(const Time& time);
	void WriteTimerStart(const Time& time);
}
