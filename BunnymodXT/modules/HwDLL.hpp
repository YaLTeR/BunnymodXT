#pragma once

#include "./sptlib-wrapper.hpp"
#include <SPTLib/IHookableNameFilterOrdered.hpp>

class HwDLL : public IHookableNameFilterOrdered
{
	HOOK_DECL(void, __cdecl, Cbuf_Execute)
	HOOK_DECL(void, __cdecl, SeedRandomNumberGenerator)
	HOOK_DECL(time_t, __cdecl, time, time_t *Time)
	HOOK_DECL(long double, __cdecl, RandomFloat, float a1, float a2)
	HOOK_DECL(long, __cdecl, RandomLong, long a1, long a2)

	struct cmdbuf_t
	{
		char *name;
		unsigned flags;
		char *data;
		unsigned maxsize;
		unsigned cursize;
	};

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
	// Make sure to have hl.exe last here, so that it is the lowest priority.
	HwDLL() : IHookableNameFilterOrdered({ L"hw.dll", L"hw.so", L"sw.dll", L"hl.exe" }) {};
	HwDLL(const HwDLL&);
	void operator=(const HwDLL&);

protected:
	typedef void(__cdecl *_Cbuf_InsertText) (char* text);
	_Cbuf_InsertText ORIG_Cbuf_InsertText;
	typedef void(__cdecl *_Con_Printf) (char* fmt, ...);
	_Con_Printf ORIG_Con_Printf;

	void *cls;
	void *sv;
	cmdbuf_t *cmd_text;

	bool insideSeedRNG;
};
