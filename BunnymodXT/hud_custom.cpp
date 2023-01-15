#include "stdafx.hpp"

#include "cvars.hpp"
#include "modules.hpp"
#include <SPTLib/Hooks.hpp>
#include "hud_custom.hpp"
#include "interprocess.hpp"
#include "runtime_data.hpp"
#include "opengl_utils.hpp"

#include <GL/gl.h>

#include "modules/HwDLL.hpp"

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
	bool invalidRun = false;
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

	struct FrameBulkStatus {
		bool strafe;
		HLTAS::StrafeType strafe_type;
		HLTAS::StrafeDir strafe_dir;

		bool lgagst;
		bool autojump;
		bool ducktap;
		bool jumpbug;
		bool dbc;
		bool dbc_ceilings;
		bool dbg;
		bool dwj;

		bool forward;
		bool left;
		bool right;
		bool back;
		bool up;
		bool down;

		bool jump;
		bool duck;
		bool use;
		bool attack1;
		bool attack2;
		bool reload;

		double pitch;
		double yaw;
		unsigned count;
		bool pitch_present;
		bool yaw_present;
		bool count_present;

		std::string commands;

		float vel;
		float zvel;
		float pos[3];
		float realyaw;
		float health;
		float armor;

		float stamina;

		std::string frametime;
	};
	static FrameBulkStatus frame_bulk_status;
	static bool frame_bulk_selected;

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

	static int DrawString(int x, int y, const char* s, float r, float g, float b)
	{
		ClientDLL::GetInstance().pEngfuncs->pfnDrawSetTextColor(r, g, b);
		return ClientDLL::GetInstance().pEngfuncs->pfnDrawConsoleString(x, y, const_cast<char*>(s));
	}

	static inline int DrawString(int x, int y, const char* s)
	{
		return DrawString(x, y, s, consoleColor[0], consoleColor[1], consoleColor[2]);
	}

	static int DrawMultilineString(int x, int y, std::string s, float r, float g, float b)
	{
		int max_new_x = 0;

		while (s.size() > 0)
		{
			auto pos = s.find('\n');

			int new_x = DrawString(x, y, const_cast<char*>(s.substr(0, pos).c_str()), r, g, b);
			max_new_x = std::max(new_x, max_new_x);
			y += si.iCharHeight;

			if (pos != std::string::npos)
				s = s.substr(pos + 1, std::string::npos);
			else
				s.erase();
		};

		return max_new_x;
	}

	static int DrawMultilineString(int x, int y, std::string s)
	{
		int max_new_x = 0;

		while (s.size() > 0)
		{
			auto pos = s.find('\n');

			int new_x = DrawString(x, y, const_cast<char*>(s.substr(0, pos).c_str()));
			max_new_x = std::max(new_x, max_new_x);
			y += si.iCharHeight;

			if (pos != std::string::npos)
				s = s.substr(pos + 1, std::string::npos);
			else
				s.erase();
		};

		return max_new_x;
	}

	static void DrawDigit(int digit, int x, int y, int r, int g, int b)
	{
		assert(digit >= 0 && digit <= 9);

		ClientDLL::GetInstance().bxt_hud_color_set = true;
		ClientDLL::GetInstance().pEngfuncs->pfnSPR_Set(NumberSprites[digit], r, g, b);
		ClientDLL::GetInstance().bxt_hud_color_set = false;

		ClientDLL::GetInstance().pEngfuncs->pfnSPR_DrawAdditive(0, x, y, &NumberSpriteRects[digit]);
	}

	static inline int DrawBitmap(int x, int y, const int bitmap[], int width, int height, int r, int g, int b) {
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				ClientDLL::GetInstance().bxt_hud_color_fill = true;
				ClientDLL::GetInstance().pEngfuncs->pfnFillRGBA(x + j, y + i, 1, 1, r, g, b, bitmap[i * width + j]);
				ClientDLL::GetInstance().bxt_hud_color_fill = false;
			}

		return width;
	}

	static void DrawDot(int x, int y, int r, int g, int b)
	{
		const int Dot320[] = {
			143, 199, 122,
			255, 255, 218,
			120, 169, 95
		};

		const int Dot640[] = {
			21,  114, 128, 128, 83,  21,
			150, 255, 255, 255, 255, 104,
			239, 255, 255, 255, 255, 192,
			226, 255, 255, 255, 255, 165,
			114, 255, 255, 255, 255, 65,
			29,  43,  89,  89,  29,  29
		};

		if (si.iWidth < 640)
			DrawBitmap(x, y, Dot320, 3, 3, r, g, b);
		else
			DrawBitmap(x, y, Dot640, 6, 6, r, g, b);
	}

	static void DrawLine(int x, int y, int length, int r, int g, int b) {
		if ((si.iWidth < 640 && length < 3) || length < 5) {
			// might as well draw a dot
			DrawDot(x, y, r, g, b);
			return;
		}


		const int Line640_left[] = {
			21,  114,
			150, 255,
			239, 255,
			226, 255,
			114, 255,
			29,  43
		};

		const int Line640_repeat[] = {
			128,
			255,
			255,
			255,
			255,
			89
		};

		const int Line640_Right[] = {
			 83,  21,
			255, 104,
			255, 192,
			255, 165,
			255,  65,
			 29,  29
		};

		const int Line320_left[] = {
			143,
			255,
			120,
		};

		const int Line320_repeat[] = {
			199,
			255,
			169,
		};

		const int Line320_Right[] = {
			122,
			218,
			95
		};

		const int *line_left;
		const int *line_repeat;
		const int *line_right;
		int linesprite_height;
		int linesprite_width;

		if (si.iWidth <= 640) {
			line_left = Line320_left;
			line_repeat = Line320_repeat;
			line_right = Line320_Right;
			linesprite_height = 3;
			linesprite_width = 1;
		} else {
			line_left = Line640_left;
			line_repeat = Line640_repeat;
			line_right = Line640_Right;
			linesprite_height = 6;
			linesprite_width = 2;
		}

		y -= linesprite_height / 2;

		x += DrawBitmap(x, y, line_left, linesprite_width, linesprite_height, r, g, b);

		for (int xOffset = 0; xOffset < length - linesprite_width * 2; xOffset++) {
			x += DrawBitmap(x, y, line_repeat, 1, linesprite_height, r, g, b);
		}

		DrawBitmap(x, y, line_right, linesprite_width, linesprite_height, r, g, b);
	}

	static int DrawNumber(int number, int x, int y, int r, int g, int b, int fieldMinWidth = 1)
	{
		if (number < 0)
		{
			if (number == std::numeric_limits<int>::min())
			{
				number = 0;
			}
			else
			{
				number = abs(number);
				DrawLine(x - NumberWidth, y + NumberHeight / 2 , NumberWidth, r, g, b);
			}
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

	static inline int DrawNumberTimer(int number, int x, int y, int fieldMinWidth = 1)
	{
		if (invalidRun)
			return DrawNumber(number, x, y, 240, 0, 0, fieldMinWidth);
		else
			return DrawNumber(number, x, y, hudColor[0], hudColor[1], hudColor[2], fieldMinWidth);
	}

	static void DrawDecimalSeparator(int x, int y, int r, int g, int b)
	{
		x += (NumberWidth - 6) / 2;
		y += NumberHeight - 5;
		DrawDot(x + 1, y, r, g, b);
	}

	static void DrawDecimalSeparator(int x, int y)
	{
		if (invalidRun)
			return DrawDecimalSeparator(x, y, 240, 0, 0);
		else
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
		if (invalidRun)
			return DrawColon(x, y, 240, 0, 0);
		else
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
		receivedAccurateInfo = HwDLL::GetInstance().TryGettingAccurateInfo(player.origin, player.velocity, player.health, player.armorvalue, player.waterlevel, player.stamina);
		ClientDLL::GetInstance().pEngfuncs->GetViewAngles(player.viewangles);
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

	static void DrawQuickGauss(float flTime)
	{
		if (CVars::bxt_hud_quickgauss.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_quickgauss_offset, CVars::bxt_hud_quickgauss_anchor, &x, &y, 150, 0);

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);

			auto time = ServerDLL::GetInstance().GetTime();
			auto v_forward = ClientDLL::GetInstance().AnglesToForward(player.viewangles);
			float flDamage = 200;
			if (time < 4.0f)
				flDamage = 200 * time / 4;

			auto vel_gain = v_forward * flDamage * 5;
			auto ups = static_cast<int>(trunc(length(vel_gain[0], vel_gain[1])));

			out << "Server time: " << time << "\n"
				<< "Qgauss dmg: " << flDamage << "\n"
				<< "Qgauss boost: " << ups;

			DrawMultilineString(x, y, out.str());
		}
	}

	static void DrawOrigin(float flTime)
	{
		if (CVars::bxt_hud_origin.GetBool())
		{
			const auto& cl = ClientDLL::GetInstance();

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

			if (CVars::bxt_hud_origin.GetInt() == 2)
			{
				out << "X: " << cl.last_vieworg[0] << "\n"
					<< "Y: " << cl.last_vieworg[1] << "\n"
					<< "Z: " << cl.last_vieworg[2];
			}
			else
			{
				out << "X: " << player.origin[0] << "\n"
					<< "Y: " << player.origin[1] << "\n"
					<< "Z: " << player.origin[2];
			}

			DrawMultilineString(x, y, out.str());
		}
	}

	void DrawViewangles(float flTime)
	{
		if (CVars::bxt_hud_viewangles.GetBool())
		{
			const auto& cl = ClientDLL::GetInstance();

			int x, y;
			GetPosition(CVars::bxt_hud_viewangles_offset, CVars::bxt_hud_viewangles_anchor, &x, &y, -200, (si.iCharHeight * 10) + 2);

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);

			if (CVars::bxt_hud_viewangles.GetInt() == 2)
			{
				out << "Angles (incl. punch):" << "\n"
				<< "Pitch: " << cl.last_viewangles[0] << "\n"
				<< "Yaw: " << cl.last_viewangles[1];
			}
			else
			{
				out << "Angles:" << "\n"
				<< "Pitch: " << player.viewangles[0] << "\n"
				<< "Yaw: " << player.viewangles[1];
			}

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
					double speed = length(prevVel[0], prevVel[1]);
					bool is_ducktap = player.velocity[2] < 0.0f && prevVel[2] == 0.0f;

					if (CVars::bxt_bhopcap.GetBool() && !is_ducktap && player.waterlevel == 0)
					{
						HLStrafe::MovementVars vars = HwDLL::GetInstance().GetMovementVars();
						auto maxscaledspeed = vars.BhopcapMaxspeedScale * vars.Maxspeed;
						if (maxscaledspeed > 0) {
							auto xyz_speed = length(prevVel[0], prevVel[1], prevVel[2]);
							if (xyz_speed > maxscaledspeed)
								speed *= (maxscaledspeed / xyz_speed) * vars.BhopcapMultiplier;
						}
					}

					double difference = speed - jumpSpeed;
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
						jumpSpeed = speed;
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

	static void DrawJumpDistance(float flTime)
	{
		static float prevOrigin[3] = { 0.0f, 0.0f, 0.0f };
		static float prevPlayerOrigin[3] = { 0.0f, 0.0f, 0.0f };
		static float prevVel[3] = { 0.0f, 0.0f, 0.0f };

		if (CVars::bxt_hud_jumpdistance.GetBool())
		{
			static bool inJump = false;
			static double jumpDistance = 0.0;

			// 1 = jumpdistance will update when velocity is reasonably positive
			// not 1 = jumpdistance will update when velocity changes, eg just falling
			if (!inJump && (
				(((CVars::bxt_hud_jumpdistance.GetInt() == 1) ?
					player.velocity[2] > 0.0f : player.velocity[2] != 0.0f) && prevVel[2] == 0.0f) ||
				(player.velocity[2] > 0.0f && prevVel[2] < 0.0f)))
			{
				inJump = true;

				// by the time the instantaneous velocity is here
				// there is already displacement, which is in the previous frame
				vecCopy(prevPlayerOrigin, prevOrigin);
			}
			else if (inJump && ((player.velocity[2] == 0.0f && prevVel[2] < 0.0f)))
			{
				inJump = false;

				// add 16*2 because of player size can reach the edge of the block up to 16 unit
				double distance = length(player.origin[0] - prevOrigin[0], player.origin[1] - prevOrigin[1]) + 32.0;

				if (distance > 150.0 || CVars::bxt_hud_jumpdistance.GetInt() != 1) // from uq_jumpstats default
				{
					jumpDistance = distance;
				}
			}
			else if (inJump && player.origin[2] == prevPlayerOrigin[2])
			{
				// walking
				inJump = false;
			}

			int x, y;
			GetPosition(CVars::bxt_hud_jumpdistance_offset, CVars::bxt_hud_jumpdistance_anchor, &x, &y, 0, -4 * NumberHeight);
			DrawNumber(static_cast<int>(trunc(jumpDistance)), x, y, hudColor[0], hudColor[1], hudColor[2]);
		}

		vecCopy(player.origin, prevPlayerOrigin);
		vecCopy(player.velocity, prevVel);
	}


	void DrawTimer(float flTime)
	{
		if (CVars::bxt_hud_timer.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_timer_offset, CVars::bxt_hud_timer_anchor, &x, &y, 0, 0);

			if (invalidRun)
				DrawMultilineString(x, y + NumberHeight, "Invalid run", 1.0f, 0.0f, 0.0f);

			if (hours)
			{
				x = DrawNumberTimer(hours, x, y);
				DrawColon(x, y);
				x += NumberWidth;
			}

			if (hours || minutes)
			{
				int fieldMinWidth = (hours && minutes < 10) ? 2 : 1;
				x = DrawNumberTimer(minutes, x, y, fieldMinWidth);
				DrawColon(x, y);
				x += NumberWidth;
			}

			int fieldMinWidth = ((hours || minutes) && seconds < 10) ? 2 : 1;
			x = DrawNumberTimer(seconds, x, y, fieldMinWidth);

			DrawDecimalSeparator(x, y);
			x += NumberWidth;

			DrawNumberTimer(static_cast<int>(timeRemainder * 1000), x, y, 3);
		}
	}

	void DrawDistance(float flTime)
	{
		if (CVars::bxt_hud_distance.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_distance_offset, CVars::bxt_hud_distance_anchor, &x, &y, -200, (si.iCharHeight * 13) + 3);

			float view[3], end[3];
			ClientDLL::GetInstance().SetupTraceVectors(view, end);

			const auto tr = ServerDLL::GetInstance().TraceLine(view, end, 0, HwDLL::GetInstance().GetPlayerEdict());
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

	void DrawEntityInfo(float flTime)
	{
		if (CVars::bxt_hud_entity_info.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_entity_info_offset, CVars::bxt_hud_entity_info_anchor, &x, &y, -200, (si.iCharHeight * 17) + 3);

			const auto& hw = HwDLL::GetInstance();
			const auto& sv = ServerDLL::GetInstance();

			float view[3], end[3];
			ClientDLL::GetInstance().SetupTraceVectors(view, end);

			const auto tr = sv.TraceLine(view, end, 0, HwDLL::GetInstance().GetPlayerEdict());

			std::ostringstream out;
			if (tr.pHit)
			{
				out.setf(std::ios::fixed);
				out.precision(precision);

				const auto ent = tr.pHit;

				edict_t *edicts;
				hw.GetEdicts(&edicts);
				const auto index = ent - edicts;
				out << "Entity: " << index << '\n';

				const char *classname = hw.GetString(ent->v.classname);
				out << classname << '\n';

				if (ent->v.target != 0) {
					const char *target = hw.GetString(ent->v.target);
					out << "Target: " << target << '\n';
				}

				if (ent->v.targetname != 0) {
					const char *targetname = hw.GetString(ent->v.targetname);
					out << "Name: " << targetname << '\n';
				}

				out << "HP: " << ent->v.health << '\n';

				if (strstr(classname, "func_door") != NULL)
				{
					// https://github.com/ValveSoftware/halflife/blob/master/dlls/doors.h#L27-L28
					if (ent->v.spawnflags & 256)
						out << "Usable: Yes" << '\n';
					else
						out << "Usable: No" << '\n';

					if (ent->v.spawnflags & 512)
						out << "Monsters: Can't open" << '\n';
					else
						out << "Monsters: Can open" << '\n';
				}

				if ((strstr(classname, "func_door") != NULL) || (!strncmp(classname, "func_rotating", 13)) || (!strncmp(classname, "func_train", 10)))
					out << "Damage: " << ent->v.dmg << '\n';

				if (CVars::bxt_hud_entity_info.GetInt() == 2)
				{
					out << "Yaw: " << ent->v.angles[1] << '\n';

					// Borrowed from https://github.com/SNMetamorph/goldsrc-monitor/blob/08c368e246d09996b2d85e4367d4d8cc1e507712/sources/library/displaymode_entityreport.cpp#L45
					Vector origin;

					if (ent->v.solid == SOLID_BSP || ent->v.movetype == MOVETYPE_PUSHSTEP)
						origin = ent->v.origin + ((ent->v.mins + ent->v.maxs) / 2.f);
					else
						origin = ent->v.origin;

					out << "X: " << origin.x << '\n';
					out << "Y: " << origin.y << '\n';
					out << "Z: " << origin.z << '\n';

					out << "X Vel: " << ent->v.velocity.x << '\n';
					out << "Y Vel: " << ent->v.velocity.y << '\n';
					out << "Z Vel: " << ent->v.velocity.z;
				}
			}
			else
			{
				out << "Entity: N/A";
			}

			DrawMultilineString(x, y, out.str().c_str());
		}
	}

	static void GetSelfgaussInfo(bool &selfgaussable, float &length, int &hitGroup)
	{
		selfgaussable = false;

		float start[3], end[3];
		ClientDLL::GetInstance().SetupTraceVectors(start, end);

		auto tr = ServerDLL::GetInstance().TraceLine(start, end, 0, HwDLL::GetInstance().GetPlayerEdict());

		if (!tr.pHit || !tr.pHit->pvPrivateData || tr.pHit->v.solid != SOLID_BSP || tr.pHit->v.takedamage)
			return;

		Vector forward, right, up;
		ClientDLL::GetInstance().pEngfuncs->pfnAngleVectors(player.viewangles, forward, right, up);
		float n = -DotProduct(forward, tr.vecPlaneNormal);

		if (n < 0.5)
			return;

		auto beamTr = ServerDLL::GetInstance().TraceLine(tr.vecEndPos + 8 * forward, end, 0, nullptr);

		if (beamTr.fAllSolid)
			return;

		selfgaussable = true;

		beamTr = ServerDLL::GetInstance().TraceLine(beamTr.vecEndPos, tr.vecEndPos, 0, nullptr);
		length = (beamTr.vecEndPos - tr.vecEndPos).Length();

		tr = ServerDLL::GetInstance().TraceLine(start, end, 0, nullptr);
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
			GetPosition(CVars::bxt_hud_selfgauss_offset, CVars::bxt_hud_selfgauss_anchor, &x, &y, -200, (si.iCharHeight * 32) + 3);

			bool selfgaussable;
			int hitGroup = 0; // It's always initialized if selfgaussable is set to true, but GCC issues a warning anyway.
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
		if (CVars::bxt_hud_visible_landmarks.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_visible_landmarks_offset, CVars::bxt_hud_visible_landmarks_anchor, &x, &y, -75, 0);

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

				out << HwDLL::GetInstance().GetString(pent->v.targetname) << '\n';
			}

			DrawMultilineString(x, y, out.str());
		}
	}

	void DrawArmor(float flTime)
	{
		if (CVars::bxt_hud_armor.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_armor_offset, CVars::bxt_hud_armor_anchor, &x, &y, -200, (si.iCharHeight * 35) + 3);

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "Armor: " << player.armorvalue;

			DrawMultilineString(x, y, out.str());
		}
	}

	void DrawWaterlevel(float flTime)
	{
		if (CVars::bxt_hud_waterlevel.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_waterlevel_offset, CVars::bxt_hud_waterlevel_anchor, &x, &y, -200, (si.iCharHeight * 36) + 3);

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "Waterlevel: " << player.waterlevel;

			DrawMultilineString(x, y, out.str());
		}
	}

	void DrawNihilanthInfo(float flTime)
	{
		static const char *IRRITATIONS[4] = {"Idle", "Attacking", "Opened", "Killed"};

		if (CVars::bxt_hud_nihilanth.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_nihilanth_offset, CVars::bxt_hud_nihilanth_anchor, &x, &y, -200, (si.iCharHeight * 37) + 3);

			std::ostringstream out;
			out << "Nihilanth:\n";

			float health, frame;
			int level, irritation, nspheres, sequence;
			bool recharger;
			if (ServerDLL::GetInstance().GetNihilanthInfo(health, level, irritation, recharger, nspheres, sequence, frame)) {
				// This check is in case the offset for m_irritation is wrong
				// and so some weird values are obtained.
				const char *irritation_str = "INVALID";
				if (0 <= irritation && irritation <= 3) {
					irritation_str = IRRITATIONS[irritation];
				}
				out << "Health: " << health << '\n'
					<< "Level: " << level << "/10\n"
					<< "Irritation: " << irritation << " (" << irritation_str << ")\n"
					<< "Recharger: " << (recharger ? "Found" : "Not found") << '\n'
					<< "Spheres: " << nspheres << "/20\n"
					<< "Sequence: " << sequence << " (" << std::fixed << std::setprecision(1) << frame << ")\n";
			} else {
				out << "Not found";
			}

			DrawMultilineString(x, y, out.str());
		}
	}

	void DrawGonarchInfo(float flTime)
	{
		if (CVars::bxt_hud_gonarch.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_gonarch_offset, CVars::bxt_hud_gonarch_anchor, &x, &y, -200, (si.iCharHeight * 44) + 3);

			std::ostringstream out;
			out << "Gonarch:\n";

			float health, frame;
			int sequence;

			if (ServerDLL::GetInstance().GetGonarchInfo(health, sequence, frame)) {
				out << "Health: " << health << '\n'
					<< "Sequence: " << sequence << " (" << std::fixed << std::setprecision(1) << frame << ")\n";
			}
			else {
				out << "Not found";
			}

			DrawMultilineString(x, y, out.str());
		}
	}

	void DrawHealth(float flTime)
	{
		if (CVars::bxt_hud_health.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_health_offset, CVars::bxt_hud_health_anchor, &x, &y, 0, -4 * NumberHeight);

			if (ClientDLL::GetInstance().pEngfuncs->pDemoAPI->IsPlayingback() && CVars::bxt_hud_health_override_in_demo.GetInt() >= 1)
				DrawNumber(CVars::bxt_hud_health_override_in_demo.GetInt(), x, y);
			else
				DrawNumber(static_cast<int>(player.health), x, y);
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

	static void DrawCollisionDepthMap(float flTime)
	{
		if (CVars::bxt_collision_depth_map.GetBool())
		{
			constexpr float M_DEG2RAD = float(M_PI) / 180.f;

			const auto hull_type = static_cast<HLStrafe::HullType>(std::clamp(CVars::bxt_collision_depth_map_hull.GetInt(), 0, 2));

			// Arbitrary sanity clamps.
			const auto max_depth = std::max(CVars::bxt_collision_depth_map_max_depth.GetFloat(), 10.f);

			// Some constants.
			const float aspect_ratio = (float)si.iHeight / (float)si.iWidth;

			float fov_getfloat;
			if (CVars::bxt_force_fov.GetFloat() >= 1.0)
				fov_getfloat = CVars::bxt_force_fov.GetFloat();
			else
				fov_getfloat = CVars::default_fov.GetFloat();

			const auto fov = std::clamp(fov_getfloat, 30.f, 150.f) * M_DEG2RAD;
			const auto vfov = 2.f * std::atan(std::tan(fov * 0.5f) * aspect_ratio);

			const float screen_width = max_depth * (float) std::tan(fov * 0.5);
			const float screen_height = max_depth * (float) std::tan(vfov * 0.5);

			// The trace starting point and forward/right/up vectors.
			const auto& cl = ClientDLL::GetInstance();
			const auto start = cl.last_vieworg;
			Vector forward, right, up;
			cl.pEngfuncs->pfnAngleVectors(cl.last_viewangles, forward, right, up);

			const auto remove_limit = CVars::bxt_collision_depth_map_remove_distance_limit.GetBool();
			HwDLL::GetInstance().StartTracing(remove_limit);

			// Set up some OpenGL stuff.
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			const auto pixel_scale = CVars::bxt_collision_depth_map_pixel_scale.GetInt();

			// Main loop.
			for (int y = 0; y < si.iHeight; y += pixel_scale) {
				// -1 <= y_offset <= 1
				const float y_offset = -((float)y / (float)si.iHeight - 0.5f) * 2.f;

				for (int x = 0; x < si.iWidth; x += pixel_scale) {
					// -1 <= x_offset <= 1
					const float x_offset = ((float)x / (float)si.iWidth - 0.5f) * 2.f;

					const auto end = start + forward * max_depth
						+ screen_width * x_offset * right
						+ screen_height * y_offset * up;

					// Trace.
					const auto result = HwDLL::GetInstance().UnsafePlayerTrace(start, end, hull_type);

					if (CVars::bxt_collision_depth_map_colors.GetBool()) {
						const auto make_color = [](const unsigned char value[4]) {
							return value[0] ^ value[1] ^ value[2] ^ value[3];
						};
						glColor4ub(
							make_color(reinterpret_cast<const unsigned char*>(&result.PlaneNormal[0])),
							make_color(reinterpret_cast<const unsigned char*>(&result.PlaneNormal[1])),
							make_color(reinterpret_cast<const unsigned char*>(&result.PlaneNormal[2])),
							255);
					} else {
						const auto value = 255 - static_cast<int>(std::round(result.Fraction * 255));
						glColor4ub(value, value, value, 255);
					}

					glBegin(GL_QUADS);
					glVertex2i(x, y);
					glVertex2i(x, y + pixel_scale);
					glVertex2i(x + pixel_scale, y + pixel_scale);
					glVertex2i(x + pixel_scale, y);
					glEnd();
				}
			}

			glColor3f(1.0f, 1.0f, 1.0f);
			glDisable(GL_BLEND);
			glEnable(GL_TEXTURE_2D);

			HwDLL::GetInstance().StopTracing();
		}
	}

	static void DrawTASEditorStatus()
	{
		if (!CVars::bxt_hud_tas_editor_status.GetBool())
			return;

		int x, y;
		GetPosition(CVars::bxt_hud_tas_editor_status_offset, CVars::bxt_hud_tas_editor_status_anchor, &x, &y, 2, si.iCharHeight + 2);

		std::ostringstream out;
		out.setf(std::ios::fixed);
		out.precision(precision);
		out << "TAS Editor Status:\n";
		if (frame_bulk_selected) {
			out << "Strafing:\n  ";
			if (frame_bulk_status.strafe) {
				out << 's'
					<< static_cast<int>(frame_bulk_status.strafe_type)
					<< static_cast<int>(frame_bulk_status.strafe_dir);

				switch (frame_bulk_status.strafe_type) {
					case HLTAS::StrafeType::MAXACCEL:
						out << " (speed increasing)";
						break;
					case HLTAS::StrafeType::MAXANGLE:
						out << " (quick turn)";
						break;
					case HLTAS::StrafeType::MAXDECCEL:
						out << " (slow down)";
						break;
					case HLTAS::StrafeType::CONSTSPEED:
						out << " (constant speed)";
						break;
					default:
						assert(false);
						break;
				}
			} else {
				out << "disabled";
			}

			out << "\nEnabled Actions:\n";

			const std::pair<const char*, bool> actions[] = {
				{ "lgagst", frame_bulk_status.lgagst },
				{ "auto jump", frame_bulk_status.autojump },
				{ "duck tap", frame_bulk_status.ducktap },
				{ "jump bug", frame_bulk_status.jumpbug },
				{ "duck before collision", frame_bulk_status.dbc },
				{ "  (incl. ceilings)", frame_bulk_status.dbc_ceilings },
				{ "duck before ground", frame_bulk_status.dbg },
				{ "duck when jump", frame_bulk_status.dwj },
				{ "forward", frame_bulk_status.forward },
				{ "left", frame_bulk_status.left },
				{ "right", frame_bulk_status.right },
				{ "back", frame_bulk_status.back },
				{ "up", frame_bulk_status.up },
				{ "down", frame_bulk_status.down },
				{ "jump", frame_bulk_status.jump },
				{ "duck", frame_bulk_status.duck },
				{ "use", frame_bulk_status.use },
				{ "attack1", frame_bulk_status.attack1 },
				{ "attack2", frame_bulk_status.attack2 },
				{ "reload", frame_bulk_status.reload }
			};
			for (const auto& action : actions) {
				if (!action.second)
					continue;

				out << "  " << action.first << '\n';
			}

			out << "Frametime: " << frame_bulk_status.frametime << '\n';

			if (frame_bulk_status.pitch_present) {
				out << "Pitch: " << frame_bulk_status.pitch << "\n";
			}
			if (frame_bulk_status.yaw_present) {
				out << "Yaw: " << frame_bulk_status.yaw << "\n";
			}
			if (frame_bulk_status.count_present) {
				out << "Left-right frame count: " << frame_bulk_status.count << "\n";
			}
			if (!frame_bulk_status.commands.empty()) {
				out << "Commands:\n  " << frame_bulk_status.commands << '\n';
			}

			out << "Camera Yaw: " << frame_bulk_status.realyaw << '\n';

			out << "Vel: " << frame_bulk_status.vel << '\n';
			out << "Z Vel: " << frame_bulk_status.zvel << '\n';

			out << "X Pos: " << frame_bulk_status.pos[0] << '\n'
				<< "Y Pos: " << frame_bulk_status.pos[1] << '\n'
				<< "Z Pos: " << frame_bulk_status.pos[2] << '\n';

			out << "Health: " << frame_bulk_status.health << '\n';
			out << "Armor: " << frame_bulk_status.armor << '\n';
		} else {
			out << " no frame bulk selected";
		}

		DrawMultilineString(x, y, out.str());
	}

	static void DrawEntities(float flTime)
	{
		if (!CVars::bxt_hud_entities.GetBool())
			return;

		int x, y;
		GetPosition(CVars::bxt_hud_entities_offset, CVars::bxt_hud_entities_anchor, &x, &y, 2, (si.iCharHeight * 3) + 2);

		const auto max_lines = std::max(1, (si.iHeight - y - si.iCharHeight) / si.iCharHeight);
		int current_line = 0;

		std::ostringstream out;

		const auto& hw = HwDLL::GetInstance();

		edict_t *edicts;
		const int numEdicts = hw.GetEdicts(&edicts);
		for (int e = 0; e < numEdicts; ++e) {
			const edict_t *ent = edicts + e;
			if (!hw.IsValidEdict(ent))
				continue;

			const char *classname = hw.GetString(ent->v.classname);
			out << e << ": " << classname;

			if (ent->v.targetname != 0) {
				const char *targetname = hw.GetString(ent->v.targetname);
				out << " - " << targetname;
			}

			out << '\n';

			if (++current_line == max_lines) {
				x = DrawMultilineString(x, y, out.str()) + 10;
				out.str(std::string());
				current_line = 0;
			}
		}

		if (current_line > 0)
			DrawMultilineString(x, y, out.str());
	}

	static void DrawCrosshair(float time)
	{
		if (!CVars::bxt_cross.GetBool())
			return;

		unsigned char alpha;
		if (sscanf(CVars::bxt_cross_alpha.GetString().c_str(), "%hhu", &alpha) != 1)
			alpha = 255;

		if (alpha == 0)
			return;

		unsigned char r, g, b;
		if (sscanf(CVars::bxt_cross_color.GetString().c_str(), "%hhu %hhu %hhu", &r, &g, &b) != 3) {
			r = 0;
			g = 255;
			b = 0;
		}

		Vector2D center(si.iWidth / 2.0f, si.iHeight / 2.0f);

		GLUtils gl;

		// Draw the outline.
		if (CVars::bxt_cross_outline.GetBool()) {
			gl.color(0, 0, 0, alpha);
			gl.line_width(CVars::bxt_cross_outline.GetFloat());

			auto size = CVars::bxt_cross_size.GetFloat();
			auto gap = CVars::bxt_cross_gap.GetFloat();
			auto half_thickness = CVars::bxt_cross_thickness.GetFloat() / 2.0f;
			auto half_width = CVars::bxt_cross_outline.GetFloat() / 2.0f;
			auto offset = half_thickness + half_width;

			// Top line
			if (CVars::bxt_cross_top_line.GetBool()) {
				gl.line(Vector2D(center.x - offset, center.y - gap - size), Vector2D(center.x + offset, center.y - gap - size));
				gl.line(Vector2D(center.x + half_thickness, center.y - gap - size + half_width), Vector2D(center.x + half_thickness, center.y - gap - half_width));
				gl.line(Vector2D(center.x + offset, center.y - gap), Vector2D(center.x - offset, center.y - gap));
				gl.line(Vector2D(center.x - half_thickness, center.y - gap - half_width), Vector2D(center.x - half_thickness, center.y - gap - size + half_width));
			}

			// Bottom line
			if (CVars::bxt_cross_bottom_line.GetBool()) {
				gl.line(Vector2D(center.x - offset, center.y + gap + size), Vector2D(center.x + offset, center.y + gap + size));
				gl.line(Vector2D(center.x + half_thickness, center.y + gap + size - half_width), Vector2D(center.x + half_thickness, center.y + gap + half_width));
				gl.line(Vector2D(center.x + offset, center.y + gap), Vector2D(center.x - offset, center.y + gap));
				gl.line(Vector2D(center.x - half_thickness, center.y + gap + half_width), Vector2D(center.x - half_thickness, center.y + gap + size - half_width));
			}

			// Left line
			if (CVars::bxt_cross_left_line.GetBool()) {
				gl.line(Vector2D(center.x - gap - size, center.y - offset), Vector2D(center.x - gap - size, center.y + offset));
				gl.line(Vector2D(center.x - gap - size + half_width, center.y + half_thickness), Vector2D(center.x - gap - half_width, center.y + half_thickness));
				gl.line(Vector2D(center.x - gap, center.y + offset), Vector2D(center.x - gap, center.y - offset));
				gl.line(Vector2D(center.x - gap - half_width, center.y - half_thickness), Vector2D(center.x - gap - size + half_width, center.y - half_thickness));
			}

			// Right line
			if (CVars::bxt_cross_right_line.GetBool()) {
				gl.line(Vector2D(center.x + gap + size, center.y - offset), Vector2D(center.x + gap + size, center.y + offset));
				gl.line(Vector2D(center.x + gap + size - half_width, center.y + half_thickness), Vector2D(center.x + gap + half_width, center.y + half_thickness));
				gl.line(Vector2D(center.x + gap, center.y + offset), Vector2D(center.x + gap, center.y - offset));
				gl.line(Vector2D(center.x + gap + half_width, center.y - half_thickness), Vector2D(center.x + gap + size - half_width, center.y - half_thickness));
			}

			// Dot
			if (CVars::bxt_cross_dot_size.GetBool()) {
				auto size = CVars::bxt_cross_dot_size.GetFloat();
				auto offset = Vector2D(size / 2.0f, size / 2.0f);

				gl.line(Vector2D(center.x - offset.x - half_width, center.y - offset.y), Vector2D(center.x + offset.x + half_width, center.y - offset.y));
				gl.line(Vector2D(center.x + offset.x, center.y - offset.y + half_width), Vector2D(center.x + offset.x, center.y + offset.y - half_width));
				gl.line(Vector2D(center.x - offset.x, center.y - offset.y + half_width), Vector2D(center.x - offset.x, center.y + offset.y - half_width));
				gl.line(Vector2D(center.x - offset.x - half_width, center.y + offset.y), Vector2D(center.x + offset.x + half_width, center.y + offset.y));
			}
		}

		gl.color(r, g, b, alpha);

		// Draw the crosshairs.
		if (CVars::bxt_cross_thickness.GetBool()) {
			gl.line_width(CVars::bxt_cross_thickness.GetFloat());

			auto size = CVars::bxt_cross_size.GetFloat();
			auto gap = CVars::bxt_cross_gap.GetFloat();

			if (CVars::bxt_cross_top_line.GetBool())
				gl.line(Vector2D(center.x, center.y - gap - size), Vector2D(center.x, center.y - gap));
			if (CVars::bxt_cross_bottom_line.GetBool())
				gl.line(Vector2D(center.x, center.y + gap + size), Vector2D(center.x, center.y + gap));
			if (CVars::bxt_cross_left_line.GetBool())
				gl.line(Vector2D(center.x - gap - size, center.y), Vector2D(center.x - gap, center.y));
			if (CVars::bxt_cross_right_line.GetBool())
				gl.line(Vector2D(center.x + gap + size, center.y), Vector2D(center.x + gap, center.y));
		}

		// Draw the circle.
		if (CVars::bxt_cross_circle_radius.GetBool()) {
			gl.line_width(1.0f);

			static float old_circle_radius = 0;
			static std::vector<Vector2D> circle_points;

			auto radius = CVars::bxt_cross_circle_radius.GetFloat();
			if (old_circle_radius != radius) {
				// Recompute the circle points.
				circle_points = gl.compute_circle(radius);
				old_circle_radius = radius;
			}

			gl.circle(center, circle_points);
		}

		// Draw the dot.
		if (CVars::bxt_cross_dot_size.GetBool()) {
			unsigned char r, g, b;
			if (sscanf(CVars::bxt_cross_dot_color.GetString().c_str(), "%hhu %hhu %hhu", &r, &g, &b) == 3)
				gl.color(r, g, b, alpha);

			auto size = CVars::bxt_cross_dot_size.GetFloat();
			auto offset = Vector2D(size / 2.0f, size / 2.0f);

			gl.rectangle(center - offset, center + offset);
		}
	}

	void DrawStamina(float flTime)
	{
		if (CVars::bxt_hud_stamina.GetBool())
		{
			int x, y;
			GetPosition(CVars::bxt_hud_stamina_offset, CVars::bxt_hud_stamina_anchor, &x, &y, -75, si.iCharHeight * 4);

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "Stamina: ";

			if (frame_bulk_selected)
				out << frame_bulk_status.stamina;
			else
				out << player.stamina;

			DrawString(x, y, out.str().c_str());
		}
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

		DrawQuickGauss(flTime);
		DrawHealth(flTime);
		DrawArmor(flTime);
		DrawWaterlevel(flTime);
		DrawVelocity(flTime);
		DrawOrigin(flTime);
		DrawViewangles(flTime);
		DrawSpeedometer(flTime);
		DrawJumpspeed(flTime);
		DrawJumpDistance(flTime);
		DrawTimer(flTime);
		DrawDistance(flTime);
		DrawEntityInfo(flTime);
		DrawSelfgaussInfo(flTime);
		DrawVisibleLandmarks(flTime);
		DrawNihilanthInfo(flTime);
		DrawGonarchInfo(flTime);
		DrawIncorrectFPSIndicator(flTime);
		DrawCollisionDepthMap(flTime);
		DrawTASEditorStatus();
		DrawEntities(flTime);
		DrawCrosshair(flTime);
		DrawStamina(flTime);

		receivedAccurateInfo = false;
		frame_bulk_selected = false;
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
		invalidRun = false;
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

	void SetInvalidRun(bool invalidated)
	{
		// Only set as invalid, if the timer is actually running
		if (countingTime)
			invalidRun = invalidated;
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

	bool GetCountingTime()
	{
		return countingTime;
	}

	bool GetInvalidRun()
	{
		return invalidRun;
	}

	const SCREENINFO& GetScreenInfo()
	{
		return si;
	}

	void UpdateTASEditorStatus(const HLTAS::Frame& frame_bulk, const float& player_vel, const float& player_zvel, const float player_pos[3], const float& player_realyaw, const float& player_health, const float& player_armor, const float& player_stamina)
	{
		frame_bulk_selected = true;
		frame_bulk_status = FrameBulkStatus{};

		frame_bulk_status.vel = player_vel;
		frame_bulk_status.zvel = player_zvel;
		frame_bulk_status.pos[0] = player_pos[0];
		frame_bulk_status.pos[1] = player_pos[1];
		frame_bulk_status.pos[2] = player_pos[2];
		frame_bulk_status.realyaw = player_realyaw;

		frame_bulk_status.strafe = frame_bulk.Strafe;
		if (frame_bulk_status.strafe) {
			frame_bulk_status.strafe_type = frame_bulk.GetType();
			frame_bulk_status.strafe_dir = frame_bulk.GetDir();
		}

		frame_bulk_status.lgagst = frame_bulk.Lgagst;
		frame_bulk_status.autojump = frame_bulk.Autojump;
		frame_bulk_status.ducktap = frame_bulk.Ducktap;
		frame_bulk_status.jumpbug = frame_bulk.Jumpbug;
		frame_bulk_status.dbc = frame_bulk.Dbc;
		frame_bulk_status.dbc_ceilings = frame_bulk.GetDbcCeilings();
		frame_bulk_status.dbg = frame_bulk.Dbg;
		frame_bulk_status.dwj = frame_bulk.Dwj;

		frame_bulk_status.forward = frame_bulk.Forward;
		frame_bulk_status.left = frame_bulk.Left;
		frame_bulk_status.right = frame_bulk.Right;
		frame_bulk_status.back = frame_bulk.Back;
		frame_bulk_status.up = frame_bulk.Up;
		frame_bulk_status.down = frame_bulk.Down;

		frame_bulk_status.jump = frame_bulk.Jump;
		frame_bulk_status.duck = frame_bulk.Duck;
		frame_bulk_status.use = frame_bulk.Use;
		frame_bulk_status.attack1 = frame_bulk.Attack1;
		frame_bulk_status.attack2 = frame_bulk.Attack2;
		frame_bulk_status.reload = frame_bulk.Reload;

		frame_bulk_status.pitch_present = frame_bulk.PitchPresent;
		if (frame_bulk_status.pitch_present) {
			frame_bulk_status.pitch = frame_bulk.GetPitch();
		}

		frame_bulk_status.yaw_present = frame_bulk.HasYaw();
		if (frame_bulk_status.yaw_present) {
			frame_bulk_status.yaw = frame_bulk.GetYaw();
		}

		frame_bulk_status.count_present = frame_bulk.HasCount();
		if (frame_bulk_status.count_present) {
			frame_bulk_status.count = frame_bulk.GetCount();
		}

		frame_bulk_status.commands = frame_bulk.Commands;

		frame_bulk_status.health = player_health;
		frame_bulk_status.armor = player_armor;

		frame_bulk_status.stamina = player_stamina;

		frame_bulk_status.frametime = frame_bulk.Frametime;
	}
}
