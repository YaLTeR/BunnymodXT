#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include "HwDLL.hpp"
#include "../patterns.hpp"
#include "../cvars.hpp"

// Linux hooks.
#ifndef _WIN32
extern "C" void __cdecl Cbuf_Execute()
{
	return HwDLL::HOOKED_Cbuf_Execute();
}
#endif

void HwDLL::Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept)
{
	Clear(); // Just in case.

	m_Handle = moduleHandle;
	m_Base = moduleBase;
	m_Length = moduleLength;
	m_Name = moduleName;
	m_Intercepted = needToIntercept;

	FindStuff();

	if (needToIntercept)
		MemUtils::Intercept(moduleName, {
			{ reinterpret_cast<void**>(&ORIG_Cbuf_Execute), reinterpret_cast<void*>(HOOKED_Cbuf_Execute) },
			{ reinterpret_cast<void**>(&ORIG_SeedRandomNumberGenerator), reinterpret_cast<void*>(HOOKED_SeedRandomNumberGenerator) },
			{ reinterpret_cast<void**>(&ORIG_time), reinterpret_cast<void*>(HOOKED_time) },
			{ reinterpret_cast<void**>(&ORIG_RandomFloat), reinterpret_cast<void*>(HOOKED_RandomFloat) },
			{ reinterpret_cast<void**>(&ORIG_RandomLong), reinterpret_cast<void*>(HOOKED_RandomLong) },
			{ reinterpret_cast<void**>(&ORIG_Host_Changelevel2_f), reinterpret_cast<void*>(HOOKED_Host_Changelevel2_f) }
		});
}

void HwDLL::Unhook()
{
	if (m_Intercepted)
		MemUtils::RemoveInterception(m_Name, {
			{ reinterpret_cast<void**>(&ORIG_Cbuf_Execute), reinterpret_cast<void*>(HOOKED_Cbuf_Execute) },
			{ reinterpret_cast<void**>(&ORIG_SeedRandomNumberGenerator), reinterpret_cast<void*>(HOOKED_SeedRandomNumberGenerator) },
			{ reinterpret_cast<void**>(&ORIG_time), reinterpret_cast<void*>(HOOKED_time) },
			{ reinterpret_cast<void**>(&ORIG_RandomFloat), reinterpret_cast<void*>(HOOKED_RandomFloat) },
			{ reinterpret_cast<void**>(&ORIG_RandomLong), reinterpret_cast<void*>(HOOKED_RandomLong) },
			{ reinterpret_cast<void**>(&ORIG_RandomLong), reinterpret_cast<void*>(HOOKED_RandomLong) },
			{ reinterpret_cast<void**>(&ORIG_Host_Changelevel2_f), reinterpret_cast<void*>(HOOKED_Host_Changelevel2_f) }
	});

	for (auto cvar : CVars::allCVars)
		cvar->Refresh();

	Clear();
}

void HwDLL::Clear()
{
	ORIG_Cbuf_Execute = nullptr;
	ORIG_SeedRandomNumberGenerator = nullptr;
	ORIG_time = nullptr;
	ORIG_RandomFloat = nullptr;
	ORIG_RandomLong = nullptr;
	ORIG_Host_Changelevel2_f = nullptr;
	ORIG_Cbuf_InsertText = nullptr;
	ORIG_Con_Printf = nullptr;
	ORIG_Cvar_RegisterVariable = nullptr;
	ORIG_Cvar_DirectSet = nullptr;
	ORIG_Cvar_FindVar = nullptr;
	ORIG_Cmd_AddMallocCommand = nullptr;
	ORIG_Cmd_Argc = nullptr;
	ORIG_Cmd_Args = nullptr;
	ORIG_Cmd_Argv = nullptr;
	registeredVarsAndCmds = false;
	cls = nullptr;
	sv = nullptr;
	cmd_text = nullptr;
	host_frametime = nullptr;
	rng_global_1 = nullptr;
	rng_global_2 = nullptr;
	executing = false;
	loading = false;
	insideCbuf_Execute = false;
	finishingLoad = false;
	dontPauseNextCycle = false;
	changelevel = false;
	insideSeedRNG = false;
	LastRandomSeed = 0;
	player = {};
	input.Clear();
	demoName.clear();
	saveName.clear();
	runningFrames = false;
	wasRunningFrames = false;
	currentFramebulk = 0;
	totalFramebulks = 0;
	currentRepeat = 0;
	previousButtons = {};
	SeedsPresent = false;
	SharedRNGSeed = 0;
	CountingSharedRNGSeed = false;
	SharedRNGSeedCounter = 0;
	LoadingSeedCounter = 0;
	ButtonsPresent = false;
}

