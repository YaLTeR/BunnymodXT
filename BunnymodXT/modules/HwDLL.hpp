#pragma once

#include "./sptlib-wrapper.hpp"
#include <SPTLib/IHookableNameFilter.hpp>

class HwDLL : public IHookableNameFilter
{
	HOOK_DECL(void, __cdecl, Cbuf_Execute)
	HOOK_DECL(void, __cdecl, SeedRandomNumberGenerator)
	HOOK_DECL(time_t, __cdecl, time, time_t *Time)

public:
	static HwDLL& GetInstance()
	{
		static HwDLL instance;
		return instance;
	}

	virtual void Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept);
	virtual void Unhook();
	virtual void Clear();

private:
	HwDLL() : IHookableNameFilter({ L"hw.dll", L"hw.so" }) {};
	HwDLL(const HwDLL&);
	void operator=(const HwDLL&);

protected:
	typedef void(__cdecl *_Cbuf_InsertText) (char* text);
	_Cbuf_InsertText ORIG_Cbuf_InsertText;
	typedef void(__cdecl *_Con_Printf) (char* fmt, ...);
	_Con_Printf ORIG_Con_Printf;

	void *cls;
	void *sv;

	bool insideSeedRNG;
};
