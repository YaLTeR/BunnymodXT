#pragma once

#include "stdafx.hpp"

class CVarWrapper
{
public:
	CVarWrapper();
	CVarWrapper(const char* name, const char* string);
	~CVarWrapper();
	void Assign(cvar_t* cvar);
	cvar_t* GetPointer() const;

	bool IsEmpty() const;
	bool GetBool() const;
	int GetInt() const;
	std::string GetString() const;

protected:
	cvar_t *m_CVar;
	bool m_Serverside;
};

// Serverside CVars
extern const CVarWrapper y_bxt_autojump;
extern const CVarWrapper y_bxt_bhopcap;
extern const CVarWrapper _y_bxt_taslog;

// Clientside CVars
extern CVarWrapper con_color_;
extern CVarWrapper y_bxt_autojump_prediction;
extern CVarWrapper y_bxt_bhopcap_prediction;
extern CVarWrapper y_bxt_hud;
extern CVarWrapper y_bxt_hud_precision;
extern CVarWrapper y_bxt_hud_velocity;
extern CVarWrapper y_bxt_hud_velocity_pos;
extern CVarWrapper y_bxt_hud_origin;
extern CVarWrapper y_bxt_hud_origin_pos;
