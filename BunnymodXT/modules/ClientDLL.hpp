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

	unsigned short last_buttons;

	// When set to false, the mouse won't move the camera.
	void SetMouseState(bool active);

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

	ptrdiff_t offBhopcap;
	ptrdiff_t pBhopcapWindows;
	byte originalBhopcapInsn[6];

	bool cantJumpNextTime;

	unsigned SeedsQueued;

	bool insideKeyEvent;
	bool insideStudioCalcAttachmentsViewmodel;
};
