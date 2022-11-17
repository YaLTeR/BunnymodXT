#include "stdafx.hpp"

#include "cvars.hpp"

namespace CVars
{
	// Engine CVars
	CVarWrapper _bxt_taslog("_bxt_taslog", "0");
	CVarWrapper _bxt_min_frametime("_bxt_min_frametime", "0");
	CVarWrapper _bxt_norefresh("_bxt_norefresh", "0");
	CVarWrapper _bxt_bunnysplit_time_update_frequency("_bxt_bunnysplit_time_update_frequency", "41");
	CVarWrapper _bxt_save_runtime_data_in_demos("_bxt_save_runtime_data_in_demos", "1");
	CVarWrapper _bxt_tas_script_generation("_bxt_tas_script_generation", "1337");
	CVarWrapper bxt_taslog_filename("bxt_taslog_filename", "taslogger.log");
	CVarWrapper bxt_autopause("bxt_autopause", "0");
	CVarWrapper bxt_interprocess_enable("bxt_interprocess_enable", "0");
	CVarWrapper bxt_shake_remove("bxt_shake_remove", "0");
	CVarWrapper bxt_fade_remove("bxt_fade_remove", "0");
	CVarWrapper bxt_skybox_remove("bxt_skybox_remove", "0");
	CVarWrapper bxt_water_remove("bxt_water_remove", "0");
	CVarWrapper bxt_stop_demo_on_changelevel("bxt_stop_demo_on_changelevel", "0");
	CVarWrapper bxt_tas_editor_simulate_for_ms("bxt_tas_editor_simulate_for_ms", "40");
	CVarWrapper bxt_tas_editor_camera_editor("bxt_tas_editor_camera_editor", "0");
	CVarWrapper bxt_tas_editor_apply_smoothing_over_s("bxt_tas_editor_apply_smoothing_over_s", "0.15");
	CVarWrapper _bxt_tas_editor_apply_smoothing_high_weight_duration("_bxt_tas_editor_apply_smoothing_high_weight_duration", "0.03");
	CVarWrapper _bxt_tas_editor_apply_smoothing_high_weight_multiplier("_bxt_tas_editor_apply_smoothing_high_weight_multiplier", "3");
	CVarWrapper bxt_tas_norefresh_until_last_frames("bxt_tas_norefresh_until_last_frames", "0");
	CVarWrapper bxt_tas_write_log("bxt_tas_write_log", "0");
	CVarWrapper bxt_tas_playback_speed("bxt_tas_playback_speed", "1");
	CVarWrapper bxt_disable_vgui("bxt_disable_vgui", "0");
	CVarWrapper bxt_show_only_viewmodel_and_player("bxt_show_only_viewmodel_and_player", "0");
	CVarWrapper bxt_force_zmax("bxt_force_zmax", "0");
	CVarWrapper bxt_viewmodel_fov("bxt_viewmodel_fov", "0");
	CVarWrapper bxt_viewmodel_disable_idle("bxt_viewmodel_disable_idle", "0");
	CVarWrapper bxt_viewmodel_disable_equip("bxt_viewmodel_disable_equip", "0");
	CVarWrapper bxt_viewmodel_semitransparent("bxt_viewmodel_semitransparent", "0");
	CVarWrapper bxt_clear_color("bxt_clear_color", "");
	CVarWrapper bxt_force_fov("bxt_force_fov", "0");
	CVarWrapper bxt_force_clear("bxt_force_clear", "0");
	CVarWrapper bxt_fix_mouse_horizontal_limit("bxt_fix_mouse_horizontal_limit", "0");
	CVarWrapper bxt_hud_game_color("bxt_hud_game_color", "");
	CVarWrapper bxt_disable_gamedir_check_in_demo("bxt_disable_gamedir_check_in_demo", "0");
	CVarWrapper bxt_remove_fps_limit("bxt_remove_fps_limit", "0");
	CVarWrapper bxt_cof_disable_save_lock("bxt_cof_disable_save_lock", "0");

