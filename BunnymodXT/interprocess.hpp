#pragma once

namespace Interprocess
{
	void Initialize();
	void Shutdown();
	void Write(const std::vector<char>& data);

	void WriteTime(int hours, int minutes, int seconds, int milliseconds);
	void WriteClip(float normal_z, float vel_in[3], float vel_out[3]);
	void WriteWater();
	void WriteFrametimeRemainder(double frametime_remainder);
}
