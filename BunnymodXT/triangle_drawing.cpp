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
		static bool left_was_pressed = false;

		auto& hw = HwDLL::GetInstance();
		if (!hw.edit_strafe_active)
			return;
		auto& cl = ClientDLL::GetInstance();

		int x, y;
		auto mouse_state = SDL_GetMouseState(&x, &y);
		Vector2D mouse(x, y);
		auto left_pressed = (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
		bool left_got_pressed = false;
		if (left_pressed && !left_was_pressed)
			left_got_pressed = true;
		left_was_pressed = left_pressed;

		// Convert from SDL_GetMouseState coordinates to ScreenToWorld coordinates.
		auto convert = [](Vector2D v) {
			return Vector2D(
				TriangleUtils::PixelWidthToProportion(v.x) * 2 - 1,
				-TriangleUtils::PixelHeightToProportion(v.y) * 2 + 1
			);
		};

		auto player_edict = hw.GetPlayerEdict();
		auto origin = player_edict->v.origin;
		auto view = cl.last_vieworg;

		// Get the world point corresponding to where the mouse is on screen.
		auto mouse_stw = convert(mouse);
		auto mouse_stw_3d = Vector(mouse_stw.x, mouse_stw.y, 0);
		Vector mouse_world_close;
		// ScreenToWorld doesn't trace, so its point is very close in front of the player.
		pTriAPI->ScreenToWorld(mouse_stw_3d, mouse_world_close);

		// Trace to find the world point we're interested in.
		Vector mouse_dir = (mouse_world_close - view).Normalize();
		Vector end = view + mouse_dir * 8192;
		auto tr = hw.PlayerTrace(view, end, HullType::POINT);
		Vector mouse_world(tr.EndPos);

		auto dir = mouse_world - origin;
		float yaw = atan2(dir.y, dir.x) * M_RAD2DEG;

		// Strafe towards the yaw.
		const auto movement_vars = hw.GetMovementVars();

		auto frametime = movement_vars.Frametime;
		auto frame_bulk = HLTAS::Frame();
		auto frame_count = hw.input.GetFrames().size();
		if (frame_count > 0) {
			frame_bulk = hw.input.GetFrames()[frame_count - 1];
			frame_bulk.Commands.clear();
			frame_bulk.SetRepeats(100);
			frame_bulk.SetDir(StrafeDir::YAW);
			frame_bulk.SetYaw(yaw);
			frametime = std::strtof(frame_bulk.Frametime.c_str(), nullptr);
		} else {
			frame_bulk.Strafe = true;
			frame_bulk.SetDir(StrafeDir::YAW);
			frame_bulk.SetType(StrafeType::MAXACCEL);
			frame_bulk.SetYaw(yaw);
		}

		vector<Vector> positions;
		positions.push_back(origin);

		auto strafe_state = hw.StrafeState;
		strafe_state.Jump = hw.currentKeys.Jump.IsDown();
		strafe_state.Duck = hw.currentKeys.Duck.IsDown();

		auto player = hw.GetPlayerData();
		auto distance_from_mouse = (mouse_world - origin).Length2D();

		hw.StartTracing();

		size_t frame;
		size_t frame_limit = 5 / frametime;
		size_t frames_until_mouse = frame_limit;
		size_t frames_until_non_ground_collision = frame_limit;
		for (frame = 0; frame < frame_limit; ++frame)
		{
			auto processed_frame = HLStrafe::MainFunc(
				player,
				movement_vars,
				frame_bulk,
				strafe_state,
				hw.Buttons,
				hw.ButtonsPresent,
				std::bind(
					&HwDLL::UnsafePlayerTrace,
					&hw,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3
				)
			);

			player = processed_frame.NewPlayerData;
			origin = player.Origin;

			auto new_distance_from_mouse = (mouse_world - origin).Length2D();
			if (frames_until_mouse == frame_limit && new_distance_from_mouse > distance_from_mouse)
				frames_until_mouse = frame + 1;
			distance_from_mouse = new_distance_from_mouse;

			// If we bumped into something along the way
			if (frames_until_non_ground_collision == frame_limit && processed_frame.fractions[0] != 1) {
				auto n = processed_frame.normalzs[0];
				// And it wasn't a ground or a ceiling
				if (n < 0.7 && n != -1)
					frames_until_non_ground_collision = frame;
			}

			positions.push_back(origin);
		}

		hw.StopTracing();

		frame_bulk.SetRepeats(frames_until_mouse);

		// Draw the positions.
		pTriAPI->RenderMode(kRenderTransColor);
		pTriAPI->Color4f(0, 1, 0, 1);
		TriangleUtils::DrawPyramid(pTriAPI, mouse_world, 10, 20);

		for (size_t frame = 1; frame < positions.size(); ++frame) {
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

			TriangleUtils::DrawLine(pTriAPI, positions[frame - 1], positions[frame]);
		}

		if (left_got_pressed) {
			hw.SetEditStrafe(false);

			if (frame_count == 0)
				return;

			hw.input.InsertFrame(frame_count - 1, frame_bulk);

			auto err = hw.input.Save(hw.hltas_filename);
			if (err.Code == HLTAS::ErrorCode::OK)
				hw.ORIG_Con_Printf("Saved the script: %s\n", hw.hltas_filename.c_str());
			else
				hw.ORIG_Con_Printf("Error saving the script: %s\n", HLTAS::GetErrorMessage(err).c_str());
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
