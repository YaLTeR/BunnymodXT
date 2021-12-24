#pragma once

class CVarWrapper
{
public:
	CVarWrapper() : m_Reference(true) {}
	CVarWrapper(const char* name, const char* string);
	~CVarWrapper();
	void MarkAsStale();
	void Refresh();
	void Assign(cvar_t* cvar);
	cvar_t* GetPointer() const;

	bool IsEmpty() const;
	bool GetBool() const;
	int GetInt() const;
	unsigned GetUint() const;
	float GetFloat() const;
	std::string GetString() const;

	// Only use before registering!
	void Set(const char* string);

protected:
	cvar_t *m_CVar = nullptr;
	const char* m_String = nullptr;
	bool m_StaleString = false;
	bool m_Reference = false;
};

inline CVarWrapper::CVarWrapper(const char* name, const char* string)
{
	m_CVar = new cvar_t;
	m_CVar->name = const_cast<char*>(name);
	m_String = string;
	m_CVar->string = const_cast<char*>(m_String);
	m_CVar->flags = 0;
	m_CVar->value = static_cast<float>(std::atof(m_String));
	m_CVar->next = nullptr;
}

inline CVarWrapper::~CVarWrapper()
{
	if (!m_Reference)
		delete m_CVar;
}

inline void CVarWrapper::MarkAsStale()
{
	assert(!m_Reference);
	m_StaleString = true;
}

inline void CVarWrapper::Refresh()
{
	if (m_StaleString)
	{
		assert(!m_Reference);
		m_StaleString = false;
		m_CVar->string = const_cast<char*>(m_String);
		m_CVar->value = static_cast<float>(std::atof(m_String));
	} else if (m_Reference)
		m_CVar = nullptr;
}

inline void CVarWrapper::Assign(cvar_t* cvar)
{
	assert(m_Reference);
	m_CVar = cvar;
}

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

inline unsigned CVarWrapper::GetUint() const
{
	return m_CVar ? std::strtoul(m_CVar->string, NULL, 10) : 0;
}

inline float CVarWrapper::GetFloat() const
{
	return m_CVar ? m_CVar->value : 0.0f;
}

inline std::string CVarWrapper::GetString() const
{
	if (!m_CVar)
		return std::string();
	return std::string(m_CVar->string);
}

inline void CVarWrapper::Set(const char* string)
{
	assert(!m_Reference);

	m_String = string;
	m_CVar->string = const_cast<char*>(m_String);
	m_CVar->value = static_cast<float>(std::atof(m_String));
}

namespace CVars
{
	// Engine CVars
	extern CVarWrapper _bxt_taslog;
	extern CVarWrapper _bxt_min_frametime;
	extern CVarWrapper _bxt_norefresh;
	extern CVarWrapper _bxt_bunnysplit_time_update_frequency;
	extern CVarWrapper _bxt_save_runtime_data_in_demos;
	extern CVarWrapper _bxt_tas_script_generation;
	extern CVarWrapper bxt_taslog_filename;
	extern CVarWrapper bxt_autopause;
	extern CVarWrapper bxt_interprocess_enable;
	extern CVarWrapper bxt_fade_remove;
	extern CVarWrapper bxt_skybox_remove;
	extern CVarWrapper bxt_water_remove;
	extern CVarWrapper bxt_stop_demo_on_changelevel;
	extern CVarWrapper bxt_tas_editor_simulate_for_ms;
	extern CVarWrapper bxt_tas_editor_camera_editor;
	extern CVarWrapper bxt_tas_editor_apply_smoothing_over_s;
	extern CVarWrapper bxt_tas_norefresh_until_last_frames;
	extern CVarWrapper bxt_tas_write_log;
	extern CVarWrapper bxt_tas_playback_speed;
	extern CVarWrapper bxt_disable_vgui;
	extern CVarWrapper bxt_show_only_viewmodel;
	extern CVarWrapper bxt_force_zmax;
	extern CVarWrapper bxt_viewmodel_fov;
	extern CVarWrapper bxt_viewmodel_disable_idle;
	extern CVarWrapper bxt_viewmodel_disable_equip;
	extern CVarWrapper bxt_clear_green;

	extern CVarWrapper con_color;
	extern CVarWrapper sv_cheats;
	extern CVarWrapper sv_maxvelocity;
	extern CVarWrapper sv_maxspeed;
	extern CVarWrapper sv_stopspeed;
	extern CVarWrapper sv_friction;
	extern CVarWrapper edgefriction;
	extern CVarWrapper sv_accelerate;
	extern CVarWrapper sv_airaccelerate;
	extern CVarWrapper sv_gravity;
	extern CVarWrapper sv_stepsize;
	extern CVarWrapper sv_bounce;
	extern CVarWrapper fps_max;
	extern CVarWrapper default_fov;

	// Serverside CVars
	extern CVarWrapper bxt_autojump;
	extern CVarWrapper bxt_bhopcap;
	extern CVarWrapper bxt_timer_autostop;
	extern CVarWrapper bxt_show_hidden_entities;
	extern CVarWrapper bxt_show_hidden_entities_clientside;
	extern CVarWrapper bxt_show_triggers_legacy;
	extern CVarWrapper bxt_show_pickup_bbox;
	extern CVarWrapper bxt_disable_autosave;
	extern CVarWrapper bxt_disable_changelevel;

