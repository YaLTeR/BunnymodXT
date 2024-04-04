#include "stdafx.hpp"

#include "hud_custom.hpp"
#include "triangle_utils.hpp"
#include "triangle_wrappers.hpp"

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

	void DrawPyramid(Vector origin, float width, float height)
	{
		const float halfWidth = width * 0.5f;
		Vector bottom[5] = {
			origin + Vector(halfWidth, halfWidth, 0),
			origin + Vector(halfWidth, -halfWidth, 0),
			origin + Vector(-halfWidth, -halfWidth, 0),
			origin + Vector(-halfWidth, halfWidth, 0),
			origin + Vector(halfWidth, halfWidth, 0)
		};

		TriangleWrappers::Begin(TRI_QUADS);
		for (int i = 0; i < 4; ++i)
			TriangleWrappers::Vertex3f(bottom[i].x, bottom[i].y, bottom[i].z);
		TriangleWrappers::End();

		TriangleWrappers::Begin(TRI_TRIANGLE_FAN);
		TriangleWrappers::Vertex3f(origin.x, origin.y, origin.z + height);
		for (int i = 0; i < 5; ++i)
			TriangleWrappers::Vertex3f(bottom[i].x, bottom[i].y, bottom[i].z);
		TriangleWrappers::End();
	}

	void DrawScreenTriangle(Vector origin, float sideLength)
	{
		// The magic number is sqrt(3) / 6
		const auto baseToCenter = 0.288675135f * sideLength;
		const auto centerToTop = baseToCenter + baseToCenter;
		const auto halfLength = 0.5f * sideLength;

		Vector screenPoints[3];
		TriangleWrappers::WorldToScreen(origin, screenPoints[0]);
		screenPoints[0].z = 0.0f;
		screenPoints[1] = screenPoints[0];
		screenPoints[2] = screenPoints[0];

		screenPoints[0].y += centerToTop;
		screenPoints[1].x -= halfLength;
		screenPoints[1].y -= baseToCenter;
		screenPoints[2].x += halfLength;
		screenPoints[2].y -= baseToCenter;

		TriangleWrappers::Begin(TRI_TRIANGLES);
		for (int i = 0; i < 3; ++i) {
			Vector worldPoint;
			TriangleWrappers::ScreenToWorld(screenPoints[i], worldPoint);
			TriangleWrappers::Vertex3f(worldPoint.x, worldPoint.y, worldPoint.z);
		}
		TriangleWrappers::End();
	}

	void DrawScreenRectangle(Vector2D corner1, Vector2D corner2)
	{
		Vector screen_points[4], world_points[4];
		screen_points[0] = Vector(corner1.x, corner1.y, 0.0f);
		screen_points[1] = Vector(corner1.x, corner2.y, 0.0f);
		screen_points[2] = Vector(corner2.x, corner2.y, 0.0f);
		screen_points[3] = Vector(corner2.x, corner1.y, 0.0f);

		for (int i = 0; i < 4; ++i)
			TriangleWrappers::ScreenToWorld(screen_points[i], world_points[i]);

		TriangleWrappers::Begin(TRI_QUADS);

		TriangleWrappers::Vertex3fv(world_points[0]);
		TriangleWrappers::Vertex3fv(world_points[1]);
		TriangleWrappers::Vertex3fv(world_points[2]);
		TriangleWrappers::Vertex3fv(world_points[3]);

		TriangleWrappers::End();
	}

	void DrawAACuboid(Vector corner1, Vector corner2)
	{
		TriangleWrappers::Begin(TRI_QUADS);

		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner1.z);

		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner1.z);

		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner2.z);

		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner2.z);

		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner1.z);

		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner2.z);

		TriangleWrappers::End();
	}

	void DrawAACuboidWireframe(Vector corner1, Vector corner2)
	{
		TriangleWrappers::Begin(TRI_LINES);

		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner1.z);

		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner1.z);

		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner1.z);

		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner2.z);

		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner1.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner2.z);

		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner1.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner1.x, corner2.y, corner2.z);
		TriangleWrappers::Vertex3f(corner2.x, corner2.y, corner2.z);

		TriangleWrappers::End();
	}

	void DrawLine(Vector start, Vector end)
	{
		TriangleWrappers::Begin(TRI_LINES);
		TriangleWrappers::Vertex3f(start.x, start.y, start.z);
		TriangleWrappers::Vertex3f(end.x, end.y, end.z);
		TriangleWrappers::End();
	}
};
