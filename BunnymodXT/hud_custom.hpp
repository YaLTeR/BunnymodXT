#pragma once

namespace CustomHud
{
	typedef struct
	{
		float origin[3];
		float velocity[3];
	} playerinfo;

	void Init();
	void InitIfNecessary();
	void VidInit();
	void Draw(float flTime);

	void UpdatePlayerInfo(float vel[3], float org[3]);
	void UpdatePlayerInfoInaccurate(float vel[3], float org[3]);
};