void HwDLL::FindStuff()
{
	ORIG_Cbuf_Execute = reinterpret_cast<_Cbuf_Execute>(MemUtils::GetSymbolAddress(m_Handle, "Cbuf_Execute"));
	if (ORIG_Cbuf_Execute)
	{
		EngineDevMsg("[hw dll] Found Cbuf_Execute at %p.\n", ORIG_Cbuf_Execute);

		cls = MemUtils::GetSymbolAddress(m_Handle, "cls");
		if (cls)
			EngineDevMsg("[hw dll] Found cls at %p.\n", cls);
		else
			EngineDevWarning("[hw dll] Could not find cls.\n");

		sv = MemUtils::GetSymbolAddress(m_Handle, "sv");
		if (sv)
			EngineDevMsg("[hw dll] Found sv at %p.\n", sv);
		else
			EngineDevWarning("[hw dll] Could not find sv.\n");

		cmd_text = reinterpret_cast<cmdbuf_t*>(MemUtils::GetSymbolAddress(m_Handle, "cmd_text"));
		if (cmd_text)
			EngineDevMsg("[hw dll] Found cmd_text at %p.\n", cmd_text);
		else
			EngineDevWarning("[hw dll] Could not find cmd_text.\n");

		host_frametime = reinterpret_cast<double*>(MemUtils::GetSymbolAddress(m_Handle, "host_frametime"));
		if (host_frametime)
			EngineDevMsg("[hw dll] Found host_frametime at %p.\n", sv);
		else
			EngineDevWarning("[hw dll] Could not find host_frametime.\n");

		if (!cls || !sv || !cmd_text || !host_frametime)
			ORIG_Cbuf_Execute = nullptr;

		void* ran1 = MemUtils::GetSymbolAddress(m_Handle, "ran1");
		if (ran1) {
			EngineDevMsg("[hw dll] Found ran1 at %p.\n", ran1);
			auto f = reinterpret_cast<uintptr_t>(ran1);
			// TODO set rng globals.
		}
		else
			EngineDevWarning("[hw dll] Could not find ran1.\n");

		#define FIND(f) \
			ORIG_##f = reinterpret_cast<_##f>(MemUtils::GetSymbolAddress(m_Handle, #f)); \
			if (ORIG_##f) \
				EngineDevMsg("[hw dll] Found " #f " at %p.\n", ORIG_##f); \
			else \
			{ \
				EngineDevWarning("[hw dll] Could not find " #f ".\n"); \
				ORIG_Cbuf_Execute = nullptr; \
			}
		FIND(Con_Printf)
		FIND(Cvar_RegisterVariable)
		FIND(Cvar_DirectSet)
		FIND(Cvar_FindVar)
		FIND(Cbuf_InsertText)
		FIND(Cmd_AddMallocCommand)
		FIND(SeedRandomNumberGenerator)
		//FIND(RandomFloat)
		//FIND(RandomLong)
		FIND(Host_Changelevel2_f)
		#undef FIND
	}
	else
	{
		#define DEF_FUTURE(name) auto f##name = MemUtils::FindPatternOnly(reinterpret_cast<void**>(&ORIG_##name), m_Base, m_Length, Patterns::ptns##name, [](MemUtils::ptnvec_size ptnNumber) { }, []() { });
		DEF_FUTURE(Cvar_RegisterVariable)
		DEF_FUTURE(Cvar_DirectSet)
		DEF_FUTURE(Cvar_FindVar)
		DEF_FUTURE(Cbuf_InsertText)
		DEF_FUTURE(Cmd_AddMallocCommand)
		//DEF_FUTURE(RandomFloat)
		//DEF_FUTURE(RandomLong)
		DEF_FUTURE(Host_Changelevel2_f)
		#undef DEF_FUTURE

		auto fCbuf_Execute = MemUtils::FindPatternOnly(reinterpret_cast<void**>(&ORIG_Cbuf_Execute), m_Base, m_Length, Patterns::ptnsCbuf_Execute,
			[&](MemUtils::ptnvec_size ptnNumber) {
				switch (ptnNumber)
				{
				case 0:
					cmd_text = reinterpret_cast<cmdbuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 11) - offsetof(cmdbuf_t, cursize));
					break;
				case 1:
					cmd_text = reinterpret_cast<cmdbuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 2) - offsetof(cmdbuf_t, cursize));
					break;
				}
			}, []() {}
		);

		auto fSeedRandomNumberGenerator = MemUtils::FindPatternOnly(reinterpret_cast<void**>(&ORIG_SeedRandomNumberGenerator), m_Base, m_Length, Patterns::ptnsSeedRandomNumberGenerator,
			[&](MemUtils::ptnvec_size ptnNumber) {
				ORIG_time = reinterpret_cast<_time>(
					*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_SeedRandomNumberGenerator) + 3)
					+ reinterpret_cast<uintptr_t>(ORIG_SeedRandomNumberGenerator) + 7
				);
			}, []() {}
		);

		void *SCR_DrawFPS;
		auto fSCR_DrawFPS = MemUtils::FindPatternOnly(&SCR_DrawFPS, m_Base, m_Length, Patterns::ptnsSCR_DrawFPS,
			[&](MemUtils::ptnvec_size ptnNumber) {
				host_frametime = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(SCR_DrawFPS) + 21);
			}, []() {}
		);

		void *Host_Tell_f;
		auto fHost_Tell_f = MemUtils::FindPatternOnly(&Host_Tell_f, m_Base, m_Length, Patterns::ptnsHost_Tell_f,
			[&](MemUtils::ptnvec_size ptnNumber) {
				uintptr_t offCmd_Argc, offCmd_Args, offCmd_Argv;
				switch (ptnNumber)
				{
				case 0: // SteamPipe.
					offCmd_Argc = 28;
					offCmd_Args = 42;
					offCmd_Argv = 145;
					break;
				case 1: // NGHL.
					offCmd_Argc = 24;
					offCmd_Args = 38;
					offCmd_Argv = 143;
					break;
				}

				auto f = reinterpret_cast<uintptr_t>(Host_Tell_f);
				ORIG_Cmd_Argc = reinterpret_cast<_Cmd_Argc>(
					*reinterpret_cast<uintptr_t*>(f + offCmd_Argc)
					+ (f + offCmd_Argc + 4)
				);
				ORIG_Cmd_Args = reinterpret_cast<_Cmd_Args>(
					*reinterpret_cast<uintptr_t*>(f + offCmd_Args)
					+ (f + offCmd_Args + 4)
				);
				ORIG_Cmd_Argv = reinterpret_cast<_Cmd_Argv>(
					*reinterpret_cast<uintptr_t*>(f + offCmd_Argv)
					+ (f + offCmd_Argv + 4)
				);
			}, []() { }
		);

		void *Host_AutoSave_f;
		auto fHost_AutoSave_f = MemUtils::FindPatternOnly(&Host_AutoSave_f, m_Base, m_Length, Patterns::ptnsHost_AutoSave_f,
			[&](MemUtils::ptnvec_size ptnNumber) {
				auto f = reinterpret_cast<uintptr_t>(Host_AutoSave_f);
				sv = *reinterpret_cast<void**>(f + 19);
				ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
					*reinterpret_cast<ptrdiff_t*>(f + 33)
					+ (f + 37)
					);
				cls = *reinterpret_cast<void**>(f + 69);
			}, []() {}
		);

		void *ran1;
		auto fran1 = MemUtils::FindPatternOnly(&ran1, m_Base, m_Length, Patterns::ptnsran1,
			[&](MemUtils::ptnvec_size ptnNumber) {
				auto f = reinterpret_cast<uintptr_t>(ran1);
				rng_global_1 = *reinterpret_cast<int**>(f + 13);
				rng_global_2 = *reinterpret_cast<int**>(f + 97);
			}, []() {}
		);

		auto n = fCbuf_Execute.get();
		if (ORIG_Cbuf_Execute) {
			EngineDevMsg("[hw dll] Found Cbuf_Execute at %p (using the %s pattern).\n", ORIG_Cbuf_Execute, Patterns::ptnsCbuf_Execute[n].build.c_str());
			EngineDevMsg("[hw dll] Found cmd_text at %p.\n", cmd_text);
		} else
			EngineDevWarning("[hw dll] Could not find Cbuf_Execute.\n");

		n = fHost_AutoSave_f.get();
		if (Host_AutoSave_f) {
			EngineDevMsg("[hw dll] Found Host_AutoSave_f at %p (using the %s pattern).\n", Host_AutoSave_f, Patterns::ptnsHost_AutoSave_f[n].build.c_str());
			EngineDevMsg("[hw dll] Found cls at %p.\n", cls);
			EngineDevMsg("[hw dll] Found sv at %p.\n", sv);
			EngineDevMsg("[hw dll] Found Con_Printf at %p.\n", ORIG_Con_Printf);
		} else {
			EngineDevWarning("[hw dll] Could not find Host_AutoSave_f.\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		n = fSeedRandomNumberGenerator.get();
		if (ORIG_SeedRandomNumberGenerator) {
			EngineDevMsg("[hw dll] Found SeedRandomNumberGenerator at %p (using the %s pattern).\n", ORIG_SeedRandomNumberGenerator, Patterns::ptnsSeedRandomNumberGenerator[n].build.c_str());
			EngineDevMsg("[hw dll] ORIG_time is %p.\n", ORIG_time);
		} else {
			EngineDevWarning("[hw dll] Could not find SeedRandomNumberGenerator.\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		n = fSCR_DrawFPS.get();
		if (SCR_DrawFPS) {
			EngineDevMsg("[hw dll] Found SCR_DrawFPS at %p (using the %s pattern).\n", SCR_DrawFPS, Patterns::ptnsSCR_DrawFPS[n].build.c_str());
			EngineDevMsg("[hw dll] Found host_frametime at %p.\n", host_frametime);
		} else {
			EngineDevWarning("[hw dll] Could not find SCR_DrawFPS.\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		#define GET_FUTURE(name) \
			n = f##name.get(); \
			if (ORIG_##name) { \
				EngineDevMsg("[hw dll] Found " #name " at %p (using the %s pattern).\n", ORIG_##name, Patterns::ptns##name[n].build.c_str()); \
			} else { \
				EngineDevWarning("[hw dll] Could not find " #name ".\n"); \
				ORIG_Cbuf_Execute = nullptr; \
			}
		GET_FUTURE(Cvar_RegisterVariable)
		GET_FUTURE(Cvar_DirectSet)
		GET_FUTURE(Cvar_FindVar)
		GET_FUTURE(Cbuf_InsertText)
		GET_FUTURE(Cmd_AddMallocCommand)
		//GET_FUTURE(RandomFloat)
		//GET_FUTURE(RandomLong)
		GET_FUTURE(Host_Changelevel2_f)
		#undef GET_FUTURE

		n = fHost_Tell_f.get();
		if (Host_Tell_f) {
			EngineDevMsg("[hw dll] Found Host_Tell_f at %p (using the %s pattern).\n", Host_Tell_f, Patterns::ptnsHost_Tell_f[n].build.c_str());
			EngineDevMsg("[hw dll] Found Cmd_Argc at %p.\n", ORIG_Cmd_Argc);
			EngineDevMsg("[hw dll] Found Cmd_Args at %p.\n", ORIG_Cmd_Args);
			EngineDevMsg("[hw dll] Found Cmd_Argv at %p.\n", ORIG_Cmd_Argv);
		} else {
			EngineDevWarning("[hw dll] Could not find Host_Tell_f.\n");
			ORIG_Cmd_AddMallocCommand = nullptr;
			ORIG_Cbuf_Execute = nullptr;
		}

		n = fran1.get();
		if (ran1) {
			EngineDevMsg("[hw dll] Found ran1 at %p (using the %s pattern).\n", ran1, Patterns::ptnsran1[n].build.c_str());
			EngineDevMsg("[hw dll] Found the first RNG global at %p.\n", rng_global_1);
			EngineDevMsg("[hw dll] Found the second RNG global at %p.\n", rng_global_2);
		} else {
			EngineDevWarning("[hw dll] Could not find ran1.\n");
			ORIG_Cbuf_Execute = nullptr;
		}
	}

	if (ORIG_Cbuf_Execute && !ORIG_time)
		ORIG_time = time;
}

void HwDLL::RegisterCVar(CVarWrapper& cvar)
{
	if (!ORIG_Cvar_FindVar || !ORIG_Cvar_RegisterVariable)
		return;

	if (ORIG_Cvar_FindVar(cvar.GetPointer()->name))
		return;

	ORIG_Cvar_RegisterVariable(cvar.GetPointer());
	cvar.MarkAsStale();
}

cvar_t* HwDLL::FindCVar(const char* name)
{
	if (!ORIG_Cvar_FindVar)
		return nullptr;

	return ORIG_Cvar_FindVar(name);
}

void HwDLL::Cmd_BXT_TAS_LoadScript()
{
	return HwDLL::GetInstance().Cmd_BXT_TAS_LoadScript_f();
}
void HwDLL::Cmd_BXT_TAS_LoadScript_f()
{
	runningFrames = false;
	currentFramebulk = 0;
	currentRepeat = 0;

	if (ORIG_Cmd_Argc() != 2) {
		ORIG_Con_Printf("Usage: bxt_tas_loadscript <filename>\n");
		return;
	}

	ORIG_Cvar_DirectSet(CVars::bxt_tas.GetPointer(), "1");

	std::string filename(ORIG_Cmd_Argv(1));
	auto err = input.Open(filename).get();
	if (err.Code != HLTAS::ErrorCode::OK) {
		ORIG_Con_Printf("Error loading the script file on line %u: %s\n", err.LineNumber, HLTAS::GetErrorMessage(err));
		return;
	}

	for (auto prop : input.GetProperties()) {
		if (prop.first == "demo")
			demoName = prop.second;
		else if (prop.first == "save")
			saveName = prop.second;
		else if (prop.first == "seed") {
			std::istringstream ss(prop.second);
			ss >> SharedRNGSeed >> NonSharedRNGSeed;
			SeedsPresent = true;
			SetNonSharedRNGSeed = true;
		}
	}

	if (!input.GetFrames().empty()) {
		runningFrames = true;
		totalFramebulks = input.GetFrames().size();
		HLTAS::Frame f;
		if (GetNextMovementFrame(f)) {
			std::ostringstream ss;
			ss << "host_framerate " << f.Frametime.c_str() << "\n";
			ORIG_Cbuf_InsertText(ss.str().c_str());
		}
	}
}

void HwDLL::RegisterCVarsAndCommandsIfNeeded()
{
	if (!registeredVarsAndCmds)
	{
		registeredVarsAndCmds = true;
		RegisterCVar(CVars::bxt_tas);
		RegisterCVar(CVars::_bxt_taslog);
		if (ORIG_Cmd_AddMallocCommand)
			ORIG_Cmd_AddMallocCommand("bxt_tas_loadscript", Cmd_BXT_TAS_LoadScript, 2); // 2 - Cmd_AddGameCommand.
	}
}

bool HwDLL::CheckUnpause()
{
	size_t current_cmd;
	for (size_t off = 0; off < cmd_text->cursize; ++off)
	{
		current_cmd = off;
		unsigned quotes = 0;
		for (; off < cmd_text->cursize; ++off)
		{
			char c = cmd_text->data[off];
			quotes += (c == '"');
			if (!(quotes & 1) && c == ';')
				break;
			if (c == '\n')
				break;
		}

		if (off - current_cmd == 7 && !std::strncmp(cmd_text->data + current_cmd, "unpause", 7))
			return true;
	}

	return false;
}

void HwDLL::InsertCommands()
{
	bool runningFramesBackup = runningFrames;

	if (runningFrames) {
		while (currentFramebulk < totalFramebulks) {
			auto f = input.GetFrame(currentFramebulk);
			// Movement frame.
			if (currentRepeat || (f.SaveName.empty() && !f.SeedsPresent && f.Buttons == HLTAS::ButtonState::NOTHING)) {
				auto c = f.Commands;
				if (!c.empty())
					ORIG_Cbuf_InsertText(c.c_str());

				auto p = HLStrafe::MainFunc(player, GetMovementVars(), f);

				// TODO viewangles.

				#define INS(btn, cmd) \
					if (p.btn && !previousButtons.btn) \
						ORIG_Cbuf_InsertText("+" #cmd "\n"); \
					else if (!p.btn && previousButtons.btn) \
						ORIG_Cbuf_InsertText("-" #cmd "\n");
				INS(Forward, forward)
				INS(Left, moveleft)
				INS(Right, moveright)
				INS(Back, back)
				INS(Up, moveup)
				INS(Down, movedown)
				INS(Jump, jump)
				INS(Duck, duck)
				INS(Use, use)
				INS(Attack1, attack)
				INS(Attack2, attack2)
				INS(Reload, reload)
				#undef INS

				// TODO speeds.

				// We need this to be in the before all our movement commands,
				// so insert it last.
				if (!wasRunningFrames)
					ResetButtons();

				previousButtons = p;

				if (++currentRepeat >= f.GetRepeats()) {
					currentRepeat = 0;
					currentFramebulk++;
					HLTAS::Frame next;
					if (GetNextMovementFrame(next)) {
						if (next.Frametime != f.Frametime) {
							std::ostringstream ss;
							ss << "host_framerate " << next.Frametime.c_str() << "\n";
							ORIG_Cbuf_InsertText(ss.str().c_str());
						}
					}
				}
				break;
			} else if (!f.SaveName.empty()) { // Saveload frame.
				std::ostringstream ss;
				ss << "save " << f.SaveName << ";load " << f.SaveName << "\n";
				ORIG_Cbuf_InsertText(ss.str().c_str());
				currentFramebulk++;
				break;
			} else if (f.SeedsPresent) { // Seeds frame.
				SeedsPresent = true;
				NonSharedRNGSeed = f.GetNonSharedRNGSeed();
				SharedRNGSeed = f.GetSharedRNGSeed();
			} else if (f.Buttons != HLTAS::ButtonState::NOTHING) { // Buttons frame.
				if (f.Buttons == HLTAS::ButtonState::SET) {
					ButtonsPresent = true;
					AirLeftBtn = f.GetAirLeftBtn();
					AirRightBtn = f.GetAirRightBtn();
					GroundLeftBtn = f.GetGroundLeftBtn();
					GroundRightBtn = f.GetGroundRightBtn();
				} else
					ButtonsPresent = false;
			}

			currentFramebulk++;
		};

		// Ran through all frames.
		if (currentFramebulk >= totalFramebulks)
			runningFrames = false;
	} else {
		if (wasRunningFrames) {
			ResetButtons();
			CountingSharedRNGSeed = false;
		}
	}

	wasRunningFrames = runningFramesBackup;
}

bool HwDLL::GetNextMovementFrame(HLTAS::Frame& f)
{
	auto curFramebulk = currentFramebulk;
	while (curFramebulk < totalFramebulks) {
		f = input.GetFrame(curFramebulk);
		// Only movement frames can have repeats.
		if (currentRepeat || (f.SaveName.empty() && !f.SeedsPresent && f.Buttons == HLTAS::ButtonState::NOTHING))
			return true;

		curFramebulk++;
	};

	return false;
}

void HwDLL::ResetButtons()
{
	ORIG_Cbuf_InsertText("-forward\n"
	                     "-moveleft\n"
	                     "-moveright\n"
	                     "-back\n"
	                     "-moveup\n"
	                     "-movedown\n"
	                     "-left\n"
	                     "-right\n"
	                     "-lookup\n"
	                     "-lookdown\n"
	                     "-jump\n"
	                     "-duck\n"
	                     "-use\n"
	                     "-attack\n"
	                     "-attack2\n"
	                     "-reload\n");
	previousButtons = {};
}

void HwDLL::FindCVarsIfNeeded()
{
	#define FIND(cvar) if (!CVars::cvar.GetPointer()) CVars::cvar.Assign(FindCVar(#cvar))
	FIND(sv_maxvelocity);
	FIND(sv_maxspeed);
	FIND(sv_stopspeed);
	FIND(sv_friction);
	FIND(sv_edgefriction);
	FIND(sv_accelerate);
	FIND(sv_airaccelerate);
	FIND(sv_gravity);
	#undef FIND
}

HLStrafe::MovementVars HwDLL::GetMovementVars()
{
	HLStrafe::MovementVars vars = {};
	
	FindCVarsIfNeeded();
	vars.Frametime = static_cast<float>(*host_frametime);
	vars.Maxvelocity = CVars::sv_maxvelocity.GetFloat();
	vars.Maxspeed = CVars::sv_maxspeed.GetFloat();
	vars.Stopspeed = CVars::sv_stopspeed.GetFloat();
	vars.Friction = CVars::sv_friction.GetFloat();
	vars.Edgefriction = CVars::sv_edgefriction.GetFloat();
	vars.EntFriction = 1.0f; // TBD
	vars.Accelerate = CVars::sv_accelerate.GetFloat();
	vars.Airaccelerate = CVars::sv_airaccelerate.GetFloat();
	vars.Gravity = CVars::sv_gravity.GetFloat();
	vars.EntGravity = 1.0f; // TBD

	return vars;
}

void HwDLL::SetNonSharedRNG()
{
	insideSeedRNG = true;
	ORIG_SeedRandomNumberGenerator();
	insideSeedRNG = false;
	*rng_global_1 = 0;
	for (size_t i = 0; i < 32; ++i)
		rng_global_2[i] = 0;
}

HOOK_DEF_0(HwDLL, void, __cdecl, Cbuf_Execute)
{
	RegisterCVarsAndCommandsIfNeeded();

	int state = *reinterpret_cast<int*>(cls);
	int paused = *(reinterpret_cast<int*>(sv) + 1);

	// If cls.state == 4 and the game isn't paused, execute "pause" right now.
	// This case happens when loading a savegame.
	if (state == 4 && !paused && CVars::bxt_tas.GetBool())
	{
		ORIG_Cbuf_InsertText("pause\n");
		finishingLoad = true;
	}

	// All map load / change commands call Cbuf_Execute inside them, while we already are inside one.
	if (insideCbuf_Execute)
	{
		loading = true;
		executing = false;
	}
	if (state != 5)
		executing = false;
	if (!loading && state == 5)
		executing = true;
	if (loading && state == 3)
		loading = false;
	// Manually unpause in WON.
	if (finishingLoad && state == 5 && paused && !CheckUnpause())
		ORIG_Cbuf_InsertText("pause\n");

	static unsigned counter = 1;
	auto c = counter++;
	std::string buf(cmd_text->data, cmd_text->cursize); // TODO: ifdef this so it doesn't waste performance.
	if (CVars::_bxt_taslog.GetBool())
		ORIG_Con_Printf("Cbuf_Execute() #%u begin; cls.state: %d; sv.paused: %d; time: %f; loading: %s; executing: %s; host_frametime: %f; buffer: %s\n", c, state, paused, *reinterpret_cast<double*>(reinterpret_cast<uintptr_t>(sv)+16), (loading ? "true" : "false"), (executing ? "true" : "false"), *host_frametime, buf.c_str());

	insideCbuf_Execute = true;
	ORIG_Cbuf_Execute(); // executing might change inside if we had some kind of load command in the buffer.

	// Insert our commands after any commands that might have been on this frame
	// and call Cbuf_Execute again to execute them.
	if (executing)
	{
		changelevel = false;
		if (finishingLoad) { // First frame after load.
			finishingLoad = false;
			if (SeedsPresent) {
				if (LoadingSeedCounter)
					SharedRNGSeedCounter += SharedRNGSeed;
				else
					SharedRNGSeedCounter = SharedRNGSeed;
				//SetNonSharedRNG();
				SeedsPresent = false; // This should come after the RNG setting as that checks SeedsPresent itself.
				CountingSharedRNGSeed = true;
			} else {
				if (LoadingSeedCounter)
					SharedRNGSeedCounter += LoadingSeedCounter;
				else
					CountingSharedRNGSeed = false;
			}
			LoadingSeedCounter = 0;
		} else {
			SharedRNGSeedCounter++;
		}

		// For stopping Cbuf_Execute. Goes first because InsertCommands() inserts into beginning.
		if (cmd_text->cursize)
			ORIG_Cbuf_InsertText("wait\n");
		InsertCommands();

		buf.assign(cmd_text->data, cmd_text->cursize);
		if (CVars::_bxt_taslog.GetBool())
			ORIG_Con_Printf("Cbuf_Execute() #%u executing; buffer: %s\n", c, buf.c_str());

		// Setting to true once again because it might have been reset in Cbuf_Execute.
		insideCbuf_Execute = true;
		ORIG_Cbuf_Execute();
	} else if (changelevel) {
		LoadingSeedCounter++;
	}
	insideCbuf_Execute = false;

	buf.assign(cmd_text->data, cmd_text->cursize);
	if (CVars::_bxt_taslog.GetBool())
		ORIG_Con_Printf("Cbuf_Execute() #%u end; host_frametime: %f; buffer: %s\n", c, *host_frametime, buf.c_str());

	// If cls.state == 3 and the game isn't paused, execute "pause" on the next cycle.
	// This case happens when starting a map.
	if (!dontPauseNextCycle && state == 3 && !paused && CVars::bxt_tas.GetBool())
	{
		ORIG_Cbuf_InsertText("pause\n");
		finishingLoad = true;
		dontPauseNextCycle = true;
	}
	else
		dontPauseNextCycle = false;
}

void HwDLL::SetPlayerOrigin(float origin[3])
{
	player.Origin[0] = origin[0];
	player.Origin[1] = origin[1];
	player.Origin[2] = origin[2];
}

void HwDLL::SetPlayerVelocity(float velocity[3])
{
	player.Velocity[0] = velocity[0];
	player.Velocity[1] = velocity[1];
	player.Velocity[2] = velocity[2];
}

HOOK_DEF_0(HwDLL, void, __cdecl, SeedRandomNumberGenerator)
{
	insideSeedRNG = true;
	ORIG_SeedRandomNumberGenerator();
	insideSeedRNG = false;
}

HOOK_DEF_1(HwDLL, time_t, __cdecl, time, time_t*, Time)
{
	if (insideSeedRNG)
	{
		time_t ret = (SetNonSharedRNGSeed) ? NonSharedRNGSeed : ORIG_time(Time);
		SetNonSharedRNGSeed = false;

		std::ostringstream ss;
		ss << "Called time from SeedRandomNumberGenerator -> " << ret << ".\n";
		EngineMsg("%s", ss.str().c_str());

		return ret;
	}
	else
		return ORIG_time(Time);
}

HOOK_DEF_2(HwDLL, long double, __cdecl, RandomFloat, float, a1, float, a2)
{
	auto ret = ORIG_RandomFloat(a1, a2);
	ORIG_Con_Printf("RandomFloat(%f, %f) => %Lf.\n", a1, a2, ret);
	return ret;
}

HOOK_DEF_2(HwDLL, long, __cdecl, RandomLong, long, a1, long, a2)
{
	auto ret = ORIG_RandomLong(a1, a2);
	ORIG_Con_Printf("RandomLong(%ld, %ld) => %ld.\n", a1, a2, ret);
	return ret;
}

HOOK_DEF_0(HwDLL, void, __cdecl, Host_Changelevel2_f)
{
	changelevel = true;
	if (!CountingSharedRNGSeed && SeedsPresent)
		SharedRNGSeedCounter = LastRandomSeed;

	return ORIG_Host_Changelevel2_f();
}
