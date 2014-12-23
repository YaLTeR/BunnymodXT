#pragma once

#include "../sptlib-wrapper.hpp"
#include <SPTLib/IHookableNameFilter.hpp>
#include "../hud_custom.hpp"

typedef void(__cdecl *_PM_Jump) ();
typedef void(__cdecl *_PM_PlayerMove) (qboolean server);
typedef void(__cdecl *_PM_PreventMegaBunnyJumping) ();
typedef int(__cdecl *_Initialize) (cl_enginefunc_t* pEnginefuncs, int iVersion);
typedef void(__cdecl *_V_CalcRefdef) (ref_params_t* pparams);
typedef void(__cdecl *_HUD_Init) ();
typedef void(__cdecl *_HUD_VidInit) ();
typedef void(__cdecl *_HUD_Reset) ();
typedef void(__cdecl *_HUD_Redraw) (float time, int intermission);

class ClientDLL : public IHookableNameFilter
{
public:
	ClientDLL() : IHookableNameFilter({ L"client.dll", L"client.so" }) {};
	virtual void Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept);
	virtual void Unhook();
	virtual void Clear();

	static void __cdecl HOOKED_PM_Jump();
	void __cdecl HOOKED_PM_Jump_Func();
	static void __cdecl HOOKED_PM_PlayerMove(qboolean server); // Needed only for the Linux hook.
	void __cdecl HOOKED_PM_PlayerMove_Func(qboolean server);
	static void __cdecl HOOKED_PM_PreventMegaBunnyJumping();
	void __cdecl HOOKED_PM_PreventMegaBunnyJumping_Func();
	static int __cdecl HOOKED_Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion);
	int __cdecl HOOKED_Initialize_Func(cl_enginefunc_t* pEnginefuncs, int iVersion);
	static void __cdecl HOOKED_V_CalcRefdef(ref_params_t* pparams);
	void __cdecl HOOKED_V_CalcRefdef_Func(ref_params_t* pparams);
	static void __cdecl HOOKED_HUD_Init();
	void __cdecl HOOKED_HUD_Init_Func();
	static void __cdecl HOOKED_HUD_VidInit();
	void __cdecl HOOKED_HUD_VidInit_Func();
	static void __cdecl HOOKED_HUD_Reset();
	void __cdecl HOOKED_HUD_Reset_Func();
	static void __cdecl HOOKED_HUD_Redraw(float time, int intermission);
	void __cdecl HOOKED_HUD_Redraw_Func(float time, int intermission);

	bool FindHUDFunctions();
	void RegisterCVarsAndCommands();
	void AddHudElem(void* pHudElem);

	cl_enginefunc_t *pEngfuncs;

protected:
	_PM_Jump ORIG_PM_Jump;
	_PM_PlayerMove ORIG_PM_PlayerMove;
	_PM_PreventMegaBunnyJumping ORIG_PM_PreventMegaBunnyJumping;
	_Initialize ORIG_Initialize;
	_V_CalcRefdef ORIG_V_CalcRefdef;
	_HUD_Init ORIG_HUD_Init;
	_HUD_VidInit ORIG_HUD_VidInit;
	_HUD_Reset ORIG_HUD_Reset;
	_HUD_Redraw ORIG_HUD_Redraw;

	void **ppmove;
	ptrdiff_t offOldbuttons;
	ptrdiff_t offOnground;
	
	ptrdiff_t offBhopcap;
	byte originalBhopcapInsn[6];

	bool cantJumpNextTime;
};
