#include "stdafx.hpp"

#include "cvars.hpp"
#include "modules.hpp"
#include <SPTLib/Hooks.hpp>
#include "hud_custom.hpp"
#include "interprocess.hpp"

#include <chrono>

namespace CustomHud
{
	static const float FADE_DURATION_JUMPSPEED = 0.7f;

	static bool initialized = false;

	static SCREENINFO si;
	static int precision;
	static float consoleColor[3];
	static int hudColor[3];
	static bool receivedAccurateInfo = false;
	static playerinfo player;
	bool countingTime;
	int hours, minutes, seconds;
	double timeRemainder;
	int frames = 0;

	static client_sprite_t *SpriteList;
	static int SpriteCount;
	static std::array<HSPRITE_HL, 10> NumberSprites;
	static std::array<wrect_t, 10> NumberSpriteRects;
	static std::array<client_sprite_t*, 10> NumberSpritePointers;
	static int NumberWidth;
	static int NumberHeight;

	template<typename T, size_t size = 3>
	static inline void vecCopy(const T src[], T dest[])
	{
		for (size_t i = 0; i < size; ++i)
			dest[i] = src[i];
	}

	static inline double sqr(double a)
	{
		return a * a;
	}

	static inline double length(double x, double y)
	{
		return std::hypot(x, y);
	}

	static inline double length(double x, double y, double z)
	{
		return std::sqrt(sqr(x) + sqr(y) + sqr(z));
	}

	static void UpdateScreenInfo()
	{
		si.iSize = sizeof(si);
		ClientDLL::GetInstance().pEngfuncs->pfnGetScreenInfo(&si);
	}

	static void DrawString(int x, int y, const char* s, float r, float g, float b)
	{
		ClientDLL::GetInstance().pEngfuncs->pfnDrawSetTextColor(r, g, b);
		ClientDLL::GetInstance().pEngfuncs->pfnDrawConsoleString(x, y, const_cast<char*>(s));
	}

	static inline void DrawString(int x, int y, const char* s)
	{
		DrawString(x, y, s, consoleColor[0], consoleColor[1], consoleColor[2]);
	}

	static void DrawMultilineString(int x, int y, std::string s)
	{
		while (s.size() > 0)
		{
			auto pos = s.find('\n');

			DrawString(x, y, const_cast<char*>(s.substr(0, pos).c_str()));
			y += si.iCharHeight;

			if (pos != std::string::npos)
				s = s.substr(pos + 1, std::string::npos);
			else
				s.erase();
		};
	}

	static void DrawDigit(int digit, int x, int y, int r, int g, int b)
	{
		assert(digit >= 0 && digit <= 9);

		ClientDLL::GetInstance().pEngfuncs->pfnSPR_Set(NumberSprites[digit], r, g, b);
		ClientDLL::GetInstance().pEngfuncs->pfnSPR_DrawAdditive(0, x, y, &NumberSpriteRects[digit]);
	}

	static int DrawNumber(int number, int x, int y, int r, int g, int b, int fieldMinWidth = 1)
	{
		if (number < 0)
		{
			if (number == std::numeric_limits<int>::min())
				number = 0;
			else
				number = abs(number);

			// TODO: draw a minus sign.
		}

		static_assert(sizeof(int) >= 4, "Int less than 4 bytes in size is not supported.");

		int digits[10] = { 0 };
		int i;
		for (i = 0; i < 10; ++i)
		{
			if (number == 0)
				break;

			digits[i] = number % 10;
			number /= 10;
		}

		for (; fieldMinWidth > 10; --fieldMinWidth)
		{
			DrawDigit(0, x, y, r, g, b);
			x += NumberWidth;
		}
		if (fieldMinWidth > i)
			i = fieldMinWidth;

		for (int j = i; j > 0; --j)
		{
			DrawDigit(digits[j - 1], x, y, r, g, b);
			x += NumberWidth;
		}

		return x;
	}

	static inline int DrawNumber(int number, int x, int y, int fieldMinWidth = 1)
	{
		return DrawNumber(number, x, y, hudColor[0], hudColor[1], hudColor[2], fieldMinWidth);
	}

