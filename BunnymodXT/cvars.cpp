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
	CVarWrapper bxt_taslog_filename("bxt_taslog_filename", "taslogger.log");
	CVarWrapper bxt_autopause("bxt_autopause", "0");
	CVarWrapper bxt_interprocess_enable("bxt_interprocess_enable", "0");
	CVarWrapper bxt_fade_remove("bxt_fade_remove", "0");
	CVarWrapper bxt_disable_skybox("bxt_disable_skybox", "0");
	CVarWrapper bxt_stop_demo_on_changelevel("bxt_stop_demo_on_changelevel", "0");
	CVarWrapper bxt_tas_editor_simulate_for_ms("bxt_tas_editor_simulate_for_ms", "40");
	CVarWrapper bxt_tas_norefresh_until_last_frames("bxt_tas_norefresh_until_last_frames", "0");
	CVarWrapper bxt_tas_write_log("bxt_tas_write_log", "0");
	CVarWrapper bxt_tas_playback_speed("bxt_tas_playback_speed", "1");

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

	// Serverside CVars
	CVarWrapper bxt_autojump("bxt_autojump", "0");
	CVarWrapper bxt_bhopcap("bxt_bhopcap", "1");
	CVarWrapper bxt_timer_autostop("bxt_timer_autostop", "1");
	CVarWrapper bxt_show_hidden_entities("bxt_show_hidden_entities", "0");
	CVarWrapper bxt_show_triggers_legacy("bxt_show_triggers_legacy", "0");
	CVarWrapper bxt_show_pickup_bbox("bxt_show_pickup_bbox", "0");
	CVarWrapper bxt_disable_autosave("bxt_disable_autosave", "0");
	CVarWrapper bxt_disable_changelevel("bxt_disable_changelevel", "0");

	// Clientside CVars
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
	CVarWrapper bxt_hud_precision("bxt_hud_precision", "6");
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
	CVarWrapper bxt_hud_speedometer("bxt_hud_speedometer", "1");
	CVarWrapper bxt_hud_speedometer_offset("bxt_hud_speedometer_offset", "");
	CVarWrapper bxt_hud_speedometer_anchor("bxt_hud_speedometer_anchor", "0.5 1");
	CVarWrapper bxt_hud_jumpspeed("bxt_hud_jumpspeed", "0");
	CVarWrapper bxt_hud_jumpspeed_offset("bxt_hud_jumpspeed_offset", "");
	CVarWrapper bxt_hud_jumpspeed_anchor("bxt_hud_jumpspeed_anchor", "0.5 1");
	CVarWrapper bxt_hud_health("bxt_hud_health", "0");
	CVarWrapper bxt_hud_health_offset("bxt_hud_health_offset", "");
	CVarWrapper bxt_hud_health_anchor("bxt_hud_health_anchor", "0.5 1");
	CVarWrapper bxt_hud_nihilanth("bxt_hud_nihilanth", "0");
	CVarWrapper bxt_hud_nihilanth_offset("bxt_hud_nihilanth_offset", "");
	CVarWrapper bxt_hud_nihilanth_anchor("bxt_hud_nihilanth_anchor", "1 0");
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
	CVarWrapper bxt_hud_tas_editor_status_anchor("bxt_hud_tas_editor_status_anchor", "1 0");
	CVarWrapper bxt_hud_entities("bxt_hud_entities", "0");
	CVarWrapper bxt_hud_entities_offset("bxt_hud_entities_offset", "");
	CVarWrapper bxt_hud_entities_anchor("bxt_hud_entities_anchor", "0 0");

	const std::vector<CVarWrapper*> allCVars =
	{
		&_bxt_bunnysplit_time_update_frequency,
		&_bxt_taslog,
		&_bxt_min_frametime,
		&_bxt_norefresh,
		&_bxt_save_runtime_data_in_demos,
		&bxt_taslog_filename,
		&bxt_autopause,
		&bxt_interprocess_enable,
		&bxt_fade_remove,
		&bxt_disable_skybox,
		&bxt_stop_demo_on_changelevel,
		&bxt_tas_editor_simulate_for_ms,
		&bxt_tas_norefresh_until_last_frames,
		&bxt_tas_write_log,
		&bxt_tas_playback_speed,
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
		&bxt_autojump,
		&bxt_bhopcap,
		&bxt_timer_autostop,
		&bxt_show_hidden_entities,
		&bxt_autojump_prediction,
		&bxt_bhopcap_prediction,
		&bxt_show_nodes,
		&bxt_show_displacer_earth_targets,
		&bxt_show_pickup_bbox,
		&bxt_disable_autosave,
		&bxt_disable_changelevel,
		&bxt_show_custom_triggers,
		&bxt_triggers_color,
		&bxt_wallhack,
		&bxt_wallhack_additive,
		&bxt_wallhack_alpha,
		&bxt_novis,
		&bxt_show_triggers,
		&bxt_show_triggers_legacy,
		&bxt_collision_depth_map,
		&bxt_collision_depth_map_colors,
		&bxt_collision_depth_map_hull,
		&bxt_collision_depth_map_max_depth,
		&bxt_collision_depth_map_pixel_scale,
		&bxt_collision_depth_map_remove_distance_limit,
		&bxt_unlock_camera_during_pause,
		&bxt_hud,
		&bxt_hud_color,
		&bxt_hud_precision,
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
		&bxt_hud_speedometer,
		&bxt_hud_speedometer_offset,
		&bxt_hud_speedometer_anchor,
		&bxt_hud_jumpspeed,
		&bxt_hud_jumpspeed_offset,
		&bxt_hud_jumpspeed_anchor,
		&bxt_hud_health,
		&bxt_hud_health_offset,
		&bxt_hud_health_anchor,
		&bxt_hud_nihilanth,
		&bxt_hud_nihilanth_offset,
		&bxt_hud_nihilanth_anchor,
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
		&bxt_hud_entities_anchor
	};
}
