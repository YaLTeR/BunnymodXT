#pragma once

#include <SPTLib\IHookableDirFilter.hpp>

using std::uintptr_t;
using std::size_t;
using std::ptrdiff_t;

typedef void(__cdecl *_PM_Jump) ();

class ServerDLL : public IHookableDirFilter
{
public:
	ServerDLL() : IHookableDirFilter({ L"dlls" }) {};
	virtual void Hook(const std::wstring& moduleName, HMODULE hModule, uintptr_t moduleStart, size_t moduleLength);
	virtual void Unhook();
	virtual void Clear();

	static void __cdecl HOOKED_PM_Jump();
	void __cdecl HOOKED_PM_Jump_Func();

protected:
	_PM_Jump ORIG_PM_Jump;

	uintptr_t ppmove;
	ptrdiff_t offOldbuttons;
	ptrdiff_t offOnground;

	bool cantJumpNextTime;
};
