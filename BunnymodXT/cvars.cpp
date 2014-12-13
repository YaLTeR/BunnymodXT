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
const CVarWrapper y_bxt_autojump("y_bxt_autojump", "0");
const CVarWrapper y_bxt_bhopcap("y_bxt_bhopcap", "0");
const CVarWrapper _y_bxt_taslog("_y_bxt_taslog", "0");

// Clientside CVars
CVarWrapper con_color_;
CVarWrapper y_bxt_autojump_prediction;
CVarWrapper y_bxt_bhopcap_prediction;
CVarWrapper y_bxt_hud;
CVarWrapper y_bxt_hud_precision;
CVarWrapper y_bxt_hud_velocity;
CVarWrapper y_bxt_hud_velocity_pos;
CVarWrapper y_bxt_hud_origin;
CVarWrapper y_bxt_hud_origin_pos;