	CVarWrapper con_color;
	CVarWrapper sv_cheats;
	CVarWrapper sv_maxvelocity;
	CVarWrapper sv_maxspeed;
	CVarWrapper sv_stopspeed;
	CVarWrapper sv_friction;
	CVarWrapper edgefriction;
	CVarWrapper sv_accelerate;
	CVarWrapper sv_airaccelerate;
	CVarWrapper sv_gravity;
	CVarWrapper sv_stepsize;
	CVarWrapper sv_bounce;
	CVarWrapper fps_max;
	CVarWrapper default_fov;
	CVarWrapper skill;

	// Serverside CVars
	CVarWrapper bxt_fire_on_stuck("bxt_fire_on_stuck", "");
	CVarWrapper bxt_autojump("bxt_autojump", "0");
	CVarWrapper bxt_autojump_priority("bxt_autojump_priority", "1");
	CVarWrapper bxt_bhopcap("bxt_bhopcap", "1");
	CVarWrapper bxt_timer_autostop("bxt_timer_autostop", "1");
	CVarWrapper bxt_show_hidden_entities("bxt_show_hidden_entities", "0");
	CVarWrapper bxt_show_hidden_entities_clientside("bxt_show_hidden_entities_clientside", "0");
	CVarWrapper bxt_show_triggers_legacy("bxt_show_triggers_legacy", "0");
	CVarWrapper bxt_show_pickup_bbox("bxt_show_pickup_bbox", "0");
	CVarWrapper bxt_disable_autosave("bxt_disable_autosave", "0");
	CVarWrapper bxt_disable_changelevel("bxt_disable_changelevel", "0");
	CVarWrapper bxt_force_duck("bxt_force_duck", "0");
	CVarWrapper bxt_force_jumpless("bxt_force_jumpless", "0");
	CVarWrapper bxt_render_far_entities("bxt_render_far_entities", "0");
	CVarWrapper bxt_fire_on_mm_targetname("bxt_fire_on_mm_targetname", "");
	CVarWrapper bxt_fire_on_mm_command("bxt_fire_on_mm_command", "");
	CVarWrapper bxt_fire_on_button_target("bxt_fire_on_button_target", "");
	CVarWrapper bxt_fire_on_button_command("bxt_fire_on_button_command", "");
	CVarWrapper bxt_remove_stamina("bxt_remove_stamina", "0");
	CVarWrapper bxt_cof_enable_ducktap("bxt_cof_enable_ducktap", "0");
	CVarWrapper bxt_cof_slowdown_if_use_on_ground("bxt_cof_slowdown_if_use_on_ground", "0");

