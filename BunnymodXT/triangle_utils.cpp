#include "stdafx.hpp"

#include "hud_custom.hpp"
#include "triangle_utils.hpp"

namespace TriangleUtils
{
	float PixelWidthToProportion(float pixels)
	{
		return pixels / CustomHud::GetScreenInfo().iWidth;
	}

	float PixelHeightToProportion(float pixels)
	{
		return pixels / CustomHud::GetScreenInfo().iHeight;
	}

	float ProportionToPixelWidth(float proportion)
	{
		return proportion * CustomHud::GetScreenInfo().iWidth;
	}

	float ProportionToPixelHeight(float proportion)
	{
		return proportion * CustomHud::GetScreenInfo().iHeight;
	}

	void DrawPyramid(triangleapi_s *pTriAPI, Vector origin, float width, float height)
	{
		const float halfWidth = width * 0.5f;
		Vector bottom[5] = {
			origin + Vector(halfWidth, halfWidth, 0),
			origin + Vector(halfWidth, -halfWidth, 0),
			origin + Vector(-halfWidth, -halfWidth, 0),
			origin + Vector(-halfWidth, halfWidth, 0),
			origin + Vector(halfWidth, halfWidth, 0)
		};

		pTriAPI->Begin(TRI_QUADS);
		for (int i = 0; i < 4; ++i)
			pTriAPI->Vertex3f(bottom[i].x, bottom[i].y, bottom[i].z);
		pTriAPI->End();

		pTriAPI->Begin(TRI_TRIANGLE_FAN);
		pTriAPI->Vertex3f(origin.x, origin.y, origin.z + height);
		for (int i = 0; i < 5; ++i)
			pTriAPI->Vertex3f(bottom[i].x, bottom[i].y, bottom[i].z);
		pTriAPI->End();
	}

	void DrawScreenTriangle(triangleapi_s *pTriAPI, Vector origin, float sideLength)
	{
		// The magic number is sqrt(3) / 6
		const auto baseToCenter = 0.288675135f * sideLength;
		const auto centerToTop = baseToCenter + baseToCenter;
		const auto halfLength = 0.5f * sideLength;

		Vector screenPoints[3];
		pTriAPI->WorldToScreen(origin, screenPoints[0]);
		screenPoints[0].z = 0.0f;
		screenPoints[1] = screenPoints[0];
		screenPoints[2] = screenPoints[0];

		screenPoints[0].y += centerToTop;
		screenPoints[1].x -= halfLength;
		screenPoints[1].y -= baseToCenter;
		screenPoints[2].x += halfLength;
		screenPoints[2].y -= baseToCenter;

		pTriAPI->Begin(TRI_TRIANGLES);
		for (int i = 0; i < 3; ++i) {
			Vector worldPoint;
			pTriAPI->ScreenToWorld(screenPoints[i], worldPoint);
			pTriAPI->Vertex3f(worldPoint.x, worldPoint.y, worldPoint.z);
		}
		pTriAPI->End();
	}

	void DrawScreenRectangle(triangleapi_s *pTriAPI, Vector2D corner1, Vector2D corner2)
	{
		Vector screen_points[4], world_points[4];
		screen_points[0] = Vector(corner1.x, corner1.y, 0.0f);
		screen_points[1] = Vector(corner1.x, corner2.y, 0.0f);
		screen_points[2] = Vector(corner2.x, corner2.y, 0.0f);
		screen_points[3] = Vector(corner2.x, corner1.y, 0.0f);

		for (int i = 0; i < 4; ++i)
			pTriAPI->ScreenToWorld(screen_points[i], world_points[i]);

		pTriAPI->Begin(TRI_QUADS);

		pTriAPI->Vertex3fv(world_points[0]);
		pTriAPI->Vertex3fv(world_points[1]);
		pTriAPI->Vertex3fv(world_points[2]);
		pTriAPI->Vertex3fv(world_points[3]);

		pTriAPI->End();
	}

	void DrawAACuboid(triangleapi_s *pTriAPI, Vector corner1, Vector corner2)
	{
		pTriAPI->Begin(TRI_QUADS);

		pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);

		pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);

		pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);

		pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);

		pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);

		pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);

		pTriAPI->End();
	}

	void DrawAACuboidWireframe(triangleapi_s *pTriAPI, Vector corner1, Vector corner2)
	{
		pTriAPI->Begin(TRI_LINES);

		pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);

		pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);

		pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);

		pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);

		pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);

		pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);
		pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);

		pTriAPI->End();
	}

	void DrawLine(triangleapi_s *pTriAPI, Vector start, Vector end)
	{
		pTriAPI->Begin(TRI_LINES);
		pTriAPI->Vertex3f(start.x, start.y, start.z);
		pTriAPI->Vertex3f(end.x, end.y, end.z);
		pTriAPI->End();
	}

	void DrawSphere(triangleapi_s* pTriAPI, Vector origin, float r, int nlat, int nlong) {
		for (int i = 0; i <= nlat; i++) {
			const float lat0 = (-0.5f + static_cast<float>(i - 1) / nlat) * static_cast<float>(M_PI);
			const float z0 = sinf(lat0);
			const float zr0 = cosf(lat0);

			const float lat1 = (-0.5f + static_cast<float>(i) / nlat) * static_cast<float>(M_PI);
			const float z1 = sinf(lat1);
			const float zr1 = cosf(lat1);

			pTriAPI->Begin(TRI_QUAD_STRIP);
			for (int j = 0; j <= nlong; j++) {
				const float lng = static_cast<float>(j - 1) / nlong * 2.0f * static_cast<float>(M_PI);
				const float x = cosf(lng);
				const float y = sinf(lng);

				pTriAPI->Vertex3f(r * x * zr0 + origin.x, r * y * zr0 + origin.y, r * z0 + origin.z);
				pTriAPI->Vertex3f(r * x * zr1 + origin.x, r * y * zr1 + origin.y, r * z1 + origin.z);
			}
			pTriAPI->End();
		}
	}

};
