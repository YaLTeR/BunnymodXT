#include "stdafx.hpp"

#include "triangle_utils.hpp"

namespace TriangleUtils
{
	void CreatePyramid(triangleapi_s *pTriAPI, Vector origin, float width, float height)
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

	void CreateScreenTriangle(triangleapi_s *pTriAPI, Vector origin, float sideLength)
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
}
