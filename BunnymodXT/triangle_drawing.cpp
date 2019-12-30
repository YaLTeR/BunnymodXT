#include "stdafx.hpp"

#include <SDL2/SDL.h>
#include "custom_triggers.hpp"
#include "triangle_drawing.hpp"
#include "triangle_utils.hpp"
#include "modules.hpp"

namespace TriangleDrawing
{
	static HSPRITE_HL white_sprite = 0;

	static void DrawNodes(triangleapi_s *pTriAPI)
	{
		if (!CVars::bxt_show_nodes.GetBool())
			return;

		pTriAPI->RenderMode(kRenderTransAdd);
		pTriAPI->CullFace(TRI_NONE);
		pTriAPI->Color4f(0.722f, 0.0f, 0.341f, 1.0f);
		for (const Vector *position : ServerDLL::GetInstance().GetNodePositions()) {
			TriangleUtils::DrawPyramid(pTriAPI, *position, 10, 30);
		}
	}

	// From util.cpp of HLSDK.
	static Vector UTIL_ClampVectorToBox(const Vector &input, const Vector &clampSize)
	{
		Vector sourceVector = input;

		if (sourceVector.x > clampSize.x)
			sourceVector.x -= clampSize.x;
		else if (sourceVector.x < -clampSize.x)
			sourceVector.x += clampSize.x;
		else
			sourceVector.x = 0;

		if (sourceVector.y > clampSize.y)
			sourceVector.y -= clampSize.y;
		else if (sourceVector.y < -clampSize.y)
			sourceVector.y += clampSize.y;
		else
			sourceVector.y = 0;

		if (sourceVector.z > clampSize.z)
			sourceVector.z -= clampSize.z;
		else if (sourceVector.z < -clampSize.z)
			sourceVector.z += clampSize.z;
		else
			sourceVector.z = 0;

		return sourceVector.Normalize();
	}

	static void DrawUseableEntities(triangleapi_s *pTriAPI)
	{
		if (!CVars::bxt_hud_useables.GetBool())
			return;

		const auto searchRadius = CVars::bxt_hud_useables_radius.GetFloat();
		const auto player = HwDLL::GetInstance().GetPlayerEdict();
		const auto playerOrigin = player->v.origin;
		Vector forward, right, up;
		ClientDLL::GetInstance().pEngfuncs->pfnAngleVectors(player->v.v_angle, forward, right, up);

		const auto si = CustomHud::GetScreenInfo();
		const auto min_resolution = std::min(si.iHeight, si.iWidth);
		const auto half_size_pixels = min_resolution / 30.0f;
		const Vector2D half_size(TriangleUtils::PixelWidthToProportion(half_size_pixels), TriangleUtils::PixelHeightToProportion(half_size_pixels));

		constexpr float VIEW_FIELD_NARROW = 0.7f;
		float max_dot = VIEW_FIELD_NARROW;
		const edict_t* target_object = nullptr;

		pTriAPI->RenderMode(kRenderTransColor);
		pTriAPI->CullFace(TRI_NONE);

		const auto useable_entities = ServerDLL::GetInstance().GetUseableEntities(playerOrigin, searchRadius);
		for (const auto pent : useable_entities) {
			auto bmodelOrigin = pent->v.absmin + 0.5 * pent->v.size;
			const auto disp = bmodelOrigin - playerOrigin - player->v.view_ofs;

			float dot = DotProduct(UTIL_ClampVectorToBox(disp, pent->v.size * 0.5), forward);
			if (dot > max_dot) {
				target_object = pent;
				max_dot = dot;
			}
		}

		pTriAPI->Color4f(1.0f, 0.0f, 0.0f, 1.0f);

		for (const auto pent : useable_entities) {
			if (pent == target_object)
				continue;

			auto bmodelOrigin = pent->v.absmin + 0.5 * pent->v.size;
			const auto disp = bmodelOrigin - playerOrigin - player->v.view_ofs;

			// Prevent drawing entities that are behind us. WorldToScreen doesn't prevent this automatically.
			if (DotProduct(forward, disp) > 0.0f) {
				Vector screen_point;
				pTriAPI->WorldToScreen(bmodelOrigin, screen_point);

				TriangleUtils::DrawScreenRectangle(
					pTriAPI,
					screen_point.Make2D() - half_size,
					screen_point.Make2D() + half_size
				);
			}
		}

		// Make sure the target object is drawn on top.
		if (target_object)
		{
			pTriAPI->Color4f(0.0f, 1.0f, 0.0f, 1.0f);

			auto bmodelOrigin = target_object->v.absmin + 0.5 * target_object->v.size;
			const auto disp = bmodelOrigin - playerOrigin - player->v.view_ofs;

			// Prevent drawing entities that are behind us. WorldToScreen doesn't prevent this automatically.
			if (DotProduct(forward, disp) > 0.0f) {
				Vector screen_point;
				pTriAPI->WorldToScreen(bmodelOrigin, screen_point);

				TriangleUtils::DrawScreenRectangle(
					pTriAPI,
					screen_point.Make2D() - half_size,
					screen_point.Make2D() + half_size
				);
			}
		}
	}

