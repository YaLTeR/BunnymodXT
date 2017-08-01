#pragma once

#include <deque>
#include "../sptlib-wrapper.hpp"
#include <SPTLib/IHookableDirFilter.hpp>
#include "taslogger/writer.hpp"

class ServerDLL : public IHookableDirFilter
{
	HOOK_DECL(void, __cdecl, PM_Jump)
	HOOK_DECL(void, __cdecl, PM_PreventMegaBunnyJumping)
	HOOK_DECL(void, __cdecl, PM_PlayerMove, qboolean server)
	HOOK_DECL(int, __cdecl, PM_ClipVelocity, float in[3], float normal[3], float out[3], float overbounce)
	HOOK_DECL(void, __cdecl, PM_WaterMove)
	HOOK_DECL(void, __cdecl, PM_WalkMove)
	HOOK_DECL(void, __cdecl, PM_FlyMove)
	HOOK_DECL(bool, __cdecl, PM_AddToTouched, pmtrace_t tr, float* impactvelocity)
	HOOK_DECL(void, __cdecl, PM_Move, struct playermove_s* ppmove, int server)
	HOOK_DECL(void, __cdecl, CmdStart, const edict_t* player, const usercmd_t* cmd, unsigned int random_seed)
	HOOK_DECL(void, __fastcall, CNihilanth__DyingThink, void* thisptr, int edx)
	HOOK_DECL(void, __cdecl, CNihilanth__DyingThink_Linux, void* thisptr)
	HOOK_DECL(void, __fastcall, COFGeneWorm__DyingThink, void* thisptr, int edx)
	HOOK_DECL(void, __cdecl, COFGeneWorm__DyingThink_Linux, void* thisptr)
	HOOK_DECL(void, __fastcall, CApache__DyingThink, void* thisptr)
	HOOK_DECL(void, __fastcall, CBaseDoor__DoorGoUp, void* thisptr)
	HOOK_DECL(void, __fastcall, CMultiManager__ManagerThink, void* thisptr, int edx)
	HOOK_DECL(void, __cdecl, CMultiManager__ManagerUse_Linux, void* thisptr, void* pActivator, void* pCaller, int useType, float value)
	HOOK_DECL(int, __cdecl, AddToFullPack, struct entity_state_s* state, int e, edict_t* ent, edict_t* host, int hostflags, int player, unsigned char* pSet)
	HOOK_DECL(void, __fastcall, CTriggerVolume__Spawn, void* thisptr)
	HOOK_DECL(void, __cdecl, CTriggerVolume__Spawn_Linux, void* thisptr)
	HOOK_DECL(void, __cdecl, ClientCommand, edict_t* pEntity)
	HOOK_DECL(void, __fastcall, CPushable__Move, void* thisptr, int edx, void* pOther, int push)
	HOOK_DECL(int, __fastcall, CBasePlayer__TakeDamage, void* thisptr, int edx, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
	HOOK_DECL(void, __fastcall, CGraph__InitGraph, void* thisptr)
	HOOK_DECL(void, __cdecl, CGraph__InitGraph_Linux, void* thisptr)

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

	bool GetGlobalState(const std::string& name, int& state);

	std::vector<const edict_t *> GetUseableEntities(const Vector &origin, float radius) const;
	std::vector<const Vector *> GetNodePositions() const;

	enginefuncs_t *pEngfuncs;
	globalvars_t **ppGlobals;

private:
	ServerDLL() : IHookableDirFilter({ L"dlls" }) {};
	ServerDLL(const ServerDLL&);
	void operator=(const ServerDLL&);

	void GetTriggerColor(const char *classname, int solidType, float &r, float &g, float &b, float &a) const;

protected:
	typedef int(__cdecl *_GetEntityAPI)(DLL_FUNCTIONS* pFunctionTable, int interfaceVersion);
	_GetEntityAPI ORIG_GetEntityAPI;

	typedef void(__fastcall *_CBasePlayer__ForceClientDllUpdate)(void *thisptr);
	_CBasePlayer__ForceClientDllUpdate ORIG_CBasePlayer__ForceClientDllUpdate;
	typedef void(__cdecl *_CBasePlayer__ForceClientDllUpdate_Linux)(void *thisptr);
	_CBasePlayer__ForceClientDllUpdate_Linux ORIG_CBasePlayer__ForceClientDllUpdate_Linux;
	typedef void*(__cdecl *_PM_Ladder)();
	_PM_Ladder ORIG_PM_Ladder;

	typedef bool (__fastcall *_IsPlayer)(void *thisptr);
	typedef void (__fastcall *_Center)(void *thisptr, int edx, Vector *center);

#ifdef _WIN32
	typedef int (__fastcall *_ObjectCaps)(void *thisptr);
#else
	typedef int (__cdecl *_ObjectCaps)(void *thisptr);
#endif

	void FindStuff();
	void RegisterCVarsAndCommands();
	void LogPlayerMove(bool pre, uintptr_t pmove) const;
	bool IsPlayerMovingPushable(const entvars_t *pevPushable, const entvars_t *pevToucher, int push) const;

	void **ppmove;
	ptrdiff_t offPlayerIndex;
	ptrdiff_t offOldbuttons;
	ptrdiff_t offOnground;
	ptrdiff_t offVelocity;
	ptrdiff_t offOrigin;
	ptrdiff_t offAngles;
	ptrdiff_t offCmd;
	ptrdiff_t offEntFriction;
	ptrdiff_t offEntGravity;
	ptrdiff_t offPunchangles;
	ptrdiff_t offWaterlevel;
	ptrdiff_t offInDuck;
	ptrdiff_t offFlags;
	ptrdiff_t offBasevelocity;

	void *pGlobalState;

	static const ptrdiff_t offFuncIsPlayer = 0x9C;
	static const ptrdiff_t offFuncCenter = 0xC8;
	static const ptrdiff_t offFuncObjectCaps = 0x14;

	ptrdiff_t offm_iClientFOV;
	ptrdiff_t offm_rgAmmoLast;
	int maxAmmoSlots;

	void *WorldGraph = nullptr;
	size_t size_CNode;
	ptrdiff_t offm_pNodes;
	ptrdiff_t offm_cNodes;
	ptrdiff_t offm_vecOrigin;

	ptrdiff_t offBhopcap;
	byte originalBhopcapInsn[6];

	bool callerIsWalkMove;
	bool callerIsFlyMove;
	bool firstFlyMoveEnded;
	float firstFlyMoveEndVelocity[3];
	float firstFlyMoveEndOrigin[3];

	std::deque<TASLogger::Collision> firstFlyMoveTouchQueue;
	std::deque<TASLogger::Collision> secondFlyMoveTouchQueue;

	std::unordered_map<int, bool> cantJumpNextTime;
};
