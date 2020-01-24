// This stuff is defined in the bxt-rs library.
extern "C" {
	void rs_tri_draw(const triangleapi_s *tri_api);
	void rs_init_graph(const void *world_graph);
	void rs_server_clear();
}