	static float GetPulsatingAlpha(float a, float time)
	{
		constexpr float speed = 8.0f;
		float s = std::sin(speed * time);
		s *= 0.05f;
		s += a;
		s = std::max(s, 0.0f);
		s = std::min(s, 1.0f);
		return s;
	}

	static void DrawTriggers(triangleapi_s *pTriAPI)
	{
		if (!CVars::bxt_show_triggers.GetBool() || CVars::bxt_show_triggers_legacy.GetBool())
			return;

		pTriAPI->RenderMode(kRenderTransAdd);
		pTriAPI->CullFace(TRI_NONE);

		const float svTime = static_cast<float>(HwDLL::GetInstance().GetTime());
		edict_t *edicts;
		const int numEdicts = HwDLL::GetInstance().GetEdicts(&edicts);
		for (int e = 1; e < numEdicts; ++e) {
			const edict_t *ent = edicts + e;
			if (!HwDLL::GetInstance().IsValidEdict(ent))
				continue;

			const char *classname = ServerDLL::GetInstance().GetString(ent->v.classname);
			if (std::strncmp(classname, "trigger_", 8) != 0)
				continue;

			const model_t *model = HwDLL::GetInstance().GetModelByIndex(ent->v.modelindex);
			if (!model)
				continue;

			const bool active = ent->v.solid != SOLID_NOT || std::strcmp(classname, "trigger_transition") == 0;
			const msurface_t *surfs = model->surfaces + model->firstmodelsurface;
			for (int i = 0; i < model->nummodelsurfaces; ++i) {
				// Offset to make each surface look slightly different
				const float offset = i * float(M_PI) / 7;
				float r, g, b, a;
				ServerDLL::GetTriggerColor(classname, !active, true, r, g, b, a);
				r /= 255.0f;
				g /= 255.0f;
				b /= 255.0f;
				a /= 255.0f;
				if (active)
					a = GetPulsatingAlpha(a, svTime + offset);

				pTriAPI->Color4f(r, g, b, a);
				pTriAPI->Begin(TRI_POLYGON);
				for (int j = 0; j < surfs[i].polys->numverts; ++j)
					pTriAPI->Vertex3fv(surfs[i].polys->verts[j]);
				pTriAPI->End();
			}
		}
	}

	static void DrawCustomTriggers(triangleapi_s *pTriAPI)
	{
		if (!CVars::bxt_show_custom_triggers.GetBool())
			return;

		pTriAPI->CullFace(TRI_NONE);

		for (const auto& trigger : CustomTriggers::triggers) {
			auto corner_positions = trigger.get_corner_positions();

			pTriAPI->RenderMode(kRenderTransAdd);
			pTriAPI->Color4f(1.0f, 0.5f, 0.0f, 0.3f);
			TriangleUtils::DrawAACuboid(pTriAPI, corner_positions.first, corner_positions.second);

			pTriAPI->RenderMode(kRenderTransColor);
			pTriAPI->Color4f(0.5f, 0.3f, 0.0f, 1.0f);
			TriangleUtils::DrawAACuboidWireframe(pTriAPI, corner_positions.first, corner_positions.second);
		}
	}

