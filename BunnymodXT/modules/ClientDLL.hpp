#pragma once

#include "../sptlib-wrapper.hpp"
#include <SPTLib/IHookableNameFilter.hpp>
#include "../hud_custom.hpp"

class ClientDLL : public IHookableNameFilter
{
	HOOK_DECL(void, __cdecl, PM_Jump)
	HOOK_DECL(void, __cdecl, PM_PlayerMove, qboolean server)
	HOOK_DECL(void, __cdecl, PM_PreventMegaBunnyJumping)
	HOOK_DECL(int, __cdecl, PM_ClipVelocity, float in[3], float normal[3], float out[3], float overbounce)
	HOOK_DECL(void, __cdecl, PM_WaterMove)
	HOOK_DECL(void, __cdecl, PM_Move, struct playermove_s* ppmove, int server)
	HOOK_DECL(void, __cdecl, V_CalcRefdef, ref_params_t* pparams)
	HOOK_DECL(void, __cdecl, HUD_Init)
	HOOK_DECL(void, __cdecl, HUD_VidInit)
	HOOK_DECL(void, __cdecl, HUD_Reset)
	HOOK_DECL(void, __cdecl, HUD_Redraw, float time, int intermission)
	HOOK_DECL(void, __cdecl, HUD_PostRunCmd, local_state_s* from, local_state_s* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
	HOOK_DECL(void, __cdecl, HUD_Frame, double time)
	HOOK_DECL(void, __cdecl, HUD_DrawTransparentTriangles)
	HOOK_DECL(int, __cdecl, HUD_Key_Event, int down, int keynum, const char* pszCurrentBinding)
	HOOK_DECL(int, __cdecl, HUD_UpdateClientData, client_data_t* pcldata, float flTime)
	HOOK_DECL(void, __fastcall, StudioCalcAttachments, void* thisptr)
	HOOK_DECL(void, __cdecl, StudioCalcAttachments_Linux, void* thisptr)
	HOOK_DECL(void, __cdecl, VectorTransform, float *in1, float *in2, float *out)
	HOOK_DECL(void, __cdecl, EV_GetDefaultShellInfo, event_args_t *args, float *origin, float *velocity, float *ShellVelocity, float *ShellOrigin,
	          float *forward, float *right, float *up, float forwardScale, float upScale, float rightScale)
	HOOK_DECL(void, __fastcall, CStudioModelRenderer__StudioSetupBones, void* thisptr)
	HOOK_DECL(void, __cdecl, CStudioModelRenderer__StudioSetupBones_Linux, void* thisptr)
	HOOK_DECL(int, __cdecl, HUD_AddEntity, int type, cl_entity_s *ent, char *modelname)
	HOOK_DECL(int, __cdecl, CL_IsThirdPerson)
	HOOK_DECL(void, __fastcall, CStudioModelRenderer__StudioRenderModel, void* thisptr)
	HOOK_DECL(void, __cdecl, CStudioModelRenderer__StudioRenderModel_Linux, void* thisptr)
	HOOK_DECL(void, __cdecl, ScaleColors, int* r, int* g, int* b, int a)
	HOOK_DECL(int, __fastcall, HistoryResource__DrawAmmoHistory, void *thisptr, int edx, float flTime)
	HOOK_DECL(int, __cdecl, HistoryResource__DrawAmmoHistory_Linux, void *thisptr, float flTime)
	HOOK_DECL(int, __fastcall, CHudHealth__DrawDamage, void *thisptr, int edx, float flTime)
	HOOK_DECL(int, __cdecl, CHudHealth__DrawDamage_Linux, void *thisptr, float flTime)
	HOOK_DECL(int, __fastcall, CHudHealth__DrawPain, void *thisptr, int edx, float flTime)
	HOOK_DECL(int, __cdecl, CHudHealth__DrawPain_Linux, void *thisptr, float flTime)
	HOOK_DECL(void, __fastcall, CHudFlashlight__drawNightVision, void* thisptr)
	HOOK_DECL(void, __cdecl, CHudFlashlight__drawNightVision_Linux, void* thisptr)
	HOOK_DECL(bool, __fastcall, CHud__DrawHudNightVision, void *thisptr, int edx, float flTime)
	HOOK_DECL(bool, __cdecl, CHud__DrawHudNightVision_Linux, void *thisptr, float flTime)

public:
	static ClientDLL& GetInstance()
	{
		static ClientDLL instance;
		return instance;
	}

	virtual void Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept);
	virtual void Unhook();
	virtual void Clear();

	void ResetSeedsQueued() { SeedsQueued = 0; }

	inline bool IsInsideKeyEvent() const { return insideKeyEvent; }

	cl_enginefunc_t *pEngfuncs;

	Vector last_vieworg;
	Vector last_viewangles;
	Vector last_viewforward;
	Vector last_viewright;
	Vector last_viewup;

	void StudioAdjustViewmodelAttachments(Vector &vOrigin);

	bool DoesGameDirMatch(const char *game, bool substr);

	bool orig_forcehltv_found = false;
	bool orig_righthand_not_found = false;

	unsigned char custom_r, custom_g, custom_b;
	bool custom_hud_color_set = false;
	bool bxt_hud_color_set = false;
	bool bxt_hud_color_fill = false;

	bool insideDrawAmmoHistory = false;
	bool insideDrawHealthDamage = false;
	bool insideDrawHealthPain = false;
	bool insideDrawNightVision = false;

	unsigned short last_buttons;

	// When set to false, the mouse won't move the camera.
	void SetMouseState(bool active);

	Vector AnglesToForward(const Vector &angles);

	void SetAngleSpeedCap(bool capped);

	void SetSpeedScaling(bool scaled);

	void FileBase(const char *in, char *out);

private:
	ClientDLL() : IHookableNameFilter({ L"client.dll", L"client.so" }) {};
	ClientDLL(const ClientDLL&);
	void operator=(const ClientDLL&);

protected:
	typedef void(__cdecl *_IN_ActivateMouse) ();
	_IN_ActivateMouse ORIG_IN_ActivateMouse;
	typedef void(__cdecl *_IN_DeactivateMouse) ();
	_IN_DeactivateMouse ORIG_IN_DeactivateMouse;

	void FindStuff();
	bool FindHUDFunctions();
	void RegisterCVarsAndCommands();
	void AddHudElem(void* pHudElem);

	void **ppmove;
	ptrdiff_t offOldbuttons;
	ptrdiff_t offOnground;
	ptrdiff_t offIUser1;

	ptrdiff_t offBhopcap;
	ptrdiff_t pBhopcapWindows;
	byte originalBhopcapInsn[6];

	ptrdiff_t pCS_AngleSpeedCap;
	ptrdiff_t pCS_AngleSpeedCap_Linux;

	ptrdiff_t pCS_SpeedScaling;
	ptrdiff_t pCS_SpeedScaling_Linux;

	bool cantJumpNextTime;

	unsigned SeedsQueued;

	bool insideKeyEvent;
	bool insideStudioCalcAttachmentsViewmodel;
};
