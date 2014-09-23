#pragma once

#include <SPTLib\IHookableDirFilter.hpp>

using std::uintptr_t;
using std::size_t;
using std::ptrdiff_t;

typedef void(__cdecl *_PM_Jump) ();
typedef void(__cdecl *_PM_PreventMegaBunnyJumping) ();
typedef void(__cdecl *_PM_PlayerMove) (qboolean);
typedef void(__stdcall *_GiveFnptrsToDll) (enginefuncs_t*, const void*);

class ServerDLL : public IHookableDirFilter
{
public:
	ServerDLL() : IHookableDirFilter({ L"dlls" }) {};
	virtual void Hook(const std::wstring& moduleName, HMODULE hModule, uintptr_t moduleStart, size_t moduleLength);
	virtual void Unhook();
	virtual void Clear();
	virtual bool CanHook(const std::wstring& moduleFullName);

	static void __cdecl HOOKED_PM_Jump();
	void __cdecl HOOKED_PM_Jump_Func();
	static void __cdecl HOOKED_PM_PreventMegaBunnyJumping();
	void __cdecl HOOKED_PM_PreventMegaBunnyJumping_Func();
	static void __cdecl HOOKED_PM_PlayerMove(qboolean server);
	void __cdecl HOOKED_PM_PlayerMove_Func(qboolean server);
	static void __stdcall HOOKED_GiveFnptrsToDll(enginefuncs_t* pEngfuncsFromEngine, const void* pGlobals);
	void __stdcall HOOKED_GiveFnptrsToDll_Func(enginefuncs_t* pEngfuncsFromEngine, const void* pGlobals);

	void RegisterCVarsAndCommands();

protected:
	_PM_Jump ORIG_PM_Jump;
	_PM_PreventMegaBunnyJumping ORIG_PM_PreventMegaBunnyJumping;
	_PM_PlayerMove ORIG_PM_PlayerMove;
	_GiveFnptrsToDll ORIG_GiveFnptrsToDll;

	uintptr_t ppmove;
	ptrdiff_t offOldbuttons;
	ptrdiff_t offOnground;
	ptrdiff_t offVelocity;
	ptrdiff_t offOrigin;
	ptrdiff_t offAngles;

	enginefuncs_t *pEngfuncs;

	bool cantJumpNextTime;
};
