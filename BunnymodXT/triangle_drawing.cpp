#include "stdafx.hpp"

#include "custom_triggers.hpp"
#include "triangle_drawing.hpp"
#include "triangle_utils.hpp"
#include "modules.hpp"

#include "hud_custom.hpp"

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

	static void DrawDisplacerTargets(triangleapi_s* pTriAPI)
	{
		if (!CVars::bxt_show_displacer_earth_targets.GetBool())
			return;

		pTriAPI->RenderMode(kRenderTransAdd);
		pTriAPI->CullFace(TRI_NONE);
		pTriAPI->Color4f(0.0f, 0.627f, 0.0f, 1.0f);
		for (const Vector* position : ServerDLL::GetInstance().GetDisplacerTargets()) {
			TriangleUtils::DrawPyramid(pTriAPI, *position, 5, 15);
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
		if (!player)
			return;

		const auto playerOrigin = player->v.origin;
		auto forward = ClientDLL::GetInstance().AnglesToForward(player->v.v_angle);

		const auto& si = CustomHud::GetScreenInfo();
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

			if (!CVars::bxt_triggers_color.IsEmpty()) {
				unsigned r = 0, g = 0, b = 0, a = 0;
				std::istringstream ss(CVars::bxt_triggers_color.GetString());
				ss >> r >> g >> b >> a;

				static float triggerColor[4];
				triggerColor[0] = r / 255.0f;
				triggerColor[1] = g / 255.0f;
				triggerColor[2] = b / 255.0f;
				triggerColor[3] = a / 255.0f;

				pTriAPI->Color4f(triggerColor[0], triggerColor[1], triggerColor[2], triggerColor[3]);
			} else {
				pTriAPI->Color4f(1.0f, 0.5f, 0.0f, 0.3f);
			}

			TriangleUtils::DrawAACuboid(pTriAPI, corner_positions.first, corner_positions.second);

			pTriAPI->RenderMode(kRenderTransColor);
			pTriAPI->Color4f(0.5f, 0.3f, 0.0f, 1.0f);
			TriangleUtils::DrawAACuboidWireframe(pTriAPI, corner_positions.first, corner_positions.second);
		}
	}

	static void DrawAbsMinMax(triangleapi_s *pTriAPI)
	{
		static const std::vector<std::string> PICKABLE_PREFIX{
			"item_",
			"ammo_",
			"weapon_",
			"weaponbox",
		};

		if (!CVars::bxt_show_pickup_bbox.GetBool())
			return;

		pTriAPI->CullFace(TRI_NONE);

		const auto& hw = HwDLL::GetInstance();
		const auto& server = ServerDLL::GetInstance();
		const enginefuncs_t* engfuncs = server.pEngfuncs;
		if (!engfuncs) {
			return;
		}

		edict_t* edicts = nullptr;
		const int numEdicts = hw.GetEdicts(&edicts);
		for (int e = 0; e < numEdicts; ++e) {
			const edict_t* ent = edicts + e;
			if (!hw.IsValidEdict(ent)) {
				continue;
			}

			const char* classname = server.GetString(ent->v.classname);
			if (strcmp(classname, "player") == 0) {
				pTriAPI->RenderMode(kRenderTransColor);
				pTriAPI->Color4f(0.0f, 1.0f, 0.0f, 1.0f);
				TriangleUtils::DrawAACuboidWireframe(pTriAPI, ent->v.absmin, ent->v.absmax);
				if (CVars::bxt_show_pickup_bbox.GetInt() == 2) {
					pTriAPI->RenderMode(kRenderTransAdd);
					pTriAPI->Color4f(0.0f, 1.0f, 0.0f, 0.1f);
					TriangleUtils::DrawAACuboid(pTriAPI, ent->v.absmin, ent->v.absmax);
				}
				continue;
			}

			const auto it = std::find_if(PICKABLE_PREFIX.cbegin(), PICKABLE_PREFIX.cend(), [classname](const std::string& val) {
				return strncmp(classname, val.c_str(), val.length()) == 0;
			});
			if (it == PICKABLE_PREFIX.cend()) {
				continue;
			}

			if (ent->v.movetype == MOVETYPE_FOLLOW) {
				// These are usually weapons attached to the player
				continue;
			}

			pTriAPI->RenderMode(kRenderTransAdd);
			pTriAPI->Color4f(1.0f, 0.6f, 0.0f, 0.3f);
			TriangleUtils::DrawAACuboid(pTriAPI, ent->v.absmin, ent->v.absmax);
		}
	}

	static Vector perpendicular(const Vector &prev, const Vector &next) {
		Vector perpendicular;

		auto line = (next - prev).Normalize();
		if (line.x == 0 && line.y == 0)
			perpendicular = Vector(1, 0, 0);
		else if (line.x == 0)
			perpendicular = Vector(1, 0, 0);
		else if (line.y == 0)
			perpendicular = Vector(0, 1, 0);
		else
			perpendicular = Vector(1, -line.x / line.y, 0).Normalize();

		// Make sure it's oriented in a particular way: this makes right-drag to change
		// yaw behave as expected (the yaw will change in the direction where you move
		// the mouse).
		if (perpendicular.x * line.y - perpendicular.y * line.x > 0) {
			perpendicular.x = -perpendicular.x;
			perpendicular.y = -perpendicular.y;
		}

		return perpendicular;
	}

	enum KeyFrameType {
		FRAME_BULK,
		CHANGE_END,
	};

	struct KeyFrame {
		KeyFrameType type;
		// For CHANGE_END this is the change line.
		size_t frame_bulk_index;
		size_t frame;
	};

	struct Selection {
		KeyFrameType type;
		// 0 means nothing is selected.
		size_t frame_bulk_index;
		size_t initial_frame;
		size_t last_frame;
	};

	static void DrawTASEditor(triangleapi_s *pTriAPI)
	{
		using HLStrafe::HullType;
		using HLTAS::StrafeDir;
		using HLTAS::StrafeType;
		using std::vector;
		const double M_DEG2RAD = M_PI / 180;

		auto& hw = HwDLL::GetInstance();
		if (hw.tas_editor_mode == TASEditorMode::DISABLED)
			return;
		auto& cl = ClientDLL::GetInstance();

		int x, y;
		auto mouse_state = SDL::GetInstance().GetMouseState(&x, &y);
		Vector2D mouse(static_cast<float>(x), static_cast<float>(y));

		// Convert from ScreenToWorld coordinates to SDL_GetMouseState coordinates.
		auto stw_to_pixels = [](Vector2D v) {
			return Vector2D(
				TriangleUtils::ProportionToPixelWidth((v.x + 1) / 2),
				TriangleUtils::ProportionToPixelHeight(-(v.y - 1) / 2)
			);
		};

		static bool left_was_pressed = false;
		static Vector2D left_pressed_at = Vector2D(0, 0);
		auto left_pressed = (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
		bool left_got_pressed = false;
		if (left_pressed && !left_was_pressed) {
			left_got_pressed = true;
			left_pressed_at = mouse;
		}
		left_was_pressed = left_pressed;

		static bool middle_was_pressed = false;
		static Vector2D middle_pressed_at = Vector2D(0, 0);
		auto middle_pressed = (mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
		bool middle_got_pressed = false;
		if (middle_pressed && !middle_was_pressed) {
			middle_got_pressed = true;
			middle_pressed_at = mouse;
		}
		middle_was_pressed = middle_pressed;

		static bool right_was_pressed = false;
		static Vector2D right_pressed_at = Vector2D(0, 0);
		auto right_pressed = (mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
		bool right_got_pressed = false;
		if (right_pressed && !right_was_pressed) {
			right_got_pressed = true;
			right_pressed_at = mouse;
		}
		right_was_pressed = right_pressed;

		static bool mouse4_was_pressed = false;
		static Vector2D mouse4_pressed_at = Vector2D(0, 0);
		auto mouse4_pressed = (mouse_state & SDL_BUTTON(SDL_BUTTON_X1)) != 0;
		bool mouse4_got_pressed = false;
		if (mouse4_pressed && !mouse4_was_pressed) {
			mouse4_got_pressed = true;
			mouse4_pressed_at = mouse;
		}
		mouse4_was_pressed = mouse4_pressed;

		float adjustment_speed = 1;

		// Like in HwDLL::FreeCamTick().
		auto buttons = cl.last_buttons;
		if (buttons & IN_ALT1)
			adjustment_speed *= 20;
		if (buttons & IN_DUCK)
			adjustment_speed /= 20;

		auto view = cl.last_vieworg;
		auto forward = cl.AnglesToForward(cl.last_viewangles);

		auto& input = hw.tas_editor_input;
		const auto& player_datas = input.player_datas;
		const auto& fractions = input.fractions;
		const auto& normalzs = input.normalzs;
		const auto& frame_bulk_starts = input.frame_bulk_starts;

		if (input.frame_bulks.size() == 0)
			return;

		input.simulate();

		size_t stale_index = std::numeric_limits<size_t>::max();

		if (CVars::bxt_tas_editor_camera_editor.GetBool()) {
			// Draw the camera angles.
			for (size_t frame = 1; frame < player_datas.size(); ++frame) {
				const auto origin = Vector(player_datas[frame].Origin);

				if (frame >= input.first_predicted_frame)
					pTriAPI->Color4f(0.1f, 0.1f, 1, 1);
				else
					pTriAPI->Color4f(0.4f, 0.4f, 1, 1);

				auto forward = cl.AnglesToForward(player_datas[frame].Viewangles);
				TriangleUtils::DrawLine(pTriAPI, origin, origin + forward * 5);
			}

			size_t closest_frame = 0;
			float closest_frame_px_dist = INFINITY;

			// Draw the path.
			for (size_t frame = 1; frame < player_datas.size(); ++frame) {
				const auto origin = Vector(player_datas[frame].Origin);

				if (frame >= input.first_predicted_frame)
					pTriAPI->Color4f(0.5f, 0.5f, 0.5f, 1);
				else
					pTriAPI->Color4f(0.8f, 0.8f, 0.8f, 1);

				const auto prev_origin = Vector(player_datas[frame - 1].Origin);
				TriangleUtils::DrawLine(pTriAPI, prev_origin, origin);

				// If we're inserting, we need to find the closest frame.
				if (hw.tas_editor_insert_point_held) {
					auto disp = origin - view;
					if (DotProduct(forward, disp) > 0) {
						Vector origin_ = origin;
						Vector screen_point;
						pTriAPI->WorldToScreen(origin_, screen_point);
						auto screen_point_px = stw_to_pixels(screen_point.Make2D());
						auto dist = (screen_point_px - mouse).Length();

						if (dist < closest_frame_px_dist) {
							closest_frame = frame;
							closest_frame_px_dist = dist;
						}
					}
				}
			}

			vector<KeyFrame> key_frames;

			// Draw the key frames.
			for (size_t i = 0; i < frame_bulk_starts.size() - 1; ++i) {
				auto& line = input.frame_bulks[i];

				const auto frame = frame_bulk_starts[i];
				const auto& player = player_datas[frame];

				pTriAPI->Color4f(0.8f, 0.8f, 0.8f, 1);

				// TODO: we want the previous position to compute the perpendicular.
				if (frame == 0)
					continue;

				// TODO: we want the next player data to get next angles and position.
				if (frame + 1 >= player_datas.size())
					continue;

				Vector prev_origin = player_datas[frame - 1].Origin;
				Vector origin = player.Origin;
				Vector next_origin = player_datas[frame + 1].Origin;
				auto perp = perpendicular(prev_origin, origin);
				perp *= 5;
				Vector a = origin - perp, b = origin + perp;

				if (line.AlgorithmParametersPresent) {
					TriangleUtils::DrawLine(pTriAPI, a, b);

					switch (line.GetAlgorithmParameters().Type) {
						case HLTAS::ConstraintsType::VELOCITY:
						case HLTAS::ConstraintsType::VELOCITY_AVG:
						case HLTAS::ConstraintsType::VELOCITY_LOCK:
							pTriAPI->Color4f(0, 1, 0, 1);
							break;
						case HLTAS::ConstraintsType::YAW:
						case HLTAS::ConstraintsType::YAW_RANGE:
							pTriAPI->Color4f(0, 1, 1, 1);
							break;
					}

					Vector next_angles = player_datas[frame + 1].Viewangles;
					next_angles[0] = 0;
					next_angles[2] = 0;
					auto forward = cl.AnglesToForward(next_angles);
					TriangleUtils::DrawLine(pTriAPI, origin, origin + forward * 20);

					key_frames.emplace_back(KeyFrame { KeyFrameType::FRAME_BULK, i, frame });
				} else if (line.ChangePresent) {
					// Draw an arrow head facing back to mark the start of the change.
					auto diff = (next_origin - origin).Normalize() * 5;
					TriangleUtils::DrawLine(pTriAPI, a + diff, origin);
					TriangleUtils::DrawLine(pTriAPI, origin, b + diff);

					Vector next_angles = player_datas[frame + 1].Viewangles;
					next_angles[2] = 0;
					switch (line.GetChangeTarget()) {
						case HLTAS::ChangeTarget::YAW:
						case HLTAS::ChangeTarget::TARGET_YAW:
							next_angles[0] = 0;
							break;
						default:
							break;
					}

					auto forward = cl.AnglesToForward(next_angles);
					pTriAPI->Color4f(1, 0, 0, 1);
					TriangleUtils::DrawLine(pTriAPI, origin, origin + forward * 20);

					key_frames.emplace_back(KeyFrame { KeyFrameType::FRAME_BULK, i, frame });

					// Find the end of the change.
					auto time_left = line.GetChangeOver();
					size_t next_frame = frame;
					for (size_t j = i + 1; j < input.frame_bulks.size(); j++) {
						if (next_frame + 1 >= player_datas.size())
							break;

						const auto& next_line = input.frame_bulks[j];
						if (!next_line.IsMovement())
							continue;

						const auto host_frametime = std::strtof(next_line.Frametime.c_str(), nullptr);
						const auto frametime = static_cast<float>(static_cast<float>(std::floor(host_frametime * 1000)) * 0.001);

						for (size_t repeat = 0; repeat < next_line.GetRepeats(); repeat++) {
							if (next_frame + 1 >= player_datas.size())
								break;

							auto time = frametime;
							if (input.next_frame_is_0mss[next_frame])
								time = 0;

							next_frame++;

							time_left -= time;
							if (time_left <= 0)
								break;
						}

						if (time_left <= 0)
							break;
					}

					if (time_left <= 0 && next_frame < player_datas.size()) {
						Vector prev_origin = player_datas[next_frame - 1].Origin;
						Vector origin = player_datas[next_frame].Origin;
						auto perp = perpendicular(prev_origin, origin);
						perp *= 5;
						Vector a = origin - perp, b = origin + perp;

						// Draw an arrow head facing forward to mark the end of the change.
						auto diff = (prev_origin - origin).Normalize() * 5;
						pTriAPI->Color4f(0.8f, 0.8f, 0.8f, 1);
						TriangleUtils::DrawLine(pTriAPI, a + diff, origin);
						TriangleUtils::DrawLine(pTriAPI, origin, b + diff);

						Vector angles = player_datas[next_frame].Viewangles;
						angles[2] = 0;
						switch (line.GetChangeTarget()) {
							case HLTAS::ChangeTarget::YAW:
							case HLTAS::ChangeTarget::TARGET_YAW:
								angles[0] = 0;
								angles[1] = line.GetChangeFinalValue();
								break;
							case HLTAS::ChangeTarget::PITCH:
								angles[0] = line.GetChangeFinalValue();
								break;
						}

						auto forward = cl.AnglesToForward(angles);
						pTriAPI->Color4f(1, 1, 0, 1);
						TriangleUtils::DrawLine(pTriAPI, origin, origin + forward * 20);

						key_frames.emplace_back(KeyFrame { KeyFrameType::CHANGE_END, i, next_frame });
					}
				}
			}

			// Find the closest key frame.
			static Selection selection { KeyFrameType::FRAME_BULK, 0, 0, 0 };

			if (left_pressed || middle_pressed || right_pressed || mouse4_pressed || hw.tas_editor_insert_point_held) {
				// Don't change the selected frame bulk while dragging.
			} else {
				selection.frame_bulk_index = 0;

				float selected_px_dist = INFINITY;
				for (const auto& key_frame : key_frames) {
					const auto origin = Vector(player_datas[key_frame.frame].Origin);
					auto disp = origin - view;
					if (DotProduct(forward, disp) > 0) {
						Vector origin_ = origin;
						Vector screen_point;
						pTriAPI->WorldToScreen(origin_, screen_point);
						auto screen_point_px = stw_to_pixels(screen_point.Make2D());
						auto dist = (screen_point_px - mouse).Length();

						if (dist <= selected_px_dist) {
							selected_px_dist = dist;
							selection.type = key_frame.type;
							selection.frame_bulk_index = key_frame.frame_bulk_index;
							selection.initial_frame = key_frame.frame;
							selection.last_frame = key_frame.frame;
						}
					}
				}
			}

			static Vector2D saved_lmb_diff;

			// Compute vectors we'll use for reference when computing the drag adjustment.
			if (selection.frame_bulk_index > 0 && left_got_pressed) {
				if (selection.initial_frame > 0 && selection.initial_frame < player_datas.size()) {
					const auto& player = player_datas[selection.initial_frame];
					const auto& prev_player = player_datas[selection.initial_frame - 1];

					Vector origin_ = player.Origin;
					Vector screen_point;
					pTriAPI->WorldToScreen(origin_, screen_point);
					auto screen_point_px = stw_to_pixels(screen_point.Make2D());
					Vector prev_origin_ = prev_player.Origin;
					Vector prev_screen_point;
					pTriAPI->WorldToScreen(prev_origin_, prev_screen_point);
					auto prev_screen_point_px = stw_to_pixels(prev_screen_point.Make2D());
					saved_lmb_diff = (screen_point_px - prev_screen_point_px).Normalize();
				} else {
					// If we can't get the vectors, reset the selection.
					// This shouldn't happen.
					selection.frame_bulk_index = 0;
					hw.ORIG_Con_Printf("Invalid selection frame? This is a bug, please report it.\n");
				}
			}

			if (selection.frame_bulk_index > 0 && (left_pressed || (hw.tas_editor_insert_point_held && !hw.tas_editor_insert_point && selection.initial_frame < player_datas.size()))) {
				auto mouse_diff = mouse - left_pressed_at;

				auto amount = DotProduct(mouse_diff, saved_lmb_diff) * 0.1f * adjustment_speed;
				const auto new_frame = static_cast<size_t>(std::max(1, static_cast<int>(selection.initial_frame) + static_cast<int>(amount)));

				if (hw.tas_editor_insert_point_held) {
					auto& frame_bulk = input.frame_bulks[selection.frame_bulk_index];

					// If we're insert-dragging and mouse is at the initial frame, change back to target_yaw velocity_lock frame.
					if (new_frame == selection.initial_frame && frame_bulk.ChangePresent) {
						frame_bulk.ChangePresent = false;
						auto parameters = HLTAS::AlgorithmParameters {};
						parameters.Type = HLTAS::ConstraintsType::VELOCITY_LOCK;
						parameters.Parameters.VelocityLock.Constraints = 0;
						frame_bulk.SetAlgorithmParameters(parameters);
						selection.type = KeyFrameType::FRAME_BULK;
					}

					// If we're insert-dragging and mouse left initial frame, change to change frame.
					if (new_frame != selection.initial_frame) {
						if (frame_bulk.AlgorithmParametersPresent) {
							frame_bulk.AlgorithmParametersPresent = false;
							frame_bulk.ChangePresent = true;
							frame_bulk.SetChangeTarget(HLTAS::ChangeTarget::TARGET_YAW);
							frame_bulk.SetChangeOver(0); // Will be adjusted by the code below.

							// It would make more sense to set the value to new_frame when new_frame is after initial_frame,
							// but at this point viewangles at new_frame are already recomputed with this change,
							// so they are effectively equal to initial_frame viewangles...
							frame_bulk.SetChangeFinalValue(player_datas[selection.initial_frame].Viewangles[1]);
						}

						if (new_frame < selection.initial_frame) {
							selection.type = KeyFrameType::FRAME_BULK;
						} else {
							selection.type = KeyFrameType::CHANGE_END;
						}
					}
				}

				// Figure out, before or in the middle of which frame bulk the line will go.
				auto split_at = new_frame;
				size_t i;
				for (i = 0; i < input.frame_bulks.size(); ++i) {
					if (!input.frame_bulks[i].IsMovement())
						continue;

					if (split_at < input.frame_bulks[i].GetRepeats())
						break;

					split_at -= input.frame_bulks[i].GetRepeats();
				}

				// Second condition is for doing nothing when the place won't change.
				// + 1 since we always move to one before a movement frame bulk.
				if (i < input.frame_bulks.size() && !(split_at == 0 && i == selection.frame_bulk_index + 1)) {
					// Figure out how much time is between new and last frame.
					float time_delta = 0;
					if (input.frame_bulks[selection.frame_bulk_index].ChangePresent) {
						auto lo = std::min(new_frame, selection.last_frame);
						auto hi = std::max(new_frame, selection.last_frame);

						if (hi < player_datas.size()) {
							for (size_t j = lo + 1; j <= hi; j++)
								time_delta += input.frametimes[j];
						}

						// If we're moving the start forward or the end back, we need to reduce the time delta.
						if ((new_frame > selection.last_frame) == (selection.type == FRAME_BULK))
							time_delta *= -1;
					}

					// If this is a change line, we need a time delta to adjust it.
					if (!(input.frame_bulks[selection.frame_bulk_index].ChangePresent && time_delta == 0)) {
						// Adjust the time according to the delta.
						if (input.frame_bulks[selection.frame_bulk_index].ChangePresent) {
							const auto old_time = input.frame_bulks[selection.frame_bulk_index].GetChangeOver();
							// TODO: better to prevent dragging one point past another altogether.
							const auto new_time = std::max(0.f, old_time + time_delta);
							input.frame_bulks[selection.frame_bulk_index].SetChangeOver(new_time);
						}

						selection.last_frame = new_frame;

						if (selection.type == CHANGE_END) {
							// No need to move the frame bulk in this case, just mark it as stale.
							stale_index = selection.frame_bulk_index;
						} else {
							// Move the actual frame bulk.
							auto frame_bulk = input.frame_bulks[selection.frame_bulk_index];
							input.frame_bulks.erase(input.frame_bulks.begin() + selection.frame_bulk_index);

							if (i > selection.frame_bulk_index) {
								// Adjust i if we erased before it.
								i--;
								stale_index = selection.frame_bulk_index;
							} else {
								stale_index = i;
							}

							// Join back two frame bulks we just connected together if they are the same.
							{
								auto& prev = input.frame_bulks[selection.frame_bulk_index - 1];
								auto& next = input.frame_bulks[selection.frame_bulk_index];
								if (prev.IsMovement() && next.IsMovement() && next.Comments.empty()) {
									const auto temp = next.GetRepeats();
									next.SetRepeats(prev.GetRepeats());
									next.Comments = prev.Comments;
									const auto equal = prev.IsEqualToMovementFrame(next);
									next.Comments.clear();
									next.SetRepeats(temp);

									if (equal) {
										// If next was the selected frame bulk, adjust split_at accordingly.
										if (i == selection.frame_bulk_index)
											split_at += prev.GetRepeats();

										prev.SetRepeats(prev.GetRepeats() + next.GetRepeats());
										input.frame_bulks.erase(input.frame_bulks.begin() + selection.frame_bulk_index);
										stale_index = std::min(stale_index, selection.frame_bulk_index - 1);

										if (i >= selection.frame_bulk_index) {
											// Adjust i if we erased before it.
											i--;
										}
									}
								}
							}

							if (split_at == 0) {
								// Move it to before the frame bulk.
								input.frame_bulks.insert(input.frame_bulks.begin() + i, frame_bulk);
								selection.frame_bulk_index = i;
							} else {
								// Split the frame bulk in two and insert in the middle.
								auto new_frame_bulk_repeats = input.frame_bulks[i].GetRepeats() - split_at;
								input.set_repeats(i, split_at);
								auto new_frame_bulk = input.frame_bulks[i];
								new_frame_bulk.Commands.clear(); // So pause;bxt_tas_editor 1 doesn't copy over.
								new_frame_bulk.Comments.clear();
								new_frame_bulk.SetRepeats(new_frame_bulk_repeats);
								input.frame_bulks.insert(input.frame_bulks.begin() + i + 1, new_frame_bulk);

								// Insert our frame bulk in the middle.
								input.frame_bulks.insert(input.frame_bulks.begin() + i + 1, frame_bulk);
								selection.frame_bulk_index = i + 1;
							}
						}
					}
				}
			}

			if (closest_frame > 0 && closest_frame < player_datas.size() && hw.tas_editor_insert_point) {
				// Figure out, before or in the middle of which frame bulk the line will go.
				auto split_at = closest_frame;
				size_t i;
				for (i = 0; i < input.frame_bulks.size(); ++i) {
					if (!input.frame_bulks[i].IsMovement())
						continue;

					if (split_at < input.frame_bulks[i].GetRepeats())
						break;

					split_at -= input.frame_bulks[i].GetRepeats();
				}

				if (i < input.frame_bulks.size()) {
					auto frame_bulk = HLTAS::Frame();
					auto parameters = HLTAS::AlgorithmParameters {};
					parameters.Type = HLTAS::ConstraintsType::VELOCITY_LOCK;
					parameters.Parameters.VelocityLock.Constraints = 0;
					frame_bulk.SetAlgorithmParameters(parameters);

					if (split_at == 0) {
						// Put it before the frame bulk.
						input.frame_bulks.insert(input.frame_bulks.begin() + i, frame_bulk);
						selection.frame_bulk_index = i;
					} else {
						// Split the frame bulk in two and insert in the middle.
						auto new_frame_bulk_repeats = input.frame_bulks[i].GetRepeats() - split_at;
						input.set_repeats(i, split_at);
						auto new_frame_bulk = input.frame_bulks[i];
						new_frame_bulk.Commands.clear(); // So pause;bxt_tas_editor 1 doesn't copy over.
						new_frame_bulk.Comments.clear();
						new_frame_bulk.SetRepeats(new_frame_bulk_repeats);
						input.frame_bulks.insert(input.frame_bulks.begin() + i + 1, new_frame_bulk);

						// Insert our frame bulk in the middle.
						input.frame_bulks.insert(input.frame_bulks.begin() + i + 1, frame_bulk);
						selection.frame_bulk_index = i + 1;
					}

					stale_index = i;

					selection.type = KeyFrameType::FRAME_BULK;
					selection.initial_frame = closest_frame;
					selection.last_frame = closest_frame;

					// Cursed dragging re-use.
					left_pressed_at = mouse;

					const auto& player = player_datas[selection.initial_frame];
					const auto& prev_player = player_datas[selection.initial_frame - 1];
					Vector origin_ = player.Origin;
					Vector screen_point;
					pTriAPI->WorldToScreen(origin_, screen_point);
					auto screen_point_px = stw_to_pixels(screen_point.Make2D());
					Vector prev_origin_ = prev_player.Origin;
					Vector prev_screen_point;
					pTriAPI->WorldToScreen(prev_origin_, prev_screen_point);
					auto prev_screen_point_px = stw_to_pixels(prev_screen_point.Make2D());
					saved_lmb_diff = (screen_point_px - prev_screen_point_px).Normalize();
				}
			}

			if (selection.frame_bulk_index > 0 && hw.tas_editor_delete_point) {
				input.frame_bulks.erase(input.frame_bulks.begin() + selection.frame_bulk_index);
				stale_index = selection.frame_bulk_index;
			}
		} else {
			size_t next_frame_bulk_start_index = 1;

			// frame_bulk_starts always contains at least 1 element (zero), in which case we don't
			// want to access it with next_frame_bulk_start_index. Fortunately, frame is never zero,
			// so if we set this to 0 this will disable the code below that we want to disable.
			if (input.frame_bulk_starts.size() == 1)
				next_frame_bulk_start_index = 0;

			float closest_edge_px_dist;
			size_t closest_edge_frame = 0;

			static size_t closest_edge_prev_frame_bulk_index = 0;

			size_t frame_limit = player_datas.size() - 1;

			if (left_pressed || middle_pressed || right_pressed || mouse4_pressed) {
				// Don't change the selected frame bulk while dragging.
				if (closest_edge_prev_frame_bulk_index + 1 < frame_bulk_starts.size()) {
					closest_edge_frame = frame_bulk_starts[closest_edge_prev_frame_bulk_index + 1];
				} else {
					// Set to frame_limit if the simulation hasn't finished yet.
					closest_edge_frame = frame_limit;
				}
			} else {
				for (size_t i = 1; i < frame_bulk_starts.size(); ++i) {
					auto frame = frame_bulk_starts[i];

					const auto origin = Vector(player_datas[frame].Origin);
					auto disp = origin - view;
					if (DotProduct(forward, disp) > 0) {
						Vector origin_ = origin;
						Vector screen_point;
						pTriAPI->WorldToScreen(origin_, screen_point);
						auto screen_point_px = stw_to_pixels(screen_point.Make2D());
						auto dist = (screen_point_px - mouse).Length();

						if (closest_edge_frame == 0 || dist <= closest_edge_px_dist) {
							closest_edge_frame = frame;
							closest_edge_px_dist = dist;
							closest_edge_prev_frame_bulk_index = i - 1;
							while (closest_edge_prev_frame_bulk_index > 0
									&& !input.frame_bulks[closest_edge_prev_frame_bulk_index].IsMovement())
								closest_edge_prev_frame_bulk_index--;
						}
					}
				}
			}

			static int saved_repeats = 0;
			static int saved_repeats_next = 0;
			if ((left_got_pressed || middle_got_pressed) && closest_edge_frame != 0) {
				saved_repeats = input.frame_bulks[closest_edge_prev_frame_bulk_index].GetRepeats();
				if (closest_edge_prev_frame_bulk_index + 1 < input.frame_bulks.size())
					saved_repeats_next = input.frame_bulks[closest_edge_prev_frame_bulk_index + 1].GetRepeats();
			}

			static double saved_yaw = 0;
			if ((right_got_pressed || mouse4_got_pressed) && closest_edge_frame != 0
					&& input.frame_bulks[closest_edge_prev_frame_bulk_index].GetYawPresent())
				saved_yaw = input.frame_bulks[closest_edge_prev_frame_bulk_index].GetYaw();

			size_t frames_until_non_ground_collision = frame_limit;

			// Apply color to frame bulks right before and after the selected edge.
			size_t color_from = frame_limit;
			size_t color_to = frame_limit;
			if (closest_edge_frame != 0) {
				color_from = frame_bulk_starts[closest_edge_prev_frame_bulk_index];
				if (closest_edge_prev_frame_bulk_index + 2 < frame_bulk_starts.size())
					color_to = frame_bulk_starts[closest_edge_prev_frame_bulk_index + 2];
			}

			size_t closest_frame = 0;
			// This initial value is never used, but g++ in release mode doesn't see that.
			float closest_frame_px_dist = 0;

			static Vector2D saved_lmb_diff;
			static Vector2D saved_mmb_diff;
			static Vector2D saved_rmb_diff;
			static Vector2D saved_ms4_diff;

			Vector last_shown_view_angle;
			Vector last_shown_view_angle_origin;

			for (size_t frame = 1; frame < player_datas.size(); ++frame) {
				const auto origin = Vector(player_datas[frame].Origin);

				if (frame >= input.first_predicted_frame)
					pTriAPI->Color4f(0.5f, 0.5f, 0.5f, 1);
				else
					pTriAPI->Color4f(0.8f, 0.8f, 0.8f, 1);

				if (frame > color_from && frame <= color_to) {
					// If we bumped into something along the way
					if (frames_until_non_ground_collision == frame_limit && fractions[frame] != 1) {
						auto n = normalzs[frame];
						// And it wasn't a ground or a ceiling
						if (n < 0.7 && n != -1)
							frames_until_non_ground_collision = frame;
					}

					if (frame > frames_until_non_ground_collision) {
						if (frame >= input.first_predicted_frame)
							pTriAPI->Color4f(0.6f, 0, 0, 1);
						else
							pTriAPI->Color4f(1, 0, 0, 1);
					} else {
						if (frame >= input.first_predicted_frame)
							pTriAPI->Color4f(0, 0.6f, 0, 1);
						else
							pTriAPI->Color4f(0, 1, 0, 1);
					}
				}

				const auto prev_origin = Vector(player_datas[frame - 1].Origin);
				TriangleUtils::DrawLine(pTriAPI, prev_origin, origin);

				// Draw the view angle.
				{
					auto forward = cl.AnglesToForward(player_datas[frame].Viewangles);

					if (last_shown_view_angle == Vector()
							// Angle differs by more than ~11.5 degrees.
							|| DotProduct(forward, last_shown_view_angle) < 0.98f
							|| (last_shown_view_angle_origin - origin).Length() > 50.f) {
						last_shown_view_angle = forward;
						last_shown_view_angle_origin = origin;
						forward = origin + forward * 5;
						pTriAPI->Color4f(0.4f, 0.4f, 1, 1);
						TriangleUtils::DrawLine(pTriAPI, origin, forward);
					}
				}

				// If we're inserting, we need to find the closest frame.
				if (hw.tas_editor_insert_point) {
					auto disp = origin - view;
					if (DotProduct(forward, disp) > 0) {
						Vector origin_ = origin;
						Vector screen_point;
						pTriAPI->WorldToScreen(origin_, screen_point);
						auto screen_point_px = stw_to_pixels(screen_point.Make2D());
						auto dist = (screen_point_px - mouse).Length();

						if (closest_frame == 0 || dist < closest_frame_px_dist) {
							closest_frame = frame;
							closest_frame_px_dist = dist;
						}
					}
				}

				// Reset the coloring on the edge.
				if (closest_edge_frame != 0 && frame == closest_edge_frame)
					frames_until_non_ground_collision = frame_limit;

				if (frame == frame_bulk_starts[next_frame_bulk_start_index]) {
					while (next_frame_bulk_start_index + 1 != frame_bulk_starts.size()
							&& frame == frame_bulk_starts[next_frame_bulk_start_index])
						++next_frame_bulk_start_index;

					auto perp = perpendicular(prev_origin, origin);
					perp *= 5;
					Vector a = origin + perp, b = origin - perp;

					if (frame == closest_edge_frame) {
						auto& frame_bulk = input.frame_bulks[closest_edge_prev_frame_bulk_index];

						// Visualize the target yaw.
						if (frame_bulk.GetYawPresent()) {
							auto yaw = frame_bulk.GetYaw() * M_DEG2RAD;
							auto yaw_dir = Vector(static_cast<float>(std::cos(yaw)), static_cast<float>(std::sin(yaw)), 0);
							yaw_dir *= 20;
							pTriAPI->Color4f(0.5, 0.5, 1, 1);
							TriangleUtils::DrawLine(pTriAPI, origin - yaw_dir, origin + yaw_dir);
						}

						pTriAPI->Color4f(1, 1, 1, 1);

						if (left_got_pressed) {
							Vector origin_ = origin;
							Vector screen_point;
							pTriAPI->WorldToScreen(origin_, screen_point);
							auto screen_point_px = stw_to_pixels(screen_point.Make2D());
							Vector prev_origin_ = prev_origin;
							Vector prev_screen_point;
							pTriAPI->WorldToScreen(prev_origin_, prev_screen_point);
							auto prev_screen_point_px = stw_to_pixels(prev_screen_point.Make2D());
							saved_lmb_diff = (screen_point_px - prev_screen_point_px).Normalize();
						}

						if (middle_got_pressed) {
							Vector origin_ = origin;
							Vector screen_point;
							pTriAPI->WorldToScreen(origin_, screen_point);
							auto screen_point_px = stw_to_pixels(screen_point.Make2D());
							Vector prev_origin_ = prev_origin;
							Vector prev_screen_point;
							pTriAPI->WorldToScreen(prev_origin_, prev_screen_point);
							auto prev_screen_point_px = stw_to_pixels(prev_screen_point.Make2D());
							saved_mmb_diff = (screen_point_px - prev_screen_point_px).Normalize();
						}

						if (right_got_pressed) {
							Vector a_screen_point;
							pTriAPI->WorldToScreen(a, a_screen_point);
							auto a_screen_point_px = stw_to_pixels(a_screen_point.Make2D());
							Vector b_screen_point;
							pTriAPI->WorldToScreen(b, b_screen_point);
							auto b_screen_point_px = stw_to_pixels(b_screen_point.Make2D());
							saved_rmb_diff = (a_screen_point_px - b_screen_point_px).Normalize();
						}

						if (mouse4_got_pressed) {
							Vector a_screen_point;
							pTriAPI->WorldToScreen(a, a_screen_point);
							auto a_screen_point_px = stw_to_pixels(a_screen_point.Make2D());
							Vector b_screen_point;
							pTriAPI->WorldToScreen(b, b_screen_point);
							auto b_screen_point_px = stw_to_pixels(b_screen_point.Make2D());
							saved_ms4_diff = (a_screen_point_px - b_screen_point_px).Normalize();
						}
					} else {
						pTriAPI->Color4f(0.8f, 0.8f, 0.8f, 1);
					}

					TriangleUtils::DrawLine(pTriAPI, a, b);
				}
			}

			if (closest_edge_frame != 0) {
				auto& frame_bulk = input.frame_bulks[closest_edge_prev_frame_bulk_index];

				float current_player_vel = 0;
				float current_player_zvel = 0;
				float current_player_zpos = 0;
				float current_player_realyaw = 0;

				float current_player_health = 0;
				float current_player_armor = 0;

				if (input.player_datas.size() > closest_edge_frame) {
					auto& current_player_data = input.player_datas[closest_edge_frame];
					auto& current_player_vels = current_player_data.Velocity;

					current_player_vel = std::hypotf(current_player_vels[0], current_player_vels[1]);
					current_player_zvel = current_player_vels[2];
					current_player_zpos = current_player_data.Origin[2];
					current_player_realyaw = current_player_data.Viewangles[1];

					current_player_health = input.player_health_datas[closest_edge_frame];
					current_player_armor = input.player_armor_datas[closest_edge_frame];
				}

				// Update the HUD status before any changes, since that's the state that was visualized earlier.
				CustomHud::UpdateTASEditorStatus(
					frame_bulk,
					current_player_vel, current_player_zvel, current_player_zpos,
					current_player_realyaw,
					current_player_health, current_player_armor);

				if (left_pressed) {
					auto mouse_diff = mouse - left_pressed_at;

					auto amount = DotProduct(mouse_diff, saved_lmb_diff) * 0.1f * adjustment_speed;
					auto new_repeats = static_cast<unsigned>(std::max(1, saved_repeats + static_cast<int>(amount)));
					input.set_repeats(closest_edge_prev_frame_bulk_index, new_repeats);
				}

				if (middle_pressed) {
					auto mouse_diff = mouse - middle_pressed_at;

					auto amount = DotProduct(mouse_diff, saved_mmb_diff) * 0.1f * adjustment_speed;
					auto new_repeats = std::max(1, saved_repeats + static_cast<int>(amount));

					if (closest_edge_prev_frame_bulk_index + 1 < input.frame_bulks.size()) {
						auto new_repeats_next = std::max(1, saved_repeats_next - static_cast<int>(amount));
						auto change = std::min(std::abs(new_repeats - saved_repeats), std::abs(new_repeats_next - saved_repeats_next));
						new_repeats = saved_repeats + change * (amount < 0 ? -1 : 1);
						new_repeats_next = saved_repeats_next + change * (amount < 0 ? 1 : -1);
						input.set_repeats(closest_edge_prev_frame_bulk_index, static_cast<unsigned>(new_repeats));
						input.set_repeats(closest_edge_prev_frame_bulk_index + 1, static_cast<unsigned>(new_repeats_next));
					} else {
						input.set_repeats(closest_edge_prev_frame_bulk_index, static_cast<unsigned>(new_repeats));
					}
				}

				if (right_pressed && frame_bulk.GetYawPresent()) {
					auto mouse_diff = mouse - right_pressed_at;

					auto amount = DotProduct(mouse_diff, saved_rmb_diff) * 0.1f * adjustment_speed;
					auto new_yaw = saved_yaw + amount;
					if (frame_bulk.GetYaw() != new_yaw) {
						stale_index = closest_edge_prev_frame_bulk_index;
						frame_bulk.SetYaw(new_yaw);
					}
				}

				if (mouse4_pressed && frame_bulk.GetYawPresent()) {
					auto mouse_diff = mouse - mouse4_pressed_at;

					auto amount = DotProduct(mouse_diff, saved_ms4_diff) * 0.1f * adjustment_speed;
					auto new_yaw = saved_yaw + amount;

					if (frame_bulk.GetYaw() != new_yaw) {
						auto old_yaw = frame_bulk.GetYaw();

						stale_index = closest_edge_prev_frame_bulk_index;
						frame_bulk.SetYaw(new_yaw);

						// SetYaw towards previous framebulks
						if (closest_edge_prev_frame_bulk_index != 0) {
							for (size_t i = closest_edge_prev_frame_bulk_index; i > 0; i--) {
								auto real_index = i - 1;

								HLTAS::Frame *prev_framebulk = &(input.frame_bulks[real_index]);

								if (prev_framebulk->GetYawPresent() && prev_framebulk->GetYaw() == old_yaw) {
									stale_index = real_index;
									prev_framebulk->SetYaw(new_yaw);
								} else {
									break;
								}
							}
						}

						// SetYaw towards end of all frame bulks
						for (size_t i = closest_edge_prev_frame_bulk_index + 1; i < input.frame_bulks.size(); i++) {
							HLTAS::Frame *next_framebulk = &(input.frame_bulks[i]);

							if (next_framebulk->GetYawPresent() && next_framebulk->GetYaw() == old_yaw) {
								next_framebulk->SetYaw(new_yaw);
							} else {
								break;
							}
						}
					}
				}

				if (hw.tas_editor_toggle_s03) {
					if (!frame_bulk.Strafe
							|| frame_bulk.GetDir() != HLTAS::StrafeDir::YAW
							|| frame_bulk.GetType() != HLTAS::StrafeType::MAXACCEL) {
						frame_bulk.Strafe = true;
						frame_bulk.SetDir(HLTAS::StrafeDir::YAW);
						frame_bulk.SetType(HLTAS::StrafeType::MAXACCEL);

						if (!frame_bulk.GetYawPresent()) {
							const auto& prev_frame_bulk = input.frame_bulks[closest_edge_prev_frame_bulk_index];
							frame_bulk.SetYaw(prev_frame_bulk.GetYawPresent() ? prev_frame_bulk.GetYaw() : 0);
						}
					} else {
						frame_bulk.Strafe = false;
					}
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_s13) {
					if (!frame_bulk.Strafe
							|| frame_bulk.GetDir() != HLTAS::StrafeDir::YAW
							|| frame_bulk.GetType() != HLTAS::StrafeType::MAXANGLE) {
						frame_bulk.Strafe = true;
						frame_bulk.SetDir(HLTAS::StrafeDir::YAW);
						frame_bulk.SetType(HLTAS::StrafeType::MAXANGLE);

						if (!frame_bulk.GetYawPresent()) {
							const auto& prev_frame_bulk = input.frame_bulks[closest_edge_prev_frame_bulk_index];
							frame_bulk.SetYaw(prev_frame_bulk.GetYawPresent() ? prev_frame_bulk.GetYaw() : 0);
						}
					} else {
						frame_bulk.Strafe = false;
					}
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_s22) {
					if (!frame_bulk.Strafe
							|| frame_bulk.GetDir() != HLTAS::StrafeDir::BEST
							|| frame_bulk.GetType() != HLTAS::StrafeType::MAXDECCEL) {
						frame_bulk.Strafe = true;
						frame_bulk.SetDir(HLTAS::StrafeDir::BEST);
						frame_bulk.SetType(HLTAS::StrafeType::MAXDECCEL);
						frame_bulk.SetYawPresent(false);
					} else {
						frame_bulk.Strafe = false;
					}
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_lgagst) {
					if (frame_bulk.Lgagst) {
						frame_bulk.Lgagst = false;
					} else if (frame_bulk.Autojump || frame_bulk.Ducktap) {
						frame_bulk.Lgagst = true;
					}
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_autojump) {
					frame_bulk.Autojump = !frame_bulk.Autojump;
					if (frame_bulk.Autojump) {
						if (frame_bulk.Ducktap)
							frame_bulk.Ducktap = false;
					} else {
						frame_bulk.Lgagst = false;
					}
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_ducktap) {
					frame_bulk.Ducktap = !frame_bulk.Ducktap;
					if (frame_bulk.Ducktap) {
						frame_bulk.SetDucktap0ms(!hw.frametime0ms.empty());
						if (frame_bulk.Autojump)
							frame_bulk.Autojump = false;
					} else {
						frame_bulk.Lgagst = false;
					}
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_jumpbug) {
					frame_bulk.Jumpbug = !frame_bulk.Jumpbug;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_dbc) {
					if (frame_bulk.Dbc)
						frame_bulk.SetDbcCeilings(false);
					frame_bulk.Dbc = !frame_bulk.Dbc;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_dbc_ceilings) {
					frame_bulk.SetDbcCeilings(!frame_bulk.GetDbcCeilings());
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_dbg) {
					frame_bulk.Dbg = !frame_bulk.Dbg;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_dwj) {
					frame_bulk.Dwj = !frame_bulk.Dwj;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_forward) {
					frame_bulk.Forward = !frame_bulk.Forward;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_left) {
					frame_bulk.Left = !frame_bulk.Left;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_right) {
					frame_bulk.Right = !frame_bulk.Right;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_back) {
					frame_bulk.Back = !frame_bulk.Back;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_up) {
					frame_bulk.Up = !frame_bulk.Up;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_down) {
					frame_bulk.Down = !frame_bulk.Down;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_jump) {
					frame_bulk.Jump = !frame_bulk.Jump;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_duck) {
					frame_bulk.Duck = !frame_bulk.Duck;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_use) {
					frame_bulk.Use = !frame_bulk.Use;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_attack1) {
					frame_bulk.Attack1 = !frame_bulk.Attack1;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_attack2) {
					frame_bulk.Attack2 = !frame_bulk.Attack2;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_toggle_reload) {
					frame_bulk.Reload = !frame_bulk.Reload;
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_set_yaw
						// If we're strafing, then if we can set the yaw it should already be present.
						// If we're not strafing, then we can set the yaw.
						&& (frame_bulk.GetYawPresent() || !frame_bulk.Strafe)) {
					frame_bulk.SetYaw(hw.tas_editor_set_yaw_yaw);
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_set_pitch) {
					frame_bulk.SetPitch(hw.tas_editor_set_pitch_pitch);
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_set_repeats) {
					frame_bulk.SetRepeats(hw.tas_editor_set_repeats_repeats);
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_set_commands) {
					frame_bulk.Commands = hw.tas_editor_set_commands_commands;
				}

				if (hw.tas_editor_unset_yaw
						// Yaw must be set if we're strafing to yaw or point.
						&& !(frame_bulk.Strafe
							&& (frame_bulk.GetDir() == StrafeDir::YAW
								|| frame_bulk.GetDir() == StrafeDir::POINT))) {
					frame_bulk.SetYawPresent(false);
					stale_index = closest_edge_prev_frame_bulk_index;
				}

				if (hw.tas_editor_unset_pitch) {
					frame_bulk.PitchPresent = false;
					stale_index = closest_edge_prev_frame_bulk_index;
				}
			}

			if (closest_edge_frame != 0
				    // TODO: make it possible to set the very end as the run point.
				    && closest_edge_prev_frame_bulk_index + 1 < input.frame_bulks.size()
				    && hw.tas_editor_set_run_point_and_save) {
				auto commands = input.frame_bulks[0].Commands;
				if (commands.empty())
					commands = "stop;bxt_timer_stop;pause;bxt_tas_editor 1";
				input.frame_bulks[closest_edge_prev_frame_bulk_index + 1].Commands = commands;
				input.frame_bulks[0].Commands.clear();
				hw.SaveEditedInput();
				return;
			}

			if (closest_frame != 0 && hw.tas_editor_insert_point) {
				size_t frame_bulk_index = 0;
				for (size_t i = 0; i < frame_bulk_starts.size(); ++i) {
					if (closest_frame > frame_bulk_starts[i])
						frame_bulk_index = i;
				}

				auto split_at_frames = closest_frame - frame_bulk_starts[frame_bulk_index];
				auto& frame_bulk = input.frame_bulks[frame_bulk_index];

				if (split_at_frames == 0)
					split_at_frames = 1;
				if (split_at_frames == frame_bulk.GetRepeats())
					split_at_frames -= 1;

				if (split_at_frames != 0) {
					auto new_frame_bulk_repeats = frame_bulk.GetRepeats() - split_at_frames;
					input.set_repeats(frame_bulk_index, split_at_frames);
					auto new_frame_bulk = frame_bulk;
					new_frame_bulk.Commands.clear(); // So pause;bxt_tas_editor 1 doesn't copy over.
					new_frame_bulk.Comments.clear();
					new_frame_bulk.SetRepeats(new_frame_bulk_repeats);
					input.frame_bulks.insert(input.frame_bulks.begin() + frame_bulk_index + 1, new_frame_bulk);
				}
			}

			if (closest_edge_frame != 0 && hw.tas_editor_delete_point) {
				input.frame_bulks.erase(input.frame_bulks.begin() + closest_edge_prev_frame_bulk_index);
				stale_index = closest_edge_prev_frame_bulk_index;
			}
		}

		input.mark_as_stale(stale_index);
	}

	void ResetTASEditorCommands()
	{
		auto& hw = HwDLL::GetInstance();
		hw.tas_editor_delete_point = false;
		hw.tas_editor_insert_point = false;
		hw.tas_editor_toggle_s03 = false;
		hw.tas_editor_toggle_s13 = false;
		hw.tas_editor_toggle_s22 = false;
		hw.tas_editor_toggle_lgagst = false;
		hw.tas_editor_toggle_autojump = false;
		hw.tas_editor_toggle_ducktap = false;
		hw.tas_editor_toggle_jumpbug = false;
		hw.tas_editor_toggle_dbc = false;
		hw.tas_editor_toggle_dbc_ceilings = false;
		hw.tas_editor_toggle_dbg = false;
		hw.tas_editor_toggle_dwj = false;
		hw.tas_editor_toggle_forward = false;
		hw.tas_editor_toggle_left = false;
		hw.tas_editor_toggle_right = false;
		hw.tas_editor_toggle_back = false;
		hw.tas_editor_toggle_up = false;
		hw.tas_editor_toggle_down = false;
		hw.tas_editor_toggle_jump = false;
		hw.tas_editor_toggle_duck = false;
		hw.tas_editor_toggle_use = false;
		hw.tas_editor_toggle_attack1 = false;
		hw.tas_editor_toggle_attack2 = false;
		hw.tas_editor_toggle_reload = false;
		hw.tas_editor_set_yaw = false;
		hw.tas_editor_set_pitch = false;
		hw.tas_editor_set_repeats = false;
		hw.tas_editor_set_commands = false;
		hw.tas_editor_unset_yaw = false;
		hw.tas_editor_unset_pitch = false;
		hw.tas_editor_set_run_point_and_save = false;
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
		DrawDisplacerTargets(pTriAPI);
		DrawUseableEntities(pTriAPI);
		DrawTriggers(pTriAPI);
		DrawCustomTriggers(pTriAPI);
		DrawAbsMinMax(pTriAPI);

		DrawTASEditor(pTriAPI);
		ResetTASEditorCommands();
	}
}
