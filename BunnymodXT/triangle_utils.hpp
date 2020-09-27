#pragma once

namespace TriangleUtils
{
	float PixelWidthToProportion(float pixels);
	float PixelHeightToProportion(float pixels);
	float ProportionToPixelWidth(float proportion);
	float ProportionToPixelHeight(float proportion);

	void DrawPyramid(triangleapi_s *pTriAPI, Vector origin, float width, float height);
	void DrawScreenTriangle(triangleapi_s *pTriAPI, Vector center, float sideLength);
	void DrawScreenRectangle(triangleapi_s *pTriAPI, Vector2D corner1, Vector2D corner2);

	// Draws an axis-aligned cuboid from two opposite corners.
	void DrawAACuboid(triangleapi_s *pTriAPI, Vector corner1, Vector corner2);
	void DrawAACuboidWireframe(triangleapi_s *pTriAPI, Vector corner1, Vector corner2);

	void DrawLine(triangleapi_s *pTriAPI, Vector start, Vector end);
	void DrawSphere(triangleapi_s* pTriAPI, Vector origin, float r, int nlat, int nlong);
};
