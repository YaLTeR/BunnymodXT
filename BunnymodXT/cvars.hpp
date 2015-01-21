#pragma once

class CVarWrapper
{
public:
	CVarWrapper(bool freeOnDestruct = false) : m_CVar(nullptr), m_Serverside(false), m_FreeOnDestruct(freeOnDestruct) {}
	CVarWrapper(const char* name, const char* string, bool freeOnDestruct = false);
	~CVarWrapper();
	void Assign(cvar_t* cvar);
	cvar_t* GetPointer() const;

	bool IsEmpty() const;
	bool GetBool() const;
	int GetInt() const;
	float GetFloat() const;
	std::string GetString() const;

protected:
	cvar_t *m_CVar;
	bool m_Serverside;
	bool m_FreeOnDestruct;
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

inline float CVarWrapper::GetFloat() const
{
	return m_CVar ? m_CVar->value : 0.0f;
}

// Engine CVars
extern const CVarWrapper _bxt_taslog;
extern const CVarWrapper bxt_tas;

extern CVarWrapper sv_maxvelocity_;
extern CVarWrapper sv_maxspeed_;
extern CVarWrapper sv_stopspeed_;
extern CVarWrapper sv_friction_;
extern CVarWrapper sv_edgefriction_;
extern CVarWrapper sv_accelerate_;
extern CVarWrapper sv_airaccelerate_;
extern CVarWrapper sv_gravity_;

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
