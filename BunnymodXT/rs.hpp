// This stuff is defined in the bxt-rs library.
extern "C" {
	void rs_tri_draw(const triangleapi_s *tri_api);
	void rs_init_graph(const void *world_graph);
	void rs_server_clear();
	void rs_init_cvar(const char *name, const cvar_t *ptr);
	void rs_create_tas_editor(
		const char *path,
		unsigned first_line,
		HLStrafe::PlayerData player,
		HLStrafe::CurrentState state,
		HLStrafe::MovementVars movement_vars,
		unsigned hlstrafe_version,
		unsigned mode
	);
	void rs_drop_tas_editor();
}
