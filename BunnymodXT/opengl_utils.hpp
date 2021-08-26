#pragma once
#include <vector>

class GLUtils {
public:
	GLUtils();
	~GLUtils();

	void color(float r, float g, float b, float a) const;
	void color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) const;
	void line_width(float width) const;
	void line(const Vector2D& start, const Vector2D& end) const;
	void circle(const Vector2D& center, const std::vector<Vector2D>& points) const;
	void rectangle(const Vector2D& corner_a, const Vector2D& corner_b) const;

	static std::vector<Vector2D> compute_circle(float radius);
};