#include "stdafx.hpp"

#include "cvars.hpp"
#include "modules.hpp"
#include "hud_custom.hpp"

void CustomHud::Init()
{
}

void CustomHud::VidInit()
{
}

void CustomHud::Draw(float flTime)
{
	if (y_bxt_hud && (y_bxt_hud->value == 0.0f))
		return;

	clientDLL.pEngfuncs->pfnDrawSetTextColor(g_DefaultTextColor[0], g_DefaultTextColor[1], g_DefaultTextColor[2]);
	clientDLL.pEngfuncs->pfnDrawConsoleString(0, 100, "Drawin' text in yo face");
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