	// Clientside CVars
	CVarWrapper cl_righthand("cl_righthand", "0");
	CVarWrapper bxt_remove_punchangles("bxt_remove_punchangles", "0");
	CVarWrapper bxt_hide_other_players("bxt_hide_other_players", "0");
	CVarWrapper bxt_disable_hud("bxt_disable_hud", "0");
	CVarWrapper bxt_disable_nightvision_sprite("bxt_disable_nightvision_sprite", "0");
	CVarWrapper bxt_autojump_prediction("bxt_autojump_prediction", "0");
	CVarWrapper bxt_bhopcap_prediction("bxt_bhopcap_prediction", "1");
	CVarWrapper bxt_show_nodes("bxt_show_nodes", "0");
	CVarWrapper bxt_show_displacer_earth_targets("bxt_show_displacer_earth_targets", "0");
	CVarWrapper bxt_show_custom_triggers("bxt_show_custom_triggers", "1");
	CVarWrapper bxt_triggers_color("bxt_triggers_color", "");
	CVarWrapper bxt_wallhack("bxt_wallhack", "0");
	CVarWrapper bxt_wallhack_additive("bxt_wallhack_additive", "0");
	CVarWrapper bxt_wallhack_alpha("bxt_wallhack_alpha", "0.6");
	CVarWrapper bxt_novis("bxt_novis", "0");
	CVarWrapper bxt_show_triggers("bxt_show_triggers", "0");
	CVarWrapper bxt_collision_depth_map("bxt_collision_depth_map", "0");
	CVarWrapper bxt_collision_depth_map_colors("bxt_collision_depth_map_colors", "0");
	CVarWrapper bxt_collision_depth_map_hull("bxt_collision_depth_map_hull", "2");
	CVarWrapper bxt_collision_depth_map_max_depth("bxt_collision_depth_map_max_depth", "1000");
	CVarWrapper bxt_collision_depth_map_pixel_scale("bxt_collision_depth_map_pixel_scale", "8");
	CVarWrapper bxt_collision_depth_map_remove_distance_limit("bxt_collision_depth_map_remove_distance_limit", "1");
	CVarWrapper bxt_unlock_camera_during_pause("bxt_unlock_camera_during_pause", "0");
	CVarWrapper bxt_hud("bxt_hud", "1");
	CVarWrapper bxt_hud_color("bxt_hud_color", "");
	CVarWrapper bxt_hud_game_alpha_max_clientside("bxt_hud_game_alpha_max_clientside", "0");
	CVarWrapper bxt_hud_precision("bxt_hud_precision", "6");
	CVarWrapper bxt_hud_quickgauss("bxt_hud_quickgauss", "0");
	CVarWrapper bxt_hud_quickgauss_offset("bxt_hud_quickgauss_offset", "");
	CVarWrapper bxt_hud_quickgauss_anchor("bxt_hud_quickgauss_anchor", "0.5 0");
	CVarWrapper bxt_hud_velocity("bxt_hud_velocity", "0");
	CVarWrapper bxt_hud_velocity_offset("bxt_hud_velocity_offset", "");
	CVarWrapper bxt_hud_velocity_anchor("bxt_hud_velocity_anchor", "1 0");
	CVarWrapper bxt_hud_origin("bxt_hud_origin", "0");
	CVarWrapper bxt_hud_origin_offset("bxt_hud_origin_offset", "");
	CVarWrapper bxt_hud_origin_anchor("bxt_hud_origin_anchor", "1 0");
	CVarWrapper bxt_hud_viewangles("bxt_hud_viewangles", "0");
	CVarWrapper bxt_hud_viewangles_offset("bxt_hud_viewangles_offset", "");
	CVarWrapper bxt_hud_viewangles_anchor("bxt_hud_viewangles_anchor", "1 0");
	CVarWrapper bxt_hud_distance("bxt_hud_distance", "0");
	CVarWrapper bxt_hud_distance_offset("bxt_hud_distance_offset", "");
	CVarWrapper bxt_hud_distance_anchor("bxt_hud_distance_anchor", "1 0");
	CVarWrapper bxt_hud_entity_info("bxt_hud_entity_info", "0");
	CVarWrapper bxt_hud_entity_info_offset("bxt_hud_entity_info_offset", "");
	CVarWrapper bxt_hud_entity_info_anchor("bxt_hud_entity_info_anchor", "1 0");
	CVarWrapper bxt_hud_selfgauss("bxt_hud_selfgauss", "0");
	CVarWrapper bxt_hud_selfgauss_offset("bxt_hud_selfgauss_offset", "");
	CVarWrapper bxt_hud_selfgauss_anchor("bxt_hud_selfgauss_anchor", "1 0");
	CVarWrapper bxt_hud_armor("bxt_hud_armor", "0");
	CVarWrapper bxt_hud_armor_offset("bxt_hud_armor_offset", "");
	CVarWrapper bxt_hud_armor_anchor("bxt_hud_armor_anchor", "1 0");
	CVarWrapper bxt_hud_waterlevel("bxt_hud_waterlevel", "0");
	CVarWrapper bxt_hud_waterlevel_offset("bxt_hud_waterlevel_offset", "");
	CVarWrapper bxt_hud_waterlevel_anchor("bxt_hud_waterlevel_anchor", "1 0");
	CVarWrapper bxt_hud_speedometer("bxt_hud_speedometer", "1");
	CVarWrapper bxt_hud_speedometer_offset("bxt_hud_speedometer_offset", "");
	CVarWrapper bxt_hud_speedometer_anchor("bxt_hud_speedometer_anchor", "0.5 1");
	CVarWrapper bxt_hud_jumpspeed("bxt_hud_jumpspeed", "0");
	CVarWrapper bxt_hud_jumpspeed_offset("bxt_hud_jumpspeed_offset", "");
	CVarWrapper bxt_hud_jumpspeed_anchor("bxt_hud_jumpspeed_anchor", "0.5 1");
	CVarWrapper bxt_hud_jumpdistance("bxt_hud_jumpdistance", "0");
	CVarWrapper bxt_hud_jumpdistance_offset("bxt_hud_jumpdistance_offset", "");
	CVarWrapper bxt_hud_jumpdistance_anchor("bxt_hud_jumpdistance_anchor", "0.5 1");
	CVarWrapper bxt_hud_health("bxt_hud_health", "0");
	CVarWrapper bxt_hud_health_offset("bxt_hud_health_offset", "");
	CVarWrapper bxt_hud_health_anchor("bxt_hud_health_anchor", "0.5 1");
	CVarWrapper bxt_hud_health_override_in_demo("bxt_hud_health_override_in_demo", "0");
	CVarWrapper bxt_hud_nihilanth("bxt_hud_nihilanth", "0");
	CVarWrapper bxt_hud_nihilanth_offset("bxt_hud_nihilanth_offset", "");
	CVarWrapper bxt_hud_nihilanth_anchor("bxt_hud_nihilanth_anchor", "1 0");
	CVarWrapper bxt_hud_gonarch("bxt_hud_gonarch", "0");
	CVarWrapper bxt_hud_gonarch_offset("bxt_hud_gonarch_offset", "");
	CVarWrapper bxt_hud_gonarch_anchor("bxt_hud_gonarch_anchor", "1 0");
	CVarWrapper bxt_hud_timer("bxt_hud_timer", "0");
	CVarWrapper bxt_hud_timer_offset("bxt_hud_timer_offset", "");
	CVarWrapper bxt_hud_timer_anchor("bxt_hud_timer_anchor", "0 0.5");
	CVarWrapper bxt_hud_visible_landmarks("bxt_hud_visible_landmarks", "0");
	CVarWrapper bxt_hud_visible_landmarks_offset("bxt_hud_visible_landmarks_offset", "");
	CVarWrapper bxt_hud_visible_landmarks_anchor("bxt_hud_visible_landmarks_anchor", "0.5 0");
	CVarWrapper bxt_hud_incorrect_fps_indicator("bxt_hud_incorrect_fps_indicator", "1");
	CVarWrapper bxt_hud_useables("bxt_hud_useables", "0");
	CVarWrapper bxt_hud_useables_radius("bxt_hud_useables_radius", "64");
	CVarWrapper bxt_hud_tas_editor_status("bxt_hud_tas_editor_status", "0");
	CVarWrapper bxt_hud_tas_editor_status_offset("bxt_hud_tas_editor_status_offset", "");
	CVarWrapper bxt_hud_tas_editor_status_anchor("bxt_hud_tas_editor_status_anchor", "0 0");
	CVarWrapper bxt_hud_entities("bxt_hud_entities", "0");
	CVarWrapper bxt_hud_entities_offset("bxt_hud_entities_offset", "");
	CVarWrapper bxt_hud_entities_anchor("bxt_hud_entities_anchor", "0 0");
	CVarWrapper bxt_hud_stamina("bxt_hud_stamina", "0");
	CVarWrapper bxt_hud_stamina_offset("bxt_hud_stamina_offset", "");
	CVarWrapper bxt_hud_stamina_anchor("bxt_hud_stamina_anchor", "0.5 0");
	CVarWrapper bxt_cross("bxt_cross", "0");
	CVarWrapper bxt_cross_color("bxt_cross_color", "");
	CVarWrapper bxt_cross_alpha("bxt_cross_alpha", "");
	CVarWrapper bxt_cross_thickness("bxt_cross_thickness", "2");
	CVarWrapper bxt_cross_size("bxt_cross_size", "10");
	CVarWrapper bxt_cross_gap("bxt_cross_gap", "3");
	CVarWrapper bxt_cross_outline("bxt_cross_outline", "0");
	CVarWrapper bxt_cross_circle_radius("bxt_cross_circle_radius", "0");
	CVarWrapper bxt_cross_dot_color("bxt_cross_dot_color", "");
	CVarWrapper bxt_cross_dot_size("bxt_cross_dot_size", "0");
	CVarWrapper bxt_cross_top_line("bxt_cross_top_line", "1");
	CVarWrapper bxt_cross_bottom_line("bxt_cross_bottom_line", "1");
	CVarWrapper bxt_cross_left_line("bxt_cross_left_line", "1");
	CVarWrapper bxt_cross_right_line("bxt_cross_right_line", "1");
	CVarWrapper bxt_viewmodel_ofs_forward("bxt_viewmodel_ofs_forward", "0");
	CVarWrapper bxt_viewmodel_ofs_right("bxt_viewmodel_ofs_right", "0");
	CVarWrapper bxt_viewmodel_ofs_up("bxt_viewmodel_ofs_up", "0");
	CVarWrapper bxt_viewmodel_bob_angled("bxt_viewmodel_bob_angled", "0");
	CVarWrapper bxt_show_bullets("bxt_show_bullets", "0");
	CVarWrapper bxt_show_bullets_enemy("bxt_show_bullets_enemy", "0");
	CVarWrapper bxt_anglespeed_cap("bxt_anglespeed_cap", "1");
	CVarWrapper bxt_speed_scaling("bxt_speed_scaling", "1");

