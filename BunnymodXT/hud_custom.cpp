#include "stdafx.hpp"

#include "cvars.hpp"
#include "modules.hpp"
#include <SPTLib/Hooks.hpp>
#include "hud_custom.hpp"
#include "interprocess.hpp"
#include "runtime_data.hpp"

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
	bool countingTime = false;
	int hours = 0, minutes = 0, seconds = 0;
	double timeRemainder = 0.0;
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

	static void DrawMultilineString(int x, int y, std::string s, float r, float g, float b)
	{
		while (s.size() > 0)
		{
			auto pos = s.find('\n');

			DrawString(x, y, const_cast<char*>(s.substr(0, pos).c_str()), r, g, b);
			y += si.iCharHeight;

			if (pos != std::string::npos)
				s = s.substr(pos + 1, std::string::npos);
			else
				s.erase();
		};
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
		receivedAccurateInfo = HwDLL::GetInstance().TryGettingAccurateInfo(player.origin, player.velocity);
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

	static void SetupTraceVectors(float start[3], float end[3])
	{
		ClientDLL::GetInstance().pEngfuncs->pEventAPI->EV_LocalPlayerViewheight(start);
		start[0] += player.origin[0];
		start[1] += player.origin[1];
		start[2] += player.origin[2];

		float forward[3], right[3], up[3];
		ClientDLL::GetInstance().pEngfuncs->pfnAngleVectors(player.viewangles, forward, right, up);

		vecCopy(start, end);
		end[0] += forward[0] * 8192;
		end[1] += forward[1] * 8192;
		end[2] += forward[2] * 8192;
	}

	void DrawDistance(float flTime)
	{
		if (CVars::bxt_hud_distance.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_distance_offset, CVars::bxt_hud_distance_anchor, &x, &y, -200, (si.iCharHeight * 12) + 3);

			float view[3], end[3];
			SetupTraceVectors(view, end);

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
			DrawMultilineString(x, y, out.str());
		}
	}

	void DrawEntityHP(float flTime)
	{
		if (CVars::bxt_hud_entity_hp.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_entity_hp_offset, CVars::bxt_hud_entity_hp_anchor, &x, &y, -200, (si.iCharHeight * 16) + 3);

			float view[3], end[3];
			SetupTraceVectors(view, end);

			TraceResult tr;
			ServerDLL::GetInstance().pEngfuncs->pfnTraceLine(view, end, 0, HwDLL::GetInstance().GetPlayerEdict(), &tr);

			std::ostringstream out;
			if (tr.pHit)
			{
				out.setf(std::ios::fixed);
				out.precision(precision);
				out << "Entity HP: " << tr.pHit->v.health;
			}
			else
			{
				out << "Entity HP: N/A";
			}
			DrawString(x, y, out.str().c_str());
		}
	}

	static void GetSelfgaussInfo(bool &selfgaussable, float &length, int &hitGroup)
	{
		selfgaussable = false;

		float start[3], end[3];
		SetupTraceVectors(start, end);

		TraceResult tr;
		ServerDLL::GetInstance().pEngfuncs->pfnTraceLine(start, end, 0, HwDLL::GetInstance().GetPlayerEdict(), &tr);

		if (!tr.pHit || !tr.pHit->pvPrivateData || tr.pHit->v.solid != SOLID_BSP || tr.pHit->v.takedamage)
			return;

		Vector forward, right, up;
		ClientDLL::GetInstance().pEngfuncs->pfnAngleVectors(player.viewangles, forward, right, up);
		float n = -DotProduct(forward, tr.vecPlaneNormal);

		if (n < 0.5)
			return;

		TraceResult beamTr;
		ServerDLL::GetInstance().pEngfuncs->pfnTraceLine(tr.vecEndPos + 8 * forward, end, 0, nullptr, &beamTr);

		if (beamTr.fAllSolid)
			return;

		selfgaussable = true;

		ServerDLL::GetInstance().pEngfuncs->pfnTraceLine(beamTr.vecEndPos, tr.vecEndPos, 0, nullptr, &beamTr);
		length = (beamTr.vecEndPos - tr.vecEndPos).Length();

		ServerDLL::GetInstance().pEngfuncs->pfnTraceLine(start, end, 0, nullptr, &tr);
		hitGroup = tr.iHitgroup;
	}

	void DrawSelfgaussInfo(float flTime)
	{
		static const char *HITGROUP_STRING[] = {
			"Generic",
			"Head",
			"Chest",
			"Stomach",
			"Left Arm",
			"Right Arm",
			"Left Leg",
			"Right Leg"
		};

		if (CVars::bxt_hud_selfgauss.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_selfgauss_offset, CVars::bxt_hud_selfgauss_anchor, &x, &y, -200, (si.iCharHeight * 17) + 3);

			bool selfgaussable;
			int hitGroup;
			float threshold;
			GetSelfgaussInfo(selfgaussable, threshold, hitGroup);

			std::ostringstream out;
			out << "Selfgauss:\n";
			if (selfgaussable)
			{
				out.setf(std::ios::fixed);
				out.precision(precision);
				out << "Threshold: " << threshold << '\n'
					<< "Hit Group: " << HITGROUP_STRING[hitGroup];
			}
			else
			{
				out << "Cannot selfgauss";
			}
			DrawMultilineString(x, y, out.str());
		}
	}

	void DrawVisibleLandmarks(float flTime)
	{
		globalvars_t **ppGlobals = ServerDLL::GetInstance().ppGlobals;
		if (CVars::bxt_hud_visible_landmarks.GetBool() && ppGlobals)
		{
			int x, y;
			GetPosition(CVars::bxt_hud_visible_landmarks_offset, CVars::bxt_hud_visible_landmarks_anchor, &x, &y, -20, 0);

			std::ostringstream out;
			out << "Visible Landmarks:\n";

			const enginefuncs_t *efun = ServerDLL::GetInstance().pEngfuncs;
			edict_t *pent = nullptr;
			for (;;)
			{
				pent = efun->pfnFindEntityByString(pent, "classname", "info_landmark");
				if (!pent || !efun->pfnEntOffsetOfPEntity(pent))
					break;

				const edict_t *pentPlayer = efun->pfnFindClientInPVS(pent);
				if (!pentPlayer || !efun->pfnEntOffsetOfPEntity(pentPlayer))
					continue;

				out << (*ppGlobals)->pStringBase + pent->v.targetname << '\n';
			}

			DrawMultilineString(x, y, out.str());
		}
	}

	void DrawIncorrectFPSIndicator(float flTime)
	{
		static float lastTime = flTime;

		// We'll use this for checking the 99.5 FPS with fps_max 99.5 condition,
		// because even on fps_max + 0.5 engines the FPS can oscillate down to 99.5.
		static unsigned badFrames = 0;

		if (CVars::bxt_hud_incorrect_fps_indicator.GetBool())
		{
			const auto timeDiff = flTime - lastTime;

			if (timeDiff > 0) {
				const auto fps = 1.0f / timeDiff;

				if (fps > 100.1f && fps < 100.6f) {
					const auto fps_max = CVars::fps_max.GetFloat();

					if (fps_max > 99.6f && fps_max < 100.6f) {
						const char message[] = "Your FPS seems to be incorrect.\n"
						                       "Most likely you need to set fps_max to 99.5.\n"
						                       "If you know what you're doing and you're sure your FPS is correct,\n"
						                       "you can disable this message with bxt_hud_incorrect_fps_indicator 0.";

						DrawMultilineString(2, 4 + si.iCharHeight, message, 1.0f, 1.0f, 1.0f);
					}
				} else if (fps < 99.6f && fps > 99.0f) {
					const auto fps_max = CVars::fps_max.GetFloat();

					if (fps_max < 99.7f && fps_max > 99.0f) {
						if (badFrames >= 10) {
							const char message[] = "Your FPS seems to be incorrect.\n"
									       "Most likely you need to set fps_max to 100.\n"
									       "If you know what you're doing and you're sure your FPS is correct,\n"
									       "you can disable this message with bxt_hud_incorrect_fps_indicator 0.";

							DrawMultilineString(2, 4 + si.iCharHeight, message, 1.0f, 1.0f, 1.0f);
						} else {
							++badFrames;
						}
					} else {
						badFrames = 0;
					}
				}

				if (!(fps < 99.6f && fps > 99.0f))
					badFrames = 0;
			}
		}

		lastTime = flTime;
	}

	void Init()
	{
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
		DrawEntityHP(flTime);
		DrawSelfgaussInfo(flTime);
		DrawVisibleLandmarks(flTime);
		DrawIncorrectFPSIndicator(flTime);

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

		const auto previous_seconds = seconds;

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

		// Only save the time occasionally (to prevent large demo file size).
		if (previous_seconds != seconds)
			SaveTimeToDemo();
	}

	void ResetTime()
	{
		Interprocess::WriteTimerReset(GetTime());
		countingTime = false;
		hours = minutes = seconds = 0;
		timeRemainder = 0.0;
		frames = 0;
		SendTimeUpdate();
	}

	void SetCountingTime(bool counting)
	{
		if (counting && !countingTime)
			Interprocess::WriteTimerStart(GetTime());

		countingTime = counting;
	}

	void SendTimeUpdate() {
		Interprocess::WriteTime(GetTime());

		if (HwDLL::GetInstance().frametime_remainder)
			Interprocess::WriteFrametimeRemainder(*HwDLL::GetInstance().frametime_remainder);
	}

	void SaveTimeToDemo() {
		RuntimeData::Add(RuntimeData::Time{
			static_cast<uint32_t>(hours),
			static_cast<uint8_t>(minutes),
			static_cast<uint8_t>(seconds),
			timeRemainder
		});
	}

	Interprocess::Time GetTime()
	{
		uint16_t milliseconds = static_cast<uint16_t>(timeRemainder * 1000);
		return Interprocess::Time{
			static_cast<uint32_t>(hours),
			static_cast<uint8_t>(minutes),
			static_cast<uint8_t>(seconds),
			milliseconds };
	}

	const SCREENINFO& GetScreenInfo()
	{
		return si;
	}
}
