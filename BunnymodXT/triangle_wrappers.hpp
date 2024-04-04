#pragma once

#include "stdafx.hpp"
#include "modules.hpp"
#include <GL/gl.h>

namespace TriangleWrappers
{
	void RenderMode(int mode); // tri_GL_RenderMode
	void Begin(int primitiveCode); // tri_GL_Begin
	void End(); // tri_GL_End
	void Color4f(float x, float y, float z, float w); // tri_GL_Color4f
	void Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // tri_GL_Color4ub
	void TexCoord2f(float u, float v); // tri_GL_TexCoord2f
	void Vertex3fv(float *worldPnt); // tri_GL_Vertex3fv
	void Vertex3f(float x, float y, float z); // tri_GL_Vertex3f
	void Brightness(float x); // tri_GL_Brightness
	void CullFace(TRICULLSTYLE style); // tri_GL_CullFace
	int SpriteTexture(struct model_s *pSpriteModel, int frame); // R_TriangleSpriteTexture
	int WorldToScreen(vec_t *point, vec_t *screen); // tri_ScreenTransform
	void ScreenToWorld(vec_t *screen, vec_t *point); // tri_WorldTransform
	void GetMatrix(const int pname, float *matrix); // tri_GetMatrix

	int InvertMatrix(const float *m, float *out); // InvertMatrix 
}