#include "stdafx.hpp"

#include <sstream>

#include "cvars.hpp"
#include "modules.hpp"
#include "hud_custom.hpp"

namespace CustomHud
{
	static SCREENINFO si;
	static float velocity[3];

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

	static void DrawString(int x, int y, std::string s)
	{
		while (s.size() > 0)
		{
			auto pos = s.find('\n');

			clientDLL.pEngfuncs->pfnDrawConsoleString(x, y, const_cast<char*>(s.substr(0, pos).c_str()));
			y += si.iCharHeight;

			if (pos != std::string::npos)
				s = s.substr(pos + 1, std::string::npos);
			else
				s.erase();
		};
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

		clientDLL.pEngfuncs->pfnDrawSetTextColor(g_DefaultTextColor[0], g_DefaultTextColor[1], g_DefaultTextColor[2]);
		
		if (y_bxt_hud_velocity->value != 0.0f)
		{
			int x = 0, y = 0;
			std::istringstream pos_ss(y_bxt_hud_velocity_pos->string);
			pos_ss >> x >> y;
			
			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "Velocity:\n"
				<< "X: " << velocity[0] << "\n"
				<< "Y: " << velocity[1] << "\n"
				<< "Z: " << velocity[2] << "\n"
				<< "XY: " << length(velocity[0], velocity[1]) << "\n"
				<< "XYZ: " << length(velocity[0], velocity[1], velocity[2]);

			DrawString(si.iWidth + x, y, out.str());
		}
	}

	void UpdateVelocity(float vel[3])
	{
		velocity[0] = vel[0];
		velocity[1] = vel[1];
		velocity[2] = vel[2];
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
