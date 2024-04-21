/*
	This is reverse-engineered TriAPI functions from the 'hardware' engine.
	Despite the fact that the full Linux 'software' engine is available starting with the 25th anniversary update,
	there is no point in reversing from it the TriAPI functions, because it is much harder to support 
	and most peoples play on the 'hardware' engine in fact.
*/

#include "triangle_wrappers.hpp"

namespace TriangleWrappers
{
	auto &hw = HwDLL::GetInstance();
	auto &cl = ClientDLL::GetInstance();

	static const int g_GL_Modes[7] = {4, 6, 7, 9, 1, 5, 8}; // GL_TRIANGLES, GL_TRIANGLE_FAN, GL_QUADS, GL_POLYGON, GL_LINES, GL_TRIANGLE_STRIP, GL_QUAD_STRIP
	static int gRenderMode;
	static float gGlR, gGlG, gGlB, gGlW;

	void RenderMode(int mode)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->RenderMode(mode);
		}
		else
		{
			switch (mode)
			{
				case kRenderNormal:
					glDisable(GL_BLEND);
					glDepthMask(GL_TRUE);
					glShadeModel(GL_FLAT);
					gRenderMode = mode;
					break;
				case kRenderTransColor:
				case kRenderTransTexture:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glEnable(GL_BLEND);
					glShadeModel(GL_SMOOTH);
					gRenderMode = mode;
					break;
				case kRenderTransAlpha:
					glEnable(GL_BLEND);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glShadeModel(GL_SMOOTH);
					glDepthMask(GL_FALSE);
					gRenderMode = mode;
					break;
				case kRenderTransAdd:
					glBlendFunc(GL_ONE, GL_ONE);
					glEnable(GL_BLEND);
					glDepthMask(GL_FALSE);
					glShadeModel(GL_SMOOTH);
					gRenderMode = mode;
					break;
				default:
					gRenderMode = mode;
					break;
			}
		}
	}

	void Begin(int primitiveCode)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->Begin(primitiveCode);
		}
		else
		{
			if (hw.ORIG_VGUI2_ResetCurrentTexture)
				hw.ORIG_VGUI2_ResetCurrentTexture();
			glBegin(g_GL_Modes[primitiveCode]);
		}
	}

	void End()
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->End();
		}
		else
		{
			glEnd();
		}
	}

	void Color4f(float x, float y, float z, float w)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->Color4f(x, y, z, w);
		}
		else
		{
			if (gRenderMode == kRenderTransAlpha)
			{
				glColor4ub(static_cast<GLubyte>(x * 255.9), static_cast<GLubyte>(y * 255.9), static_cast<GLubyte>(z * 255.9), static_cast<GLubyte>(w * 255.0));
			}
			else
			{
				glColor4f(x * w, y * w, z * w, 1.0f);
			}
			gGlR = x;
			gGlG = y;
			gGlB = z;
			gGlW = w;
		}
	}

	void Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->Color4ub(r, g, b, a);
		}
		else
		{
			gGlR = r * (1.0f / 255.0f);
			gGlG = g * (1.0f / 255.0f);
			gGlB = b * (1.0f / 255.0f);
			gGlW = a * (1.0f / 255.0f);
			glColor4f(gGlR, gGlG, gGlB, 1.0f);
		}
	}

	void TexCoord2f(float u, float v)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->TexCoord2f(u, v);
		}
		else
		{
			glTexCoord2f(u, v);
		}
	}

	void Vertex3fv(float *worldPnt)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->Vertex3fv(worldPnt);
		}
		else
		{
			glVertex3fv(worldPnt);
		}
	}

	void Vertex3f(float x, float y, float z)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->Vertex3f(x, y, z);
		}
		else
		{
			glVertex3f(x, y, z);
		}
	}

	void Brightness(float x)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->Brightness(x);
		}
		else
		{
			glColor4f(gGlR * x * gGlW, gGlG * x * gGlW, gGlB * x * gGlW, 1.0f);
		}
	}

	void CullFace(TRICULLSTYLE style)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->CullFace(style);
		}
		else
		{
			if (style == TRI_FRONT)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
			}
			else if (style == TRI_NONE)
			{
				glDisable(GL_CULL_FACE);
			}
		}
	}

	int SpriteTexture(struct model_s *pSpriteModel, int frame)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			return cl.pEngfuncs->pTriAPI->SpriteTexture(pSpriteModel, frame);
		}
		else if (hw.ORIG_R_GetSpriteFrame && hw.ORIG_GL_Bind)
		{
			if (hw.ORIG_VGUI2_ResetCurrentTexture)
				hw.ORIG_VGUI2_ResetCurrentTexture();
			mspriteframe_t *spriteframe = hw.ORIG_R_GetSpriteFrame((msprite_t*)pSpriteModel->cache.data, frame);

			if (spriteframe)
			{
				hw.ORIG_GL_Bind(spriteframe->gl_texturenum);
				return 1;
			}
		}

		return 0;
	}

	int WorldToScreen(vec_t *point, vec_t *screen)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			return cl.pEngfuncs->pTriAPI->WorldToScreen(point, screen);
		}
		else if (hw.gWorldToScreen)
		{
			auto gWorldToScreen = (*hw.gWorldToScreen);

			float x = gWorldToScreen[0] * point[0] + gWorldToScreen[4] * point[1] + gWorldToScreen[8] * point[2] + gWorldToScreen[12];
			float y = gWorldToScreen[1] * point[0] + gWorldToScreen[5] * point[1] + gWorldToScreen[9] * point[2] + gWorldToScreen[13];
			float w = gWorldToScreen[3] * point[0] + gWorldToScreen[7] * point[1] + gWorldToScreen[11] * point[2] + gWorldToScreen[15];

			float invw = 0.0f;
			if (w != 0.0f)
			{
				invw = 1.0f / w;
				screen[0] = x * invw;
				screen[1] = y * invw;
			}

			return invw <= 0.0f;
		}

		return 0;
	}

	void ScreenToWorld(vec_t *screen, vec_t *point)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->ScreenToWorld(screen, point);
		}
		else if (hw.gScreenToWorld)
		{
			auto gScreenToWorld = hw.gScreenToWorld;

			float x = gScreenToWorld[0] * screen[0] + gScreenToWorld[4] * screen[1] + gScreenToWorld[8] * screen[2] + gScreenToWorld[12];
			float y = gScreenToWorld[1] * screen[0] + gScreenToWorld[5] * screen[1] + gScreenToWorld[9] * screen[2] + gScreenToWorld[13];
			float z = gScreenToWorld[2] * screen[0] + gScreenToWorld[6] * screen[1] + gScreenToWorld[10] * screen[2] + gScreenToWorld[14];
			float w = gScreenToWorld[3] * screen[0] + gScreenToWorld[7] * screen[1] + gScreenToWorld[11] * screen[2] + gScreenToWorld[15];

			if (w != 0.0f)
			{
				float invw = 1.0f / w;
				point[0] = x * invw;
				point[1] = y * invw;
				point[2] = z * invw;
			}
		}
	}

	void GetMatrix(const int pname, float *matrix)
	{
		if (cl.interface_preserved_eng_cl && cl.pEngfuncs && !hw.is_hlsdk10)
		{
			cl.pEngfuncs->pTriAPI->GetMatrix(pname, matrix);
		}
		else
		{
			glGetFloatv(pname, matrix);
		}
	}

	// https://gitlab.freedesktop.org/mesa/mesa/-/blob/b3aefd1cfb6aacd1695c52911dd39da50d893ece/src/mesa/math/m_matrix.c#L358
	// This function is copied from the Mesa repository, but the code is apparently no different from what the GoldSrc engine uses
	int InvertMatrix(const float *m, float *out)
	{
	#define SWAP_ROWS(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
	#define MAT(m,r,c) (m)[(c)*4+(r)]

		float wtmp[4][8];
		float m0, m1, m2, m3, s;
		float *r0, *r1, *r2, *r3;

		r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

		r0[0] = MAT(m,0,0), r0[1] = MAT(m,0,1),
		r0[2] = MAT(m,0,2), r0[3] = MAT(m,0,3),
		r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,

		r1[0] = MAT(m,1,0), r1[1] = MAT(m,1,1),
		r1[2] = MAT(m,1,2), r1[3] = MAT(m,1,3),
		r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,

		r2[0] = MAT(m,2,0), r2[1] = MAT(m,2,1),
		r2[2] = MAT(m,2,2), r2[3] = MAT(m,2,3),
		r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,

		r3[0] = MAT(m,3,0), r3[1] = MAT(m,3,1),
		r3[2] = MAT(m,3,2), r3[3] = MAT(m,3,3),
		r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

		/* choose pivot - or die */
		if (fabs(r3[0])>fabs(r2[0])) SWAP_ROWS(r3, r2);
		if (fabs(r2[0])>fabs(r1[0])) SWAP_ROWS(r2, r1);
		if (fabs(r1[0])>fabs(r0[0])) SWAP_ROWS(r1, r0);
		if (0.0 == r0[0]) return 0;

		/* eliminate first variable */
		m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
		s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
		s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
		s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
		s = r0[4];
		if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
		s = r0[5];
		if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
		s = r0[6];
		if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
		s = r0[7];
		if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

		/* choose pivot - or die */
		if (fabs(r3[1])>fabs(r2[1])) SWAP_ROWS(r3, r2);
		if (fabs(r2[1])>fabs(r1[1])) SWAP_ROWS(r2, r1);
		if (0.0 == r1[1]) return 0;

		/* eliminate second variable */
		m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
		r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
		r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
		s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
		s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
		s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
		s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

		/* choose pivot - or die */
		if (fabs(r3[2])>fabs(r2[2])) SWAP_ROWS(r3, r2);
		if (0.0 == r2[2]) return 0;

		/* eliminate third variable */
		m3 = r3[2]/r2[2];
		r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
		r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
		r3[7] -= m3 * r2[7];

		/* last check */
		if (0.0 == r3[3]) return 0;

		s = 1.0f/r3[3];              /* now back substitute row 3 */
		r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

		m2 = r2[3];                 /* now back substitute row 2 */
		s = 1.0f/r2[2];
		r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
		r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
		m1 = r1[3];
		r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
		r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
		m0 = r0[3];
		r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
		r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

		m1 = r1[2];                 /* now back substitute row 1 */
		s = 1.0f/r1[1];
		r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
		r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
		m0 = r0[2];
		r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
		r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

		m0 = r0[1];                 /* now back substitute row 0 */
		s = 1.0f/r0[0];
		r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
		r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

		MAT(out,0,0) = r0[4]; MAT(out,0,1) = r0[5],
		MAT(out,0,2) = r0[6]; MAT(out,0,3) = r0[7],
		MAT(out,1,0) = r1[4]; MAT(out,1,1) = r1[5],
		MAT(out,1,2) = r1[6]; MAT(out,1,3) = r1[7],
		MAT(out,2,0) = r2[4]; MAT(out,2,1) = r2[5],
		MAT(out,2,2) = r2[6]; MAT(out,2,3) = r2[7],
		MAT(out,3,0) = r3[4]; MAT(out,3,1) = r3[5],
		MAT(out,3,2) = r3[6]; MAT(out,3,3) = r3[7];

		return 1;

	#undef MAT
	#undef SWAP_ROWS
	}
};
