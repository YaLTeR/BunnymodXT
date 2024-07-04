#pragma once

#define FIND_CVARS(X) \
	X(con_color) \
	X(sv_cheats) \
	X(sv_maxvelocity) \
	X(sv_maxspeed) \
	X(sv_stopspeed) \
	X(sv_friction) \
	X(edgefriction) \
	X(sv_accelerate) \
	X(sv_airaccelerate) \
	X(sv_gravity) \
	X(sv_stepsize) \
	X(sv_bounce) \
	X(fps_max) \
	X(default_fov) \
	X(skill) \
	X(host_framerate) \
	X(sensitivity) \
	X(coop) \
	X(r_shadows)

#define DEFINE_CVARS(X) \
	X(_bxt_taslog, "0") \
	X(_bxt_min_frametime, "0") \
	X(_bxt_norefresh, "0") \
	X(_bxt_bunnysplit_time_update_frequency, "41") \
	X(_bxt_save_runtime_data_in_demos, "1") \
	X(_bxt_tas_script_generation, "1337") \
	X(bxt_taslog_filename, "taslogger.log") \
	X(bxt_allow_keypresses_in_demo, "0") \
	X(bxt_autopause, "0") \
	X(bxt_interprocess_enable, "0") \
	X(bxt_shake_remove, "0") \
	X(bxt_fade_remove, "0") \
	X(bxt_skybox_remove, "0") \
	X(bxt_skybox_name, "") \
	X(bxt_water_remove, "0") \
	X(bxt_stop_demo_on_changelevel, "0") \
	X(bxt_tas_editor_simulate_for_ms, "40") \
	X(bxt_tas_editor_camera_editor, "0") \
	X(bxt_tas_editor_apply_smoothing_over_s, "0.15") \
	X(_bxt_tas_editor_apply_smoothing_high_weight_duration, "0.03") \
	X(_bxt_tas_editor_apply_smoothing_high_weight_multiplier, "3") \
	X(bxt_tas_editor_show_only_last_frames, "0") \
	X(bxt_tas_norefresh_until_last_frames, "0") \
	X(bxt_tas_write_log, "0") \
	X(bxt_tas_playback_speed, "1") \
	X(bxt_tas_ducktap_priority, "1") \
	X(bxt_disable_vgui, "0") \
	X(bxt_show_only_players, "0") \
	X(bxt_force_zmax, "0") \
	X(bxt_viewmodel_fov, "0") \
	X(bxt_viewmodel_disable_idle, "0") \
	X(bxt_viewmodel_disable_equip, "0") \
	X(bxt_viewmodel_semitransparent, "0") \
	X(bxt_clear_color, "") \
	X(bxt_force_fov, "0") \
	X(bxt_fix_widescreen_fov, "0") \
	X(bxt_force_clear, "0") \
	X(bxt_fix_mouse_horizontal_limit, "0") \
	X(bxt_hud_game_color, "") \
	X(bxt_disable_gamedir_check_in_demo, "0") \
	X(bxt_remove_fps_limit, "0") \
	X(bxt_cof_disable_save_lock, "0") \
	X(bxt_remove_viewmodel, "0") \
	X(bxt_disable_world, "0") \
	X(bxt_disable_particles, "0") \
	X(bxt_fix_changelevel_in_coop, "0") \
	X(bxt_fire_on_stuck, "") \
	X(bxt_autojump, "0") \
	X(bxt_autojump_priority, "1") \
	X(bxt_bhopcap, "1") \
	X(bxt_timer_autostop, "1") \
	X(bxt_show_hidden_entities, "0") \
	X(bxt_show_hidden_entities_clientside, "0") \
	X(bxt_show_triggers_legacy, "0") \
	X(bxt_show_triggers_legacy_alpha, "120") \
	X(bxt_show_pickup_bbox, "0") \
	X(bxt_show_player_bbox, "0") \
	X(bxt_show_monster_bbox, "0") \
	X(bxt_disable_autosave, "0") \
	X(bxt_disable_changelevel, "0") \
	X(bxt_force_duck, "0") \
	X(bxt_force_jumpless, "0") \
	X(bxt_render_far_entities, "0") \
	X(bxt_fire_on_mm_targetname, "") \
	X(bxt_fire_on_mm_command, "") \
	X(bxt_fire_on_button_target, "") \
	X(bxt_fire_on_button_command, "") \
	X(bxt_remove_stamina, "0") \
	X(bxt_cof_enable_ducktap, "0") \
	X(bxt_cof_slowdown_if_use_on_ground, "0") \
	X(bxt_cof_disable_monsters_teleport_to_spawn_after_load, "0") \
	X(bxt_cof_disable_viewpunch_from_jump, "0") \
	X(bxt_cof_allow_skipping_all_cutscenes, "0") \
	X(cl_righthand, "0") \
	X(bxt_remove_punchangles, "0") \
	X(bxt_disable_brush_entities, "0") \
	X(bxt_disable_sprite_entities, "0") \
	X(bxt_disable_studio_entities, "0") \
	X(bxt_disable_player_corpses, "0") \
	X(bxt_hide_other_players, "0") \
	X(bxt_disable_hud, "0") \
	X(bxt_disable_nightvision_sprite, "0") \
	X(bxt_autojump_prediction, "0") \
	X(bxt_bhopcap_prediction, "1") \
	X(bxt_show_nodes, "0") \
	X(bxt_show_displacer_earth_targets, "0") \
	X(bxt_show_custom_triggers, "1") \
	X(bxt_triggers_color, "") \
	X(bxt_wallhack, "0") \
	X(bxt_wallhack_additive, "0") \
	X(bxt_wallhack_alpha, "0.6") \
	X(bxt_novis, "0") \
	X(bxt_show_triggers, "0") \
	X(bxt_collision_depth_map, "0") \
	X(bxt_collision_depth_map_colors, "0") \
	X(bxt_collision_depth_map_hull, "2") \
	X(bxt_collision_depth_map_max_depth, "1000") \
	X(bxt_collision_depth_map_pixel_scale, "8") \
	X(bxt_collision_depth_map_remove_distance_limit, "1") \
	X(bxt_collision_depth_map_fov, "0") \
	X(bxt_unlock_camera_during_pause, "0") \
	X(bxt_hud, "1") \
	X(bxt_hud_color, "") \
	X(bxt_hud_game_alpha, "0") \
	X(bxt_hud_game_alpha_damage, "0") \
	X(bxt_hud_precision, "6") \
	X(bxt_hud_quickgauss, "0") \
	X(bxt_hud_quickgauss_offset, "") \
	X(bxt_hud_quickgauss_anchor, "0.5 0") \
	X(bxt_hud_checkpoint, "0") \
	X(bxt_hud_checkpoint_offset, "") \
	X(bxt_hud_checkpoint_anchor, "0.5 0") \
	X(bxt_hud_velocity, "0") \
	X(bxt_hud_velocity_offset, "") \
	X(bxt_hud_velocity_anchor, "1 0") \
	X(bxt_hud_origin, "0") \
	X(bxt_hud_origin_offset, "") \
	X(bxt_hud_origin_anchor, "1 0") \
	X(bxt_hud_viewangles, "0") \
	X(bxt_hud_viewangles_offset, "") \
	X(bxt_hud_viewangles_anchor, "1 0") \
	X(bxt_hud_distance, "0") \
	X(bxt_hud_distance_offset, "") \
	X(bxt_hud_distance_anchor, "1 0") \
	X(bxt_hud_entity_info, "0") \
	X(bxt_hud_entity_info_offset, "") \
	X(bxt_hud_entity_info_anchor, "1 0") \
	X(bxt_hud_selfgauss, "0") \
	X(bxt_hud_selfgauss_offset, "") \
	X(bxt_hud_selfgauss_anchor, "1 0") \
	X(bxt_hud_armor, "0") \
	X(bxt_hud_armor_offset, "") \
	X(bxt_hud_armor_anchor, "1 0") \
	X(bxt_hud_waterlevel, "0") \
	X(bxt_hud_waterlevel_offset, "") \
	X(bxt_hud_waterlevel_anchor, "1 0") \
	X(bxt_hud_speedometer, "1") \
	X(bxt_hud_speedometer_offset, "") \
	X(bxt_hud_speedometer_anchor, "0.5 1") \
	X(bxt_hud_jumpspeed, "0") \
	X(bxt_hud_jumpspeed_offset, "") \
	X(bxt_hud_jumpspeed_anchor, "0.5 1") \
	X(bxt_hud_jumpdistance, "0") \
	X(bxt_hud_jumpdistance_offset, "") \
	X(bxt_hud_jumpdistance_anchor, "0.5 1") \
	X(bxt_hud_health, "0") \
	X(bxt_hud_health_offset, "") \
	X(bxt_hud_health_anchor, "0.5 1") \
	X(bxt_hud_health_override_in_demo, "0") \
	X(bxt_hud_nihilanth, "0") \
	X(bxt_hud_nihilanth_offset, "") \
	X(bxt_hud_nihilanth_anchor, "1 0") \
	X(bxt_hud_gonarch, "0") \
	X(bxt_hud_gonarch_offset, "") \
	X(bxt_hud_gonarch_anchor, "1 0") \
	X(bxt_hud_timer, "0") \
	X(bxt_hud_timer_offset, "") \
	X(bxt_hud_timer_anchor, "0 0.5") \
	X(bxt_hud_visible_landmarks, "0") \
	X(bxt_hud_visible_landmarks_offset, "") \
	X(bxt_hud_visible_landmarks_anchor, "0.5 0") \
	X(bxt_hud_incorrect_fps_indicator, "1") \
	X(bxt_hud_useables, "0") \
	X(bxt_hud_useables_radius, "64") \
	X(bxt_hud_tas_editor_status, "0") \
	X(bxt_hud_tas_editor_status_offset, "") \
	X(bxt_hud_tas_editor_status_anchor, "0 0") \
	X(bxt_hud_entities, "0") \
	X(bxt_hud_entities_offset, "") \
	X(bxt_hud_entities_anchor, "0 0") \
	X(bxt_hud_stamina, "0") \
	X(bxt_hud_stamina_offset, "") \
	X(bxt_hud_stamina_anchor, "0.5 0") \
	X(bxt_hud_split, "0") \
	X(bxt_hud_split_speed, "0") \
	X(bxt_hud_split_offset, "") \
	X(bxt_hud_split_anchor, "") \
	X(bxt_hud_split_duration, "1") \
	X(bxt_hud_split_fadeout, "5") \
	X(bxt_cross, "0") \
	X(bxt_cross_color, "") \
	X(bxt_cross_alpha, "") \
	X(bxt_cross_thickness, "2") \
	X(bxt_cross_size, "10") \
	X(bxt_cross_gap, "3") \
	X(bxt_cross_outline, "0") \
	X(bxt_cross_circle_radius, "0") \
	X(bxt_cross_dot_color, "") \
	X(bxt_cross_dot_size, "0") \
	X(bxt_cross_top_line, "1") \
	X(bxt_cross_bottom_line, "1") \
	X(bxt_cross_left_line, "1") \
	X(bxt_cross_right_line, "1") \
	X(bxt_viewmodel_ofs_forward, "0") \
	X(bxt_viewmodel_ofs_right, "0") \
	X(bxt_viewmodel_ofs_up, "0") \
	X(bxt_viewmodel_bob_angled, "0") \
	X(bxt_viewmodel_restore_viewroll, "0") \
	X(bxt_viewmodel_viewrollangle, "2.0") \
	X(bxt_viewmodel_viewrollspeed, "200") \
	X(bxt_show_bullets, "0") \
	X(bxt_show_bullets_enemy, "0") \
	X(bxt_anglespeed_cap, "1") \
	X(bxt_speed_scaling, "1") \
	X(bxt_show_splits, "0") \
	X(bxt_splits_color, "") \
	X(bxt_splits_print, "1") \
	X(bxt_splits_print_times_at_end, "1") \
	X(bxt_splits_autorecord_on_first_split, "") \
	X(bxt_splits_start_timer_on_first_split, "0") \
	X(bxt_splits_end_on_last_split, "0") \
	X(bxt_ch_hook_speed, "869") \
	X(bxt_ch_checkpoint_with_vel, "1") \
	X(bxt_ch_checkpoint_onground_only, "0") \
	X(bxt_ch_fix_sticky_slide, "0") \
	X(bxt_ch_fix_sticky_slide_offset, "0.01")

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

#define EXTERN_FIND_WRAPPER(name) extern CVarWrapper name;
#define EXTERN_DEFINE_WRAPPER(name, string) extern CVarWrapper name;

namespace CVars
{
	FIND_CVARS(EXTERN_FIND_WRAPPER)
	DEFINE_CVARS(EXTERN_DEFINE_WRAPPER)
	
	extern const std::vector<CVarWrapper*> allCVars;
}
