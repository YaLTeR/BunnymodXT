#pragma once

#include "interprocess.hpp"

namespace CustomHud
{
	typedef struct
	{
		float origin[3];
		float velocity[3];
		float viewangles[3];
	} playerinfo;

	void Init();
	void InitIfNecessary();
	void VidInit();
	void Draw(float flTime);

	void UpdatePlayerInfo(float vel[3], float org[3]);
	void UpdatePlayerInfoInaccurate(float vel[3], float org[3]);

	void TimePassed(double time);
	void ResetTime();
	void SetCountingTime(bool counting);
	void SendTimeUpdate();
	void SaveTimeToDemo();
	Interprocess::Time GetTime();

	const SCREENINFO& GetScreenInfo();
};
