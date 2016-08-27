#pragma once

namespace TriangleUtils
{
	float PixelWidthToProportion(float pixels);
	float PixelHeightToProportion(float pixels);

	void CreatePyramid(triangleapi_s *pTriAPI, Vector origin, float width, float height);
	void CreateScreenTriangle(triangleapi_s *pTriAPI, Vector center, float sideLength);
	void DrawScreenRectangle(triangleapi_s *pTriAPI, Vector2D corner1, Vector2D corner2);

	// Draws an axis-aligned cuboid from two opposite corners.
	void DrawAACuboid(triangleapi_s *pTriAPI, Vector corner1, Vector corner2);
	void DrawAACuboidWireframe(triangleapi_s *pTriAPI, Vector corner1, Vector corner2);
};