	// Clientside CVars
	extern CVarWrapper bxt_disable_hud;
	extern CVarWrapper bxt_autojump_prediction;
	extern CVarWrapper bxt_bhopcap_prediction;
	extern CVarWrapper bxt_show_nodes;
	extern CVarWrapper bxt_show_displacer_earth_targets;
	extern CVarWrapper bxt_show_custom_triggers;
	extern CVarWrapper bxt_triggers_color;
	extern CVarWrapper bxt_wallhack;
	extern CVarWrapper bxt_wallhack_additive;
	extern CVarWrapper bxt_wallhack_alpha;
	extern CVarWrapper bxt_novis;
	extern CVarWrapper bxt_show_triggers;
	extern CVarWrapper bxt_collision_depth_map;
	extern CVarWrapper bxt_collision_depth_map_colors;
	extern CVarWrapper bxt_collision_depth_map_hull;
	extern CVarWrapper bxt_collision_depth_map_max_depth;
	extern CVarWrapper bxt_collision_depth_map_pixel_scale;
	extern CVarWrapper bxt_collision_depth_map_remove_distance_limit;
	extern CVarWrapper bxt_unlock_camera_during_pause;
	extern CVarWrapper bxt_show_player_in_hltv;
	extern CVarWrapper bxt_hud;
	extern CVarWrapper bxt_hud_color;
	extern CVarWrapper bxt_hud_precision;
	extern CVarWrapper bxt_hud_quickgauss;
	extern CVarWrapper bxt_hud_quickgauss_offset;
	extern CVarWrapper bxt_hud_quickgauss_anchor;
	extern CVarWrapper bxt_hud_velocity;
	extern CVarWrapper bxt_hud_velocity_offset;
	extern CVarWrapper bxt_hud_velocity_anchor;
	extern CVarWrapper bxt_hud_origin;
	extern CVarWrapper bxt_hud_origin_offset;
	extern CVarWrapper bxt_hud_origin_anchor;
	extern CVarWrapper bxt_hud_viewangles;
	extern CVarWrapper bxt_hud_viewangles_offset;
	extern CVarWrapper bxt_hud_viewangles_anchor;
	extern CVarWrapper bxt_hud_distance;
	extern CVarWrapper bxt_hud_distance_offset;
	extern CVarWrapper bxt_hud_distance_anchor;
	extern CVarWrapper bxt_hud_entity_info;
	extern CVarWrapper bxt_hud_entity_info_offset;
	extern CVarWrapper bxt_hud_entity_info_anchor;
	extern CVarWrapper bxt_hud_selfgauss;
	extern CVarWrapper bxt_hud_selfgauss_offset;
	extern CVarWrapper bxt_hud_selfgauss_anchor;
	extern CVarWrapper bxt_hud_armor;
	extern CVarWrapper bxt_hud_armor_offset;
	extern CVarWrapper bxt_hud_armor_anchor;
	extern CVarWrapper bxt_hud_speedometer;
	extern CVarWrapper bxt_hud_speedometer_offset;
	extern CVarWrapper bxt_hud_speedometer_anchor;
	extern CVarWrapper bxt_hud_jumpspeed;
	extern CVarWrapper bxt_hud_jumpspeed_offset;
	extern CVarWrapper bxt_hud_jumpspeed_anchor;
	extern CVarWrapper bxt_hud_health;
	extern CVarWrapper bxt_hud_health_offset;
	extern CVarWrapper bxt_hud_health_anchor;
	extern CVarWrapper bxt_hud_nihilanth;
	extern CVarWrapper bxt_hud_nihilanth_offset;
	extern CVarWrapper bxt_hud_nihilanth_anchor;
	extern CVarWrapper bxt_hud_gonarch;
	extern CVarWrapper bxt_hud_gonarch_offset;
	extern CVarWrapper bxt_hud_gonarch_anchor;
	extern CVarWrapper bxt_hud_timer;
	extern CVarWrapper bxt_hud_timer_offset;
	extern CVarWrapper bxt_hud_timer_anchor;
	extern CVarWrapper bxt_hud_visible_landmarks;
	extern CVarWrapper bxt_hud_visible_landmarks_offset;
	extern CVarWrapper bxt_hud_visible_landmarks_anchor;
	extern CVarWrapper bxt_hud_incorrect_fps_indicator;
	extern CVarWrapper bxt_hud_useables;
	extern CVarWrapper bxt_hud_useables_radius;
	extern CVarWrapper bxt_hud_tas_editor_status;
	extern CVarWrapper bxt_hud_tas_editor_status_offset;
	extern CVarWrapper bxt_hud_tas_editor_status_anchor;
	extern CVarWrapper bxt_hud_entities;
	extern CVarWrapper bxt_hud_entities_offset;
	extern CVarWrapper bxt_hud_entities_anchor;
	extern CVarWrapper bxt_cross;
	extern CVarWrapper bxt_cross_color;
	extern CVarWrapper bxt_cross_alpha;
	extern CVarWrapper bxt_cross_thickness;
	extern CVarWrapper bxt_cross_size;
	extern CVarWrapper bxt_cross_gap;
	extern CVarWrapper bxt_cross_outline;
	extern CVarWrapper bxt_cross_circle_radius;
	extern CVarWrapper bxt_cross_dot_color;
	extern CVarWrapper bxt_cross_dot_size;
	extern CVarWrapper bxt_cross_top_line;
	extern CVarWrapper bxt_cross_bottom_line;
	extern CVarWrapper bxt_cross_left_line;
	extern CVarWrapper bxt_cross_right_line;
	extern CVarWrapper bxt_viewmodel_ofs_forward;
	extern CVarWrapper bxt_viewmodel_ofs_right;
	extern CVarWrapper bxt_viewmodel_ofs_up;

	extern const std::vector<CVarWrapper*> allCVars;
}
