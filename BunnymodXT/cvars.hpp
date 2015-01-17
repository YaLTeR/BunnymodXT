#pragma once

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

inline cvar_t* CVarWrapper::GetPointer() const
{
	return m_CVar;
}

inline bool CVarWrapper::IsEmpty() const
{
	return !m_CVar || !m_CVar->string[0];
}

inline bool CVarWrapper::GetBool() const
{
	return m_CVar && (m_CVar->value != 0.0f);
}

inline int CVarWrapper::GetInt() const
{
	return m_CVar ? atoi(m_CVar->string) : 0;
}

// Engine CVars
extern const CVarWrapper _bxt_taslog;
extern const CVarWrapper bxt_tas;

// Serverside CVars
extern const CVarWrapper bxt_autojump;
extern const CVarWrapper bxt_bhopcap;

// Clientside CVars
extern CVarWrapper con_color_;
extern CVarWrapper bxt_autojump_prediction;
extern CVarWrapper bxt_bhopcap_prediction;
extern CVarWrapper bxt_hud;
extern CVarWrapper bxt_hud_color;
extern CVarWrapper bxt_hud_precision;
extern CVarWrapper bxt_hud_velocity;
extern CVarWrapper bxt_hud_velocity_offset;
extern CVarWrapper bxt_hud_velocity_anchor;
extern CVarWrapper bxt_hud_origin;
extern CVarWrapper bxt_hud_origin_offset;
extern CVarWrapper bxt_hud_origin_anchor;
extern CVarWrapper bxt_hud_speedometer;
extern CVarWrapper bxt_hud_speedometer_offset;
extern CVarWrapper bxt_hud_speedometer_anchor;
extern CVarWrapper bxt_hud_jumpspeed;
extern CVarWrapper bxt_hud_jumpspeed_offset;
extern CVarWrapper bxt_hud_jumpspeed_anchor;
