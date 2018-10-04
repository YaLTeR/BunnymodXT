#pragma once

#include "stdafx.hpp"

namespace DrawHulls
{
	struct hull_model_t;

	class HullInfo
	{
	public:
		inline void reset_worldmod(model_s *worldmod)
		{
			clean();
			current_worldmod = worldmod;
		}

		inline void set_in_draw_brush(bool in)
		{
			in_draw_brush = in;
		}

		void lazy_init_hull(model_t *(*Mod_FindName)(int, const char *), int hullnum);
		void clean();

		void draw_world_hull(float alpha) const;
		void draw_model_hull(const model_s *mod, float alpha) const;

	private:
		hull_model_t *hull_models = nullptr;
		int num_hull_models = 0;
		model_s *current_worldmod = nullptr;
		int current_hullnum = 0;

		bool in_draw_brush = false;
	};
}
