#include "stdafx.hpp"

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

	static void DrawCrosshair(triangleapi_s *pTriAPI)
	{
		if (!CVars::bxt_crosshair.GetBool())
			return;

		constexpr float PROP_SCALE = 1.0f / 125;
		constexpr float DEFAULT_GAP = 2;

		const float thickness = CVars::cl_crosshairthickness.GetFloat();
		const float size = CVars::cl_crosshairsize.GetFloat();
		const float gap = DEFAULT_GAP + CVars::cl_crosshairgap.GetFloat();

		pTriAPI->RenderMode(kRenderTransAlpha);
		pTriAPI->CullFace(TRI_NONE);
		pTriAPI->Color4f(
			CVars::cl_crosshaircolor_r.GetFloat() / 255,
			CVars::cl_crosshaircolor_g.GetFloat() / 255,
			CVars::cl_crosshaircolor_b.GetFloat() / 255,
			CVars::cl_crosshairalpha.GetFloat() / 255);

		const auto si = CustomHud::GetScreenInfo();
		const auto minResolution = std::min(si.iHeight, si.iWidth);

		auto c1 = Vector2D(0.5 * thickness + gap, 0.5 * thickness) * PROP_SCALE * minResolution;
		auto c2 = Vector2D(0.5 * thickness + gap + size, -0.5 * thickness) * PROP_SCALE * minResolution;
		for (int i = 0; i < 4; ++i) {
			const auto sc1 = Vector2D(TriangleUtils::PixelWidthToProportion(c1.x), TriangleUtils::PixelHeightToProportion(c1.y));
			const auto sc2 = Vector2D(TriangleUtils::PixelWidthToProportion(c2.x), TriangleUtils::PixelHeightToProportion(c2.y));
			TriangleUtils::DrawScreenRectangle(pTriAPI, sc1, sc2);
			c1 = Vector2D(-c1.y, c1.x);
			c2 = Vector2D(-c2.y, c2.x);
		}

		if (CVars::cl_crosshairdot.GetBool()) {
			c1 = Vector2D(thickness, thickness) * 0.5 * PROP_SCALE * minResolution;
			c2 = Vector2D(thickness, thickness) * -0.5 * PROP_SCALE * minResolution;
			c1.x = TriangleUtils::PixelWidthToProportion(c1.x);
			c1.y = TriangleUtils::PixelHeightToProportion(c1.y);
			c2.x = TriangleUtils::PixelWidthToProportion(c2.x);
			c2.y = TriangleUtils::PixelHeightToProportion(c2.y);
			TriangleUtils::DrawScreenRectangle(pTriAPI, c1, c2);
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
		DrawCrosshair(pTriAPI);
	}
}
