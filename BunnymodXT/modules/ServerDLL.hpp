#pragma once

#include "../sptlib-wrapper.hpp"
#include <SPTLib/IHookableDirFilter.hpp>

class ServerDLL : public IHookableDirFilter
{
	HOOK_DECL(void, __cdecl, PM_Jump)
	HOOK_DECL(void, __cdecl, PM_PreventMegaBunnyJumping)
	HOOK_DECL(void, __cdecl, PM_PlayerMove, qboolean server)
	HOOK_DECL(int, __cdecl, PM_ClipVelocity, float in[3], float normal[3], float out[3], float overbounce)
	HOOK_DECL(void, __cdecl, PM_WaterMove)
	HOOK_DECL(void, __cdecl, CmdStart, const edict_t* player, const usercmd_t* cmd, unsigned int random_seed)
	HOOK_DECL(void, __fastcall, CNihilanth__DyingThink, void* thisptr, int edx)
	HOOK_DECL(void, __cdecl, CNihilanth__DyingThink_Linux, void* thisptr)
	HOOK_DECL(void, __fastcall, COFGeneWorm__DyingThink, void* thisptr, int edx)
	HOOK_DECL(void, __cdecl, COFGeneWorm__DyingThink_Linux, void* thisptr)
	HOOK_DECL(void, __fastcall, CMultiManager__ManagerThink, void* thisptr, int edx)
	HOOK_DECL(void, __cdecl, CMultiManager__ManagerUse_Linux, void* thisptr, void* pActivator, void* pCaller, int useType, float value)
	HOOK_DECL(int, __cdecl, AddToFullPack, struct entity_state_s* state, int e, edict_t* ent, edict_t* host, int hostflags, int player, unsigned char* pSet)

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

	enginefuncs_t *pEngfuncs;

private:
	ServerDLL() : IHookableDirFilter({ L"dlls" }) {};
	ServerDLL(const ServerDLL&);
	void operator=(const ServerDLL&);

	void GetTriggerColor(const char *classname, float &r, float &g, float &b, float &a) const;

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

	globalvars_t **ppGlobals;

	std::unordered_map<int, bool> cantJumpNextTime;
};
