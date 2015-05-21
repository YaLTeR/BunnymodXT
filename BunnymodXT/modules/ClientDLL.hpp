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
	HOOK_DECL(void, __cdecl, V_CalcRefdef, ref_params_t* pparams)
	HOOK_DECL(void, __cdecl, HUD_Init)
	HOOK_DECL(void, __cdecl, HUD_VidInit)
	HOOK_DECL(void, __cdecl, HUD_Reset)
	HOOK_DECL(void, __cdecl, HUD_Redraw, float time, int intermission)
	HOOK_DECL(void, __cdecl, HUD_PostRunCmd, local_state_s* from, local_state_s* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
	HOOK_DECL(void, __cdecl, HUD_Frame, double time)

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

	void SetAngleSpeedCap(bool capped);

	cl_enginefunc_t *pEngfuncs;

private:
	ClientDLL() : IHookableNameFilter({ L"client.dll", L"client.so" }) {};
	ClientDLL(const ClientDLL&);
	void operator=(const ClientDLL&);

protected:
	void FindStuff();
	bool FindHUDFunctions();
	void RegisterCVarsAndCommands();
	void AddHudElem(void* pHudElem);

	void **ppmove;
	ptrdiff_t offOldbuttons;
	ptrdiff_t offOnground;
	
	ptrdiff_t offBhopcap;
	byte originalBhopcapInsn[6];

	uintptr_t angleSpeedCap;

	bool cantJumpNextTime;

	unsigned SeedsQueued;
};