	const std::vector<CVarWrapper*> allCVars =
	{
		&_bxt_bunnysplit_time_update_frequency,
		&_bxt_taslog,
		&_bxt_min_frametime,
		&_bxt_norefresh,
		&_bxt_save_runtime_data_in_demos,
		&_bxt_tas_script_generation,
		&bxt_taslog_filename,
		&bxt_autopause,
		&bxt_anglespeed_cap,
		&bxt_speed_scaling,
		&bxt_remove_stamina,
		&bxt_interprocess_enable,
		&bxt_fade_remove,
		&bxt_shake_remove,
		&bxt_skybox_remove,
		&bxt_water_remove,
		&bxt_stop_demo_on_changelevel,
		&bxt_tas_editor_apply_smoothing_over_s,
		&_bxt_tas_editor_apply_smoothing_high_weight_duration,
		&_bxt_tas_editor_apply_smoothing_high_weight_multiplier,
		&bxt_tas_editor_simulate_for_ms,
		&bxt_tas_editor_camera_editor,
		&bxt_tas_norefresh_until_last_frames,
		&bxt_tas_write_log,
		&bxt_tas_playback_speed,
		&bxt_disable_vgui,
		&bxt_show_only_viewmodel_and_player,
		&bxt_force_zmax,
		&bxt_force_duck,
		&bxt_force_jumpless,
		&bxt_viewmodel_fov,
		&bxt_viewmodel_disable_idle,
		&bxt_viewmodel_disable_equip,
		&bxt_viewmodel_semitransparent,
		&bxt_clear_color,
		&bxt_force_fov,
		&bxt_force_clear,
		&bxt_fix_mouse_horizontal_limit,
		&bxt_hud_game_color,
		&bxt_disable_gamedir_check_in_demo,
		&bxt_remove_fps_limit,
		&bxt_cof_disable_save_lock,
		&bxt_autojump_priority,
		&con_color,
		&sv_cheats,
		&sv_maxvelocity,
		&sv_maxspeed,
		&sv_stopspeed,
		&sv_friction,
		&edgefriction,
		&sv_accelerate,
		&sv_airaccelerate,
		&sv_gravity,
		&sv_stepsize,
		&sv_bounce,
		&fps_max,
		&default_fov,
		&skill,
		&bxt_fire_on_stuck,
		&bxt_autojump,
		&bxt_bhopcap,
		&bxt_timer_autostop,
		&bxt_show_hidden_entities,
		&bxt_show_hidden_entities_clientside,
		&bxt_autojump_prediction,
		&bxt_bhopcap_prediction,
		&bxt_show_nodes,
		&bxt_show_displacer_earth_targets,
		&bxt_show_pickup_bbox,
		&bxt_disable_autosave,
		&bxt_disable_changelevel,
		&bxt_disable_hud,
		&bxt_hide_other_players,
		&bxt_disable_nightvision_sprite,
		&bxt_show_custom_triggers,
		&bxt_triggers_color,
		&bxt_wallhack,
		&bxt_wallhack_additive,
		&bxt_wallhack_alpha,
		&bxt_novis,
		&bxt_render_far_entities,
		&bxt_fire_on_mm_targetname,
		&bxt_fire_on_mm_command,
		&bxt_fire_on_button_target,
		&bxt_fire_on_button_command,
		&bxt_cof_enable_ducktap,
		&bxt_cof_slowdown_if_use_on_ground,
		&bxt_show_triggers,
		&bxt_show_triggers_legacy,
		&bxt_collision_depth_map,
		&bxt_collision_depth_map_colors,
		&bxt_collision_depth_map_hull,
		&bxt_collision_depth_map_max_depth,
		&bxt_collision_depth_map_pixel_scale,
		&bxt_collision_depth_map_remove_distance_limit,
		&bxt_unlock_camera_during_pause,
		&bxt_remove_punchangles,
		&cl_righthand,
		&bxt_hud,
		&bxt_hud_color,
		&bxt_hud_game_alpha_max_clientside,
		&bxt_hud_precision,
		&bxt_hud_quickgauss,
		&bxt_hud_quickgauss_offset,
		&bxt_hud_quickgauss_anchor,
		&bxt_hud_velocity,
		&bxt_hud_velocity_offset,
		&bxt_hud_velocity_anchor,
		&bxt_hud_origin,
		&bxt_hud_origin_offset,
		&bxt_hud_origin_anchor,
		&bxt_hud_viewangles,
		&bxt_hud_viewangles_offset,
		&bxt_hud_viewangles_anchor,
		&bxt_hud_distance,
		&bxt_hud_distance_offset,
		&bxt_hud_distance_anchor,
		&bxt_hud_entity_info,
		&bxt_hud_entity_info_offset,
		&bxt_hud_entity_info_anchor,
		&bxt_hud_selfgauss,
		&bxt_hud_selfgauss_offset,
		&bxt_hud_selfgauss_anchor,
		&bxt_hud_armor,
		&bxt_hud_armor_offset,
		&bxt_hud_armor_anchor,
		&bxt_hud_waterlevel,
		&bxt_hud_waterlevel_offset,
		&bxt_hud_waterlevel_anchor,
		&bxt_hud_speedometer,
		&bxt_hud_speedometer_offset,
		&bxt_hud_speedometer_anchor,
		&bxt_hud_jumpspeed,
		&bxt_hud_jumpspeed_offset,
		&bxt_hud_jumpspeed_anchor,
		&bxt_hud_jumpdistance,
		&bxt_hud_jumpdistance_offset,
		&bxt_hud_jumpdistance_anchor,
		&bxt_hud_health,
		&bxt_hud_health_offset,
		&bxt_hud_health_anchor,
		&bxt_hud_health_override_in_demo,
		&bxt_hud_nihilanth,
		&bxt_hud_nihilanth_offset,
		&bxt_hud_nihilanth_anchor,
		&bxt_hud_gonarch,
		&bxt_hud_gonarch_offset,
		&bxt_hud_gonarch_anchor,
		&bxt_hud_timer,
		&bxt_hud_timer_offset,
		&bxt_hud_timer_anchor,
		&bxt_hud_visible_landmarks,
		&bxt_hud_visible_landmarks_offset,
		&bxt_hud_visible_landmarks_anchor,
		&bxt_hud_incorrect_fps_indicator,
		&bxt_hud_useables,
		&bxt_hud_useables_radius,
		&bxt_hud_tas_editor_status,
		&bxt_hud_tas_editor_status_offset,
		&bxt_hud_tas_editor_status_anchor,
		&bxt_hud_entities,
		&bxt_hud_entities_offset,
		&bxt_hud_entities_anchor,
		&bxt_hud_stamina,
		&bxt_hud_stamina_offset,
		&bxt_hud_stamina_anchor,
		&bxt_cross,
		&bxt_cross_color,
		&bxt_cross_alpha,
		&bxt_cross_thickness,
		&bxt_cross_size,
		&bxt_cross_gap,
		&bxt_cross_outline,
		&bxt_cross_circle_radius,
		&bxt_cross_dot_color,
		&bxt_cross_dot_size,
		&bxt_cross_top_line,
		&bxt_cross_bottom_line,
		&bxt_cross_left_line,
		&bxt_cross_right_line,
		&bxt_viewmodel_ofs_forward,
		&bxt_viewmodel_ofs_right,
		&bxt_viewmodel_ofs_up,
		&bxt_viewmodel_bob_angled,
		&bxt_show_bullets,
		&bxt_show_bullets_enemy,
	};
}