	static void DrawDot(int x, int y, int r, int g, int b)
	{
		const int Dot320[] = {
			143, 199, 122,
			255, 255, 218,
			120, 169, 95
		};
		const int Dot640[] = {
			21,  114, 128, 83,  21,
			150, 255, 255, 255, 104,
			239, 255, 255, 255, 192,
			226, 255, 255, 255, 165,
			114, 255, 255, 255, 65,
			29,  43,  89,  29,  29
		};

		if (si.iWidth < 640)
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					ClientDLL::GetInstance().pEngfuncs->pfnFillRGBA(x + j, y + i, 1, 1, r, g, b, Dot320[i*3 + j]);
		else
			for (int i = 0; i < 6; ++i)
				for (int j = 0; j < 5; ++j)
					ClientDLL::GetInstance().pEngfuncs->pfnFillRGBA(x + j, y + i, 1, 1, r, g, b, Dot640[i*5 + j]);
	}

	static void DrawDecimalSeparator(int x, int y, int r, int g, int b)
	{
		x += (NumberWidth - 6) / 2;
		y += NumberHeight - 5;
		DrawDot(x + 1, y, r, g, b);
	}

	static void DrawDecimalSeparator(int x, int y)
	{
		return DrawDecimalSeparator(x, y, hudColor[0], hudColor[1], hudColor[2]);
	}

	static void DrawColon(int x, int y, int r, int g, int b)
	{
		x += (NumberWidth - 6) / 2;
		DrawDot(x + 1, y + 2, r, g, b);
		y += NumberHeight - 5;
		DrawDot(x + 1, y - 2, r, g, b);
	}

	static void DrawColon(int x, int y)
	{
		return DrawColon(x, y, hudColor[0], hudColor[1], hudColor[2]);
	}

	static void GetPosition(const CVarWrapper& Offset, const CVarWrapper& Anchor, int* x, int* y, int rx = 0, int ry = 0)
	{
		std::istringstream iss;

		if (!Offset.IsEmpty())
		{
			iss.str(Offset.GetString());
			iss >> rx >> ry;
			iss.str(std::string());
			iss.clear();
		}

		iss.str(Anchor.GetString());
		float w = 0, h = 0;
		iss >> w >> h;

		rx += static_cast<int>(w * si.iWidth);
		ry += static_cast<int>(h * si.iHeight);

		if (x) *x = rx;
		if (y) *y = ry;
	}

	static void UpdatePrecision()
	{
		if (!CVars::bxt_hud_precision.IsEmpty())
		{
			precision = CVars::bxt_hud_precision.GetInt();
			if (precision > 16)
				precision = 16;
		}
		else
			precision = 6;
	}

	void GetAccurateInfo()
	{
		receivedAccurateInfo = HwDLL::GetInstance().TryGettingAccurateInfo(player.origin, player.velocity, player.stamina);
		HwDLL::GetInstance().GetViewangles(player.viewangles);
	}

	static void UpdateColors()
	{
		// Default: taken from con_color of HL 6153.
		consoleColor[0] = 1.0f;
		consoleColor[1] = 180 / 255.0f;
		consoleColor[2] = 30 / 255.0f;

		if (!CVars::con_color.IsEmpty())
		{
			unsigned r = 0, g = 0, b = 0;
			std::istringstream ss(CVars::con_color.GetString());
			ss >> r >> g >> b;

			consoleColor[0] = r / 255.0f;
			consoleColor[1] = g / 255.0f;
			consoleColor[2] = b / 255.0f;
		}

		// Default: yellowish.
		hudColor[0] = 255;
		hudColor[1] = 160;
		hudColor[2] = 0;

		if (!CVars::bxt_hud_color.IsEmpty())
		{
			auto colorStr = CVars::bxt_hud_color.GetString();
			if (colorStr != "auto")
			{
				std::istringstream color_ss(colorStr);
				color_ss >> hudColor[0] >> hudColor[1] >> hudColor[2];
			}
		}
	}

