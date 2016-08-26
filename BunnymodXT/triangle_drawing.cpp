#include "stdafx.hpp"

#include "triangle_drawing.hpp"
#include "triangle_utils.hpp"
#include "modules.hpp"

namespace TriangleDrawing
{
	static HSPRITE_HL white_sprite = 0;

	static void DrawNodes()
	{
		auto pEngfuncs = ClientDLL::GetInstance().pEngfuncs;

		if (CVars::bxt_show_nodes.GetBool()) {
			pEngfuncs->pTriAPI->RenderMode(kRenderTransAdd);

			pEngfuncs->pTriAPI->CullFace(TRI_NONE);
			pEngfuncs->pTriAPI->Color4f(0.722f, 0.0f, 0.341f, 1.0f);
			for (const Vector *position : ServerDLL::GetInstance().GetNodePositions()) {
				TriangleUtils::CreatePyramid(pEngfuncs->pTriAPI, *position, 10, 30);
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

		if (white_sprite == 0)
			return;

		if (!pEngfuncs->pTriAPI->SpriteTexture(const_cast<model_s*>(pEngfuncs->GetSpritePointer(white_sprite)), 0))
			return;

		DrawNodes();
	}
}