	static void DrawStrafeEditor(triangleapi_s *pTriAPI)
	{
		using HLStrafe::HullType;
		using HLTAS::StrafeDir;
		using HLTAS::StrafeType;
		using std::vector;
		const double M_RAD2DEG = 180 / M_PI;

		auto& hw = HwDLL::GetInstance();
		if (hw.edit_strafe_mode == EditStrafeMode::DISABLED)
			return;
		auto& cl = ClientDLL::GetInstance();

		int x, y;
		auto mouse_state = SDL_GetMouseState(&x, &y);
		Vector2D mouse(x, y);

		// Convert from ScreenToWorld coordinates to SDL_GetMouseState coordinates.
		auto stw_to_pixels = [](Vector2D v) {
			return Vector2D(
				TriangleUtils::ProportionToPixelWidth((v.x + 1) / 2),
				TriangleUtils::ProportionToPixelHeight(-(v.y - 1) / 2)
			);
		};

		static bool left_was_pressed = false;
		auto left_pressed = (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
		bool left_got_pressed = false;
		if (left_pressed && !left_was_pressed)
			left_got_pressed = true;
		left_was_pressed = left_pressed;

		static bool right_was_pressed = false;
		auto right_pressed = (mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
		bool right_got_pressed = false;
		if (right_pressed && !right_was_pressed)
			right_got_pressed = true;
		right_was_pressed = right_pressed;

		auto view = cl.last_vieworg;
		Vector forward, right, up;
		cl.pEngfuncs->pfnAngleVectors(cl.last_viewangles, forward, right, up);

		// Trace to find the world point we're interested in.
		Vector end = view + forward * 8192;
		auto tr = hw.PlayerTrace(view, end, HullType::POINT);
		Vector mouse_world(tr.EndPos);

		auto& input = hw.edit_strafe_input;
		const auto& positions = input.positions;
		const auto& fractions = input.fractions;
		const auto& normalzs = input.normalzs;
		const auto& frame_bulk_starts = input.frame_bulk_starts;

		input.simulate();

		if (hw.edit_strafe_mode == EditStrafeMode::APPEND) {
			auto& last_frame_bulk = input.frame_bulks[input.frame_bulks.size() - 1];
			size_t last_frame_bulk_start = input.frame_bulk_starts[input.frame_bulk_starts.size() - 2];
			auto last_frame_bulk_origin = positions[last_frame_bulk_start];
			auto dir = mouse_world - last_frame_bulk_origin;
			float yaw = atan2(dir.y, dir.x) * M_RAD2DEG;

			// Strafe towards the yaw.
			last_frame_bulk.SetYaw(yaw);
			input.simulate();

			// Draw the positions.
			pTriAPI->RenderMode(kRenderTransColor);
			pTriAPI->Color4f(0, 1, 0, 1);
			TriangleUtils::DrawPyramid(pTriAPI, mouse_world, 10, 20);

			size_t frame_limit = positions.size() - 1;
			auto distance_from_mouse = (mouse_world - last_frame_bulk_origin).Length2D();
			size_t frames_until_mouse = frame_limit;
			size_t frames_until_non_ground_collision = frame_limit;
			size_t next_frame_bulk_start_index = 1;

			pTriAPI->Color4f(0.8, 0.8, 0.8, 1);
			for (size_t frame = 1; frame < positions.size(); ++frame) {
				const auto& origin = positions[frame];

				if (frame > last_frame_bulk_start) {
					auto new_distance_from_mouse = (mouse_world - origin).Length2D();
					if (frames_until_mouse == frame_limit && new_distance_from_mouse > distance_from_mouse)
						frames_until_mouse = frame;
					distance_from_mouse = new_distance_from_mouse;

					// If we bumped into something along the way
					if (frames_until_non_ground_collision == frame_limit && fractions[frame] != 1) {
						auto n = normalzs[frame];
						// And it wasn't a ground or a ceiling
						if (n < 0.7 && n != -1)
							frames_until_non_ground_collision = frame;
					}

					if (frame > frames_until_non_ground_collision) {
						if (frame > frames_until_mouse)
							pTriAPI->Color4f(1, 0, 0, 1);
						else
							pTriAPI->Color4f(1, 0.5, 0, 1);
					} else {
						if (frame > frames_until_mouse)
							pTriAPI->Color4f(0, 1, 1, 1);
						else
							pTriAPI->Color4f(0, 1, 0, 1);
					}
				}

				TriangleUtils::DrawLine(pTriAPI, positions[frame - 1], positions[frame]);

				// Draw a small perpendicular line between frame bulks.
				if (frame == frame_bulk_starts[next_frame_bulk_start_index]) {
					if (next_frame_bulk_start_index + 1 != frame_bulk_starts.size())
						++next_frame_bulk_start_index;

					auto line = (positions[frame] - positions[frame - 1]).Normalize();

					Vector perpendicular;
					if (line.x == 0 && line.y == 0)
						perpendicular = Vector(1, 0, 0);
					else if (line.x == 0)
						perpendicular = Vector(1, 0, 0);
					else if (line.y == 0)
						perpendicular = Vector(0, 1, 0);
					else
						perpendicular = Vector(1, -line.x / line.y, 0).Normalize();

					perpendicular *= 5;
					Vector a = positions[frame] + perpendicular, b = positions[frame] - perpendicular;
					TriangleUtils::DrawLine(pTriAPI, a, b);
				}
			}

			if (left_got_pressed) {
				hw.SetEditStrafe(EditStrafeMode::DISABLED);
				hw.SetFreeCam(false);

				last_frame_bulk.SetRepeats(frames_until_mouse - last_frame_bulk_start);
				input.save();
			} else if (right_got_pressed) {
				auto new_frame_bulk = last_frame_bulk;
				last_frame_bulk.SetRepeats(frames_until_mouse - last_frame_bulk_start);
				input.frame_bulks.push_back(new_frame_bulk);
			}
		} else {
			// EditStrafeMode::EDIT
			if (input.frame_bulks.size() == 0)
				return;

			size_t next_frame_bulk_start_index = 1;

			Vector2D closest_edge_px;
			float closest_edge_px_dist;
			size_t closest_edge_frame = 0;

			pTriAPI->Color4f(0.8, 0.8, 0.8, 1);
			for (size_t frame = 1; frame < positions.size(); ++frame) {
				const auto& origin = positions[frame];

				TriangleUtils::DrawLine(pTriAPI, positions[frame - 1], positions[frame]);

				if (frame == frame_bulk_starts[next_frame_bulk_start_index]) {
					if (next_frame_bulk_start_index + 1 != frame_bulk_starts.size())
						++next_frame_bulk_start_index;

					auto disp = positions[frame] - view;
					if (DotProduct(forward, disp) > 0) {
						Vector origin = positions[frame];
						Vector screen_point;
						pTriAPI->WorldToScreen(origin, screen_point);
						auto screen_point_px = stw_to_pixels(screen_point.Make2D());
						auto dist = (screen_point_px - mouse).Length();

						if (closest_edge_frame == 0 || dist < closest_edge_px_dist) {
							closest_edge_frame = frame;
							closest_edge_px_dist = dist;
							closest_edge_px = screen_point_px;
						}
					}
				}
			}

			for (size_t i = 1; i < frame_bulk_starts.size(); ++i) {
				auto frame = frame_bulk_starts[i];

				auto line = (positions[frame] - positions[frame - 1]).Normalize();

				Vector perpendicular;
				if (line.x == 0 && line.y == 0)
					perpendicular = Vector(1, 0, 0);
				else if (line.x == 0)
					perpendicular = Vector(1, 0, 0);
				else if (line.y == 0)
					perpendicular = Vector(0, 1, 0);
				else
					perpendicular = Vector(1, -line.x / line.y, 0).Normalize();

				perpendicular *= 5;
				Vector a = positions[frame] + perpendicular, b = positions[frame] - perpendicular;

				if (frame == closest_edge_frame)
					pTriAPI->Color4f(1, 1, 1, 1);
				else
					pTriAPI->Color4f(0.8, 0.8, 0.8, 1);

				TriangleUtils::DrawLine(pTriAPI, a, b);
			}
		}
	}

	void VidInit()
	{
		white_sprite = ClientDLL::GetInstance().pEngfuncs->pfnSPR_Load("sprites/white.spr");
	}

	void Draw()
	{
		auto pEngfuncs = ClientDLL::GetInstance().pEngfuncs;
		auto pTriAPI = pEngfuncs->pTriAPI;

		if (white_sprite == 0)
			return;

		if (!pTriAPI->SpriteTexture(const_cast<model_s*>(pEngfuncs->GetSpritePointer(white_sprite)), 0))
			return;

		DrawNodes(pTriAPI);
		DrawUseableEntities(pTriAPI);
		DrawTriggers(pTriAPI);
		DrawCustomTriggers(pTriAPI);
		DrawStrafeEditor(pTriAPI);
	}
}