	static void DrawVelocity(float flTime)
	{
		if (CVars::bxt_hud_velocity.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_velocity_offset, CVars::bxt_hud_velocity_anchor, &x, &y, -200, 0);
			
			if (receivedAccurateInfo)
				DrawString(x, y, "Velocity:");
			else
				DrawString(x, y, "Velocity:", 1.0f, 0.0f, 0.0f);

			y += si.iCharHeight;
			
			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "X: " << player.velocity[0] << "\n"
				<< "Y: " << player.velocity[1] << "\n"
				<< "Z: " << player.velocity[2] << "\n"
				<< "XY: " << length(player.velocity[0], player.velocity[1]) << "\n"
				<< "XYZ: " << length(player.velocity[0], player.velocity[1], player.velocity[2]);

			DrawMultilineString(x, y, out.str());
		}
	}

	static void DrawOrigin(float flTime)
	{
		if (CVars::bxt_hud_origin.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_origin_offset, CVars::bxt_hud_origin_anchor, &x, &y, -200, (si.iCharHeight * 6) + 1);

			if (receivedAccurateInfo)
				DrawString(x, y, "Origin:");
			else
				DrawString(x, y, "Origin:", 1.0f, 0.0f, 0.0f);

			y += si.iCharHeight;

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "X: " << player.origin[0] << "\n"
				<< "Y: " << player.origin[1] << "\n"
				<< "Z: " << player.origin[2];

			DrawMultilineString(x, y, out.str());
		}
	}

	void DrawViewangles(float flTime)
	{
		if (CVars::bxt_hud_viewangles.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_viewangles_offset, CVars::bxt_hud_viewangles_anchor, &x, &y, -200, (si.iCharHeight * 10) + 2);

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "Pitch: " << player.viewangles[0] << "\n"
				<< "Yaw: " << player.viewangles[1];

			DrawMultilineString(x, y, out.str());
		}
	}

	static void DrawSpeedometer(float flTime)
	{
		if (CVars::bxt_hud_speedometer.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_speedometer_offset, CVars::bxt_hud_speedometer_anchor, &x, &y, 0, -2 * NumberHeight);
			DrawNumber(static_cast<int>(trunc(length(player.velocity[0], player.velocity[1]))), x, y);
		}
	}

	static void DrawJumpspeed(float flTime)
	{
		static float prevVel[3] = { 0.0f, 0.0f, 0.0f };

		if (CVars::bxt_hud_jumpspeed.GetBool())
		{
			static float lastTime = flTime;
			static double passedTime = FADE_DURATION_JUMPSPEED;
			static int fadingFrom[3] = { hudColor[0], hudColor[1], hudColor[2] };
			static double jumpSpeed = 0.0;

			int r = hudColor[0],
				g = hudColor[1],
				b = hudColor[2];

			if (FADE_DURATION_JUMPSPEED > 0.0f)
			{
				if ((player.velocity[2] != 0.0f && prevVel[2] == 0.0f)
					|| (player.velocity[2] > 0.0f && prevVel[2] < 0.0f))
				{
					double difference = length(player.velocity[0], player.velocity[1]) - jumpSpeed;
					if (difference != 0.0f)
					{
						if (difference > 0.0f)
						{
							fadingFrom[0] = 0;
							fadingFrom[1] = 255;
							fadingFrom[2] = 0;
						}
						else
						{
							fadingFrom[0] = 255;
							fadingFrom[1] = 0;
							fadingFrom[2] = 0;
						}

						passedTime = 0.0;
						jumpSpeed = length(player.velocity[0], player.velocity[1]);
					}
				}

				// Can be negative if we went back in time (for example, loaded a save).
				double timeDelta = std::max(flTime - lastTime, 0.0f);
				passedTime += timeDelta;

				// Check for Inf, NaN, etc.
				if (passedTime > FADE_DURATION_JUMPSPEED || !std::isnormal(passedTime)) {
					passedTime = FADE_DURATION_JUMPSPEED;
				}

				float colorVel[3] = { hudColor[0] - fadingFrom[0] / FADE_DURATION_JUMPSPEED,
				                      hudColor[1] - fadingFrom[1] / FADE_DURATION_JUMPSPEED,
				                      hudColor[2] - fadingFrom[2] / FADE_DURATION_JUMPSPEED };
				r = static_cast<int>(hudColor[0] - colorVel[0] * (FADE_DURATION_JUMPSPEED - passedTime));
				g = static_cast<int>(hudColor[1] - colorVel[1] * (FADE_DURATION_JUMPSPEED - passedTime));
				b = static_cast<int>(hudColor[2] - colorVel[2] * (FADE_DURATION_JUMPSPEED - passedTime));

				lastTime = flTime;
			}

			int x, y;
			GetPosition(CVars::bxt_hud_jumpspeed_offset, CVars::bxt_hud_jumpspeed_anchor, &x, &y, 0, -3 * NumberHeight);
			DrawNumber(static_cast<int>(trunc(jumpSpeed)), x, y, r, g, b);
		}

		vecCopy(player.velocity, prevVel);
	}

	void DrawTimer(float flTime)
	{
		if (CVars::bxt_hud_timer.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_timer_offset, CVars::bxt_hud_timer_anchor, &x, &y, 0, 0);

			if (hours)
			{
				x = DrawNumber(hours, x, y);
				DrawColon(x, y);
				x += NumberWidth;
			}

			if (hours || minutes)
			{
				int fieldMinWidth = (hours && minutes < 10) ? 2 : 1;
				x = DrawNumber(minutes, x, y, fieldMinWidth);
				DrawColon(x, y);
				x += NumberWidth;
			}

			int fieldMinWidth = ((hours || minutes) && seconds < 10) ? 2 : 1;
			x = DrawNumber(seconds, x, y, fieldMinWidth);

			DrawDecimalSeparator(x, y);
			x += NumberWidth;

			DrawNumber(static_cast<int>(timeRemainder * 1000), x, y, 3);
		}
	}

	void DrawDistance(float flTime)
	{
		if (CVars::bxt_hud_distance.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_distance_offset, CVars::bxt_hud_distance_anchor, &x, &y, -200, (si.iCharHeight * 12) + 3);

			float view[3] = { 0, 0, 28 };
			ClientDLL::GetInstance().pEngfuncs->pEventAPI->EV_LocalPlayerViewheight(view);
			view[0] += player.origin[0];
			view[1] += player.origin[1];
			view[2] += player.origin[2];

			float forward[3], right[3], up[3];
			ClientDLL::GetInstance().pEngfuncs->pfnAngleVectors(player.viewangles, forward, right, up);

			float end[3];
			vecCopy(view, end);
			end[0] += forward[0] * 8192;
			end[1] += forward[1] * 8192;
			end[2] += forward[2] * 8192;

			TraceResult tr;
			ServerDLL::GetInstance().pEngfuncs->pfnTraceLine(view, end, 0, HwDLL::GetInstance().GetPlayerEdict(), &tr);
			double hdist = std::hypot(tr.vecEndPos[0] - view[0], tr.vecEndPos[1] - view[1]);
			double vdist = tr.vecEndPos[2] - view[2];
			double hvdist = std::sqrt((tr.vecEndPos[0] - view[0]) * (tr.vecEndPos[0] - view[0])
				+ (tr.vecEndPos[1] - view[1]) * (tr.vecEndPos[1] - view[1])
				+ (tr.vecEndPos[2] - view[2]) * (tr.vecEndPos[2] - view[2]));

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "Distance:\n"
				<< "H: " << hdist << "\n"
				<< "V: " << vdist << "\n"
				<< "HV: " << hvdist;
			DrawMultilineString(x, y, out.str().c_str());
		}
	}

	void DrawStamina(float flTime)
	{
		if (CVars::bxt_hud_stamina.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_stamina_offset, CVars::bxt_hud_stamina_anchor, &x, &y, -200, (si.iCharHeight * 16) + 4);

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "Stamina: " << player.stamina;
			DrawString(x, y, out.str().c_str());
		}
	}

	void Init()
	{
		ResetTime();
		SpriteList = nullptr;
		initialized = true;
	}

	void InitIfNecessary()
	{
		if (!initialized)
			Init();
	}

	void VidInit()
	{
		UpdateScreenInfo();

		int SpriteRes = (si.iWidth < 640) ? 320 : 640;

		// Based on a similar procedure from hud.cpp.
		if (!SpriteList)
		{
			SpriteList = ClientDLL::GetInstance().pEngfuncs->pfnSPR_GetList(const_cast<char*>("sprites/hud.txt"), &SpriteCount);
			if (SpriteList)
			{
				for (client_sprite_t *p = SpriteList; p < (SpriteList + SpriteCount); ++p)
				{
					// If we have a sprite of the correct resolution which is named "number_x" where x is a digit
					if (p->iRes == SpriteRes
						&& strstr(p->szName, "number_") == p->szName
						&& *(p->szName + 8) == 0
						&& isdigit(*(p->szName + 7)))
					{
						int digit = *(p->szName + 7) - '0';
						NumberSpritePointers[digit] = p;
						NumberSpriteRects[digit] = p->rc;

						std::string path("sprites/");
						path += p->szSprite;
						path += ".spr";
						NumberSprites[digit] = ClientDLL::GetInstance().pEngfuncs->pfnSPR_Load(path.c_str());
						
						if (!digit)
						{
							NumberWidth = p->rc.right - p->rc.left;
							NumberHeight = p->rc.bottom - p->rc.top;
						}

						if (Hooks::DebugEnabled())
							EngineDevMsg("[client dll] Loaded the digit %d sprite from \"%s\".\n", digit, path.c_str());
					}
				}
			}
		}
		else
		{
			size_t i = 0;
			for (auto it = NumberSpritePointers.cbegin(); it != NumberSpritePointers.cend(); ++i, ++it)
			{
				std::string path("sprites/");
				path += (*it)->szSprite;
				path += ".spr";
				NumberSprites[i] = ClientDLL::GetInstance().pEngfuncs->pfnSPR_Load(path.c_str());

				if (Hooks::DebugEnabled())
					EngineDevMsg("[client dll] Reloaded the digit %d sprite from \"%s\".\n", i, path.c_str());
			}
		}
	}

	void Draw(float flTime)
	{
		if (!CVars::bxt_hud.GetBool())
			return;

		UpdatePrecision();
		UpdateColors();
		GetAccurateInfo();

		DrawVelocity(flTime);
		DrawOrigin(flTime);
		DrawViewangles(flTime);
		DrawSpeedometer(flTime);
		DrawJumpspeed(flTime);
		DrawTimer(flTime);
		DrawDistance(flTime);
		DrawStamina(flTime);

		receivedAccurateInfo = false;
	}

	void UpdatePlayerInfo(float vel[3], float org[3])
	{
		vecCopy(vel, player.velocity);
		vecCopy(org, player.origin);

		receivedAccurateInfo = true;

		HwDLL::GetInstance().SetPlayerOrigin(org);
		HwDLL::GetInstance().SetPlayerVelocity(vel);
	}

	void UpdatePlayerInfoInaccurate(float vel[3], float org[3])
	{
		if (!receivedAccurateInfo)
		{
			vecCopy(vel, player.velocity);
			vecCopy(org, player.origin);

			HwDLL::GetInstance().SetPlayerOrigin(org);
			HwDLL::GetInstance().SetPlayerVelocity(vel);
		}
	}

	void TimePassed(double time)
	{
		if (!countingTime)
			return;

		frames++;
		timeRemainder += time;
		seconds += static_cast<int>(timeRemainder);
		timeRemainder -= static_cast<int>(timeRemainder);
		if (seconds >= 60) {
			minutes += (seconds / 60);
			seconds %= 60;
		}
		if (minutes >= 60) {
			hours += (minutes / 60);
			minutes %= 60;
		}

		SendTimeUpdate();
	}

	void ResetTime()
	{
		countingTime = false;
		hours = minutes = seconds = 0;
		timeRemainder = 0.0;
		frames = 0;
		SendTimeUpdate();
	}

	void SetCountingTime(bool counting)
	{
		countingTime = counting;
	}

	void SendTimeUpdate() {
		if (!CVars::bxt_interprocess_enable.GetBool() || !Interprocess::mq)
			return;

		try {
			unsigned char buf[18];
			buf[0] = 18;
			buf[1] = 0x00;
			int milliseconds = static_cast<int>(timeRemainder * 1000);
			std::memcpy(buf + 2, &hours, sizeof(hours));
			std::memcpy(buf + 6, &minutes, sizeof(minutes));
			std::memcpy(buf + 10, &seconds, sizeof(seconds));
			std::memcpy(buf + 14, &milliseconds, sizeof(milliseconds));

			Interprocess::mq->send(buf, sizeof(buf), 0);

			if (HwDLL::GetInstance().frametime_remainder) {
				unsigned char buf2[10];
				buf2[0] = 10;
				buf2[1] = 0x03;
				std::memcpy(buf2 + 2, HwDLL::GetInstance().frametime_remainder, 8);

				Interprocess::mq->send(buf2, sizeof(buf2), 0);
			}
		} catch (boost::interprocess::interprocess_exception) {
			// Do nothing.
		}
	}
}
