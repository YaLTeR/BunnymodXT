#pragma once

#include <SPTLib\IHookableNameFilter.hpp>

using std::uintptr_t;
using std::size_t;
using std::ptrdiff_t;

typedef void(__cdecl *_PM_Jump) ();
typedef void(__cdecl *_PM_PreventMegaBunnyJumping) ();
typedef int(__cdecl *_Initialize) (cl_enginefunc_t* pEnginefuncs, int iVersion);

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

	void RegisterCVarsAndCommands();

protected:
	_PM_Jump ORIG_PM_Jump;
	_PM_PreventMegaBunnyJumping ORIG_PM_PreventMegaBunnyJumping;
	_Initialize ORIG_Initialize;

	uintptr_t ppmove;
	ptrdiff_t offOldbuttons;
	ptrdiff_t offOnground;

	cl_enginefunc_t *pEngfuncs;

	bool cantJumpNextTime;
};
