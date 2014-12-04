#pragma once

#include "../stdafx.hpp"

#include <SPTLib/IHookableDirFilter.hpp>

typedef void(__cdecl *_PM_Jump) ();
typedef void(__cdecl *_PM_PreventMegaBunnyJumping) ();
typedef void(__cdecl *_PM_PlayerMove) (qboolean);
typedef void(__stdcall *_GiveFnptrsToDll) (enginefuncs_t*, const void*);

class ServerDLL : public IHookableDirFilter
{
public:
	ServerDLL() : IHookableDirFilter({ L"dlls" }) {};
	virtual void Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept);
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

	void **ppmove;
	ptrdiff_t offPlayerIndex;
	ptrdiff_t offOldbuttons;
	ptrdiff_t offOnground;
	ptrdiff_t offVelocity;
	ptrdiff_t offOrigin;
	ptrdiff_t offAngles;

	enginefuncs_t *pEngfuncs;

	std::unordered_map<int, bool> cantJumpNextTime;
};
