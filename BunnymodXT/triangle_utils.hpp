#pragma once

namespace TriangleUtils
{
	float PixelWidthToProportion(float pixels);
	float PixelHeightToProportion(float pixels);
	float ProportionToPixelWidth(float proportion);
	float ProportionToPixelHeight(float proportion);

	void DrawPyramid(Vector origin, float width, float height);
	void DrawScreenTriangle(Vector center, float sideLength);
	void DrawScreenRectangle(Vector2D corner1, Vector2D corner2);

	// Draws an axis-aligned cuboid from two opposite corners.
	void DrawAACuboid(Vector corner1, Vector corner2);
	void DrawAACuboidWireframe(Vector corner1, Vector corner2);

	void DrawLine(Vector start, Vector end);
};
