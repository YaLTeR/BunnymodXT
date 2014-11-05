#pragma once

#include <SPTLib\IHookableNameFilter.hpp>
#include "hud_custom.hpp"

using std::uintptr_t;
using std::size_t;
using std::ptrdiff_t;

typedef void(__cdecl *_PM_Jump) ();
typedef void(__cdecl *_PM_PreventMegaBunnyJumping) ();
typedef int(__cdecl *_Initialize) (cl_enginefunc_t* pEnginefuncs, int iVersion);
typedef void(__fastcall *_CHud_InitFunc) (void* thisptr, int edx); // For both CHud::Init and CHud::VidInit.
typedef void(__fastcall *_CHud_AddHudElem) (void* thisptr, int edx, void* pHudElem);

class ClientDLL : public IHookableNameFilter
{
public:
	ClientDLL() : IHookableNameFilter({ L"client.dll" }) {};
	virtual void Hook(const std::wstring& moduleName, HMODULE hModule, uintptr_t moduleStart, size_t moduleLength);
	virtual void Unhook();
	virtual void Clear();

	static void __cdecl HOOKED_PM_Jump();
	void __cdecl HOOKED_PM_Jump_Func();
	static void __cdecl HOOKED_PM_PreventMegaBunnyJumping();
	void __cdecl HOOKED_PM_PreventMegaBunnyJumping_Func();
	static int __cdecl HOOKED_Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion);
	int __cdecl HOOKED_Initialize_Func(cl_enginefunc_t* pEnginefuncs, int iVersion);
	static void __fastcall HOOKED_CHud_Init(void* thisptr, int edx);
	void __fastcall HOOKED_CHud_Init_Func(void* thisptr, int edx);
	static void __fastcall HOOKED_CHud_VidInit(void* thisptr, int edx);
	void __fastcall HOOKED_CHud_VidInit_Func(void* thisptr, int edx);

	void RegisterCVarsAndCommands();
	void AddHudElem(void* pHudElem);

	cl_enginefunc_t *pEngfuncs;

protected:
	_PM_Jump ORIG_PM_Jump;
	_PM_PreventMegaBunnyJumping ORIG_PM_PreventMegaBunnyJumping;
	_Initialize ORIG_Initialize;
	_CHud_InitFunc ORIG_CHud_Init;
	_CHud_InitFunc ORIG_CHud_VidInit;
	_CHud_AddHudElem CHud_AddHudElem;

	uintptr_t ppmove;
	ptrdiff_t offOldbuttons;
	ptrdiff_t offOnground;

	uintptr_t pHud;

	bool cantJumpNextTime;

	bool novd; // Use the CHudBase w/o virtual destructor.
	CHudCustom_Wrapper customHudWrapper;
	CHudCustom_Wrapper_NoVD customHudWrapper_NoVD;
};
