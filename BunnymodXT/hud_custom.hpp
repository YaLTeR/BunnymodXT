#pragma once

#include "interprocess.hpp"

namespace CustomHud
{
	typedef struct
	{
		float origin[3];
		float velocity[3];
		float viewangles[3];
		float health;
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

	void UpdateTASEditorStatus(const HLTAS::Frame& frame_bulk, const float& player_vel, const float& player_zvel, const float& player_zpos, const float& player_realyaw);
};
