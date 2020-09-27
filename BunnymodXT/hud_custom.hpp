#pragma once

#include "interprocess.hpp"
#include "modules/ServerDLL.hpp"

namespace CustomHud
{
	constexpr std::array<std::pair<int, const char*>, 7> SOUNDENT_STRINGS{ {
		{SOUND_BITS_COMBAT, "combat"},
		{SOUND_BITS_WORLD, "world"},
		{SOUND_BITS_PLAYER, "player"},
		{SOUND_BITS_CARCASS, "carcass"},
		{SOUND_BITS_MEAT, "meat"},
		{SOUND_BITS_DANGER, "danger"},
		{SOUND_BITS_GARBAGE, "garbage"},
	} };

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

	void UpdateTASEditorStatus(const HLTAS::Frame& frame_bulk);

	bool IgnoreSound(int soundType, const std::vector<std::string_view>& ignores);
};
