#include "stdafx.hpp"

#include <sstream>

#include "cvars.hpp"
#include "modules.hpp"
#include "hud_custom.hpp"

namespace CustomHud
{
	static SCREENINFO si;
	static float consoleColor[3] = { 1.0f, (180 / 255.0f), (30 / 255.0f) };
	static bool receivedAccurateInfo = false;
	static float velocity[3];
	static float origin[3];

	static double length(double x, double y)
	{
		return std::sqrt((x * x) + (y * y));
	}

	static double length(double x, double y, double z)
	{
		return std::sqrt((x * x) + (y * y) + (z * z));
	}

	static void UpdateScreenInfo()
	{
		si.iSize = sizeof(si);
		clientDLL.pEngfuncs->pfnGetScreenInfo(&si);
	}

	static void DrawString(int x, int y, char* s, float r, float g, float b)
	{
		clientDLL.pEngfuncs->pfnDrawSetTextColor(r, g, b);
		clientDLL.pEngfuncs->pfnDrawConsoleString(x, y, s);
	}

	static void DrawString(int x, int y, char* s)
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

	static void UpdateConsoleColor()
	{
		if (!con_color)
			return;

		unsigned r = 0, g = 0, b = 0;
		std::istringstream ss(con_color->string);
		ss >> r >> g >> b;

		consoleColor[0] = r / 255.0f;
		consoleColor[1] = g / 255.0f;
		consoleColor[2] = b / 255.0f;
	}

	void Init()
	{
	}

	void VidInit()
	{
		UpdateScreenInfo();
	}

	void Draw(float flTime)
	{
		if (y_bxt_hud && (y_bxt_hud->value == 0.0f))
			return;

		// Let's just assume that every HUD-related cvar was successfully allocated by that point,
		// that'll save us from a huge amount of spaghetti code.
		unsigned precision = 0;
		std::istringstream ss(y_bxt_hud_precision->string);
		ss >> precision;
		if (precision > 16)
			precision = 16;

		UpdateConsoleColor();
		
		if (y_bxt_hud_velocity->value != 0.0f)
		{
			int x = 0, y = 0;
			std::istringstream pos_ss(y_bxt_hud_velocity_pos->string);
			pos_ss >> x >> y;
			x += si.iWidth;
			
			if (receivedAccurateInfo)
				DrawString(x, y, "Velocity:");
			else
				DrawString(x, y, "Velocity:", 1.0f, 0.0f, 0.0f);

			y += si.iCharHeight;
			
			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "X: " << velocity[0] << "\n"
				<< "Y: " << velocity[1] << "\n"
				<< "Z: " << velocity[2] << "\n"
				<< "XY: " << length(velocity[0], velocity[1]) << "\n"
				<< "XYZ: " << length(velocity[0], velocity[1], velocity[2]);

			DrawMultilineString(x, y, out.str());
		}

		if (y_bxt_hud_origin->value != 0.0f)
		{
			int x = 0, y = 0;
			std::istringstream pos_ss(y_bxt_hud_origin_pos->string);
			pos_ss >> x >> y;
			x += si.iWidth;

			if (receivedAccurateInfo)
				DrawString(x, y, "Origin:");
			else
				DrawString(x, y, "Origin:", 1.0f, 0.0f, 0.0f);

			y += si.iCharHeight;

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "X: " << origin[0] << "\n"
				<< "Y: " << origin[1] << "\n"
				<< "Z: " << origin[2];

			DrawMultilineString(x, y, out.str());
		}

		receivedAccurateInfo = false;
	}

	void UpdatePlayerInfo(float vel[3], float org[3])
	{
		velocity[0] = vel[0];
		velocity[1] = vel[1];
		velocity[2] = vel[2];
		origin[0] = org[0];
		origin[1] = org[1];
		origin[2] = org[2];

		receivedAccurateInfo = true;
	}

	void UpdatePlayerInfoInaccurate(float vel[3], float org[3])
	{
		if (!receivedAccurateInfo)
		{
			velocity[0] = vel[0];
			velocity[1] = vel[1];
			velocity[2] = vel[2];
			origin[0] = org[0];
			origin[1] = org[1];
			origin[2] = org[2];
		}
	}
}

int CHudCustom_Wrapper::Init()
{
	CustomHud::Init();

	m_Initialized = true;
	m_iFlags = HUD_ACTIVE;
	clientDLL.AddHudElem(this);

	return 1;
}

int CHudCustom_Wrapper_NoVD::Init()
{
	CustomHud::Init();

	m_Initialized = true;
	m_iFlags = HUD_ACTIVE;
	clientDLL.AddHudElem(this);

	return 1;
}
