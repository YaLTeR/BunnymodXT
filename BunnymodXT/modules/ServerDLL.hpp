#pragma once

#include "../sptlib-wrapper.hpp"
#include <SPTLib/IHookableDirFilter.hpp>

class ServerDLL : public IHookableDirFilter
{
	HOOK_DECL(void, __cdecl, PM_Jump)
	HOOK_DECL(void, __cdecl, PM_PreventMegaBunnyJumping)
	HOOK_DECL(void, __cdecl, PM_PlayerMove, qboolean server)
	HOOK_DECL(void, __cdecl, CmdStart, const edict_t* player, const usercmd_t* cmd, unsigned int random_seed)
	HOOK_DECL(void, __fastcall, CNihilanth__DyingThink, void* thisptr, int edx)

public:
	static ServerDLL& GetInstance()
	{
		static ServerDLL instance;
		return instance;
	}

	virtual void Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept);
	virtual void Unhook();
	virtual void Clear();
	virtual bool CanHook(const std::wstring& moduleFullName);

private:
	ServerDLL() : IHookableDirFilter({ L"dlls" }) {};
	ServerDLL(const ServerDLL&);
	void operator=(const ServerDLL&);

protected:
	typedef int(__cdecl *_GetEntityAPI)(DLL_FUNCTIONS* pFunctionTable, int interfaceVersion);
	_GetEntityAPI ORIG_GetEntityAPI;

	void FindStuff();
	void RegisterCVarsAndCommands();

	void **ppmove;
	ptrdiff_t offPlayerIndex;
	ptrdiff_t offOldbuttons;
	ptrdiff_t offOnground;
	ptrdiff_t offVelocity;
	ptrdiff_t offOrigin;
	ptrdiff_t offAngles;
	ptrdiff_t offCmd;

	ptrdiff_t offBhopcap;
	byte originalBhopcapInsn[6];

	enginefuncs_t *pEngfuncs;

	std::unordered_map<int, bool> cantJumpNextTime;
};
