#pragma once

#include "../sptlib-wrapper.hpp"
#include <SPTLib/IHookableNameFilter.hpp>
#include "../hud_custom.hpp"

typedef void(__cdecl *_PM_Jump) ();
typedef void(__cdecl *_PM_PlayerMove) (qboolean);
typedef void(__cdecl *_PM_PreventMegaBunnyJumping) ();
typedef int(__cdecl *_Initialize) (cl_enginefunc_t* pEnginefuncs, int iVersion);
typedef void(__cdecl *_V_CalcRefdef) (ref_params_t* pparams);
typedef void(__cdecl *_HUD_Init) ();
typedef void(__cdecl *_HUD_Redraw) (float time, int intermission);

#ifdef _WIN32
typedef void(__fastcall *_CHud_InitFunc) (void* thisptr, int edx); // For both CHud::Init and CHud::VidInit.
#else
typedef void(__cdecl *_CHud_InitFunc) (void* thisptr); // For both CHud::Init and CHud::VidInit.
#endif

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
	static void __cdecl HOOKED_HUD_Redraw(float time, int intermission);
	void __cdecl HOOKED_HUD_Redraw_Func(float time, int intermission);

	#ifdef _WIN32
	static void __fastcall HOOKED_CHud_Init(void* thisptr, int edx);
	void __fastcall HOOKED_CHud_Init_Func(void* thisptr, int edx);
	static void __fastcall HOOKED_CHud_VidInit(void* thisptr, int edx);
	void __fastcall HOOKED_CHud_VidInit_Func(void* thisptr, int edx);
	#else
	static void __cdecl HOOKED_CHud_Init(void* thisptr);
	void __cdecl HOOKED_CHud_Init_Func(void* thisptr);
	static void __cdecl HOOKED_CHud_VidInit(void* thisptr);
	void __cdecl HOOKED_CHud_VidInit_Func(void* thisptr);
	#endif

	void RegisterCVarsAndCommands();
	void AddHudElem(void* pHudElem);

	cl_enginefunc_t *pEngfuncs;

protected:
	_PM_Jump ORIG_PM_Jump;
	_PM_PlayerMove ORIG_PM_PlayerMove;
	_PM_PreventMegaBunnyJumping ORIG_PM_PreventMegaBunnyJumping;
	_Initialize ORIG_Initialize;
	_CHud_InitFunc ORIG_CHud_Init;
	_CHud_InitFunc ORIG_CHud_VidInit;
	_V_CalcRefdef ORIG_V_CalcRefdef;
	_HUD_Init ORIG_HUD_Init;
	_HUD_Redraw ORIG_HUD_Redraw;

	void **ppmove;
	ptrdiff_t offOldbuttons;
	ptrdiff_t offOnground;
	
	ptrdiff_t offBhopcap;
	byte originalBhopcapInsn[6];

	void *pHud;

	bool cantJumpNextTime;
};
