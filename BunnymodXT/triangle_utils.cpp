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
};