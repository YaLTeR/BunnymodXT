#include "stdafx.hpp"

#include "cvars.hpp"

CVarWrapper::CVarWrapper()
{
	m_CVar = nullptr;
	m_Serverside = false;
}

CVarWrapper::CVarWrapper(const char* name, const char* string)
{
	m_CVar = new cvar_t;

	// The engine should copy the strings as a part of the
	// CVar registering procedure
	m_CVar->name = const_cast<char*>(name);
	m_CVar->string = const_cast<char*>(string);

	m_Serverside = true;
}

CVarWrapper::~CVarWrapper()
{
	if (m_Serverside)
		delete m_CVar;
}

void CVarWrapper::Assign(cvar_t* cvar)
{
	assert(!m_CVar);
	assert(!m_Serverside);

	m_CVar = cvar;
}

std::string CVarWrapper::GetString() const
{
	if (!m_CVar)
		return std::string();

	return std::string(m_CVar->string);
}

// Serverside CVars
const CVarWrapper bxt_autojump("bxt_autojump", "0");
const CVarWrapper bxt_bhopcap("bxt_bhopcap", "1");
const CVarWrapper _bxt_taslog("_bxt_taslog", "0");

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
