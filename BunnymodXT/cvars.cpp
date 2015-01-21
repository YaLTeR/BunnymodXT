#include "stdafx.hpp"

#include "cvars.hpp"

CVarWrapper::CVarWrapper(const char* name, const char* string, bool freeOnDestruct)
{
	m_CVar = new cvar_t;

	// The engine should copy the strings as a part of the
	// CVar registering procedure
	m_CVar->name = const_cast<char*>(name);
	m_CVar->string = const_cast<char*>(string);

	m_Serverside = true;
	m_FreeOnDestruct = freeOnDestruct;
}

CVarWrapper::~CVarWrapper()
{
	if (m_FreeOnDestruct)
		delete m_CVar;
}

void CVarWrapper::Assign(cvar_t* cvar)
{
	assert(!m_Serverside);
	m_CVar = cvar;
}

std::string CVarWrapper::GetString() const
{
	if (!m_CVar)
		return std::string();
	return std::string(m_CVar->string);
}

// Engine CVars
const CVarWrapper _bxt_taslog("_bxt_taslog", "0");
const CVarWrapper bxt_tas("bxt_tas", "0");
CVarWrapper sv_maxvelocity_;
CVarWrapper sv_maxspeed_;
CVarWrapper sv_stopspeed_;
CVarWrapper sv_friction_;
CVarWrapper sv_edgefriction_;
CVarWrapper sv_accelerate_;
CVarWrapper sv_airaccelerate_;
CVarWrapper sv_gravity_;

// Serverside CVars
const CVarWrapper bxt_autojump("bxt_autojump", "0");
const CVarWrapper bxt_bhopcap("bxt_bhopcap", "1");

// Clientside CVars
CVarWrapper con_color_;
CVarWrapper bxt_autojump_prediction;
CVarWrapper bxt_bhopcap_prediction;
CVarWrapper bxt_hud;
CVarWrapper bxt_hud_color;
CVarWrapper bxt_hud_precision;
CVarWrapper bxt_hud_velocity;
CVarWrapper bxt_hud_velocity_offset;
CVarWrapper bxt_hud_velocity_anchor;
CVarWrapper bxt_hud_origin;
CVarWrapper bxt_hud_origin_offset;
CVarWrapper bxt_hud_origin_anchor;
CVarWrapper bxt_hud_speedometer;
CVarWrapper bxt_hud_speedometer_offset;
CVarWrapper bxt_hud_speedometer_anchor;
CVarWrapper bxt_hud_jumpspeed;
CVarWrapper bxt_hud_jumpspeed_offset;
CVarWrapper bxt_hud_jumpspeed_anchor;
