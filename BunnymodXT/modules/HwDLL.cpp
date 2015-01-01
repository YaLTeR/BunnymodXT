#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include "HwDLL.hpp"
#include "ClientDLL.hpp"
#include "../patterns.hpp"

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

	MemUtils::ptnvec_size ptnNumber;
	void *pCbuf_Execute, *pCbuf_InsertText, *pSeedRandomNumberGenerator;
	std::future<MemUtils::ptnvec_size> fCbuf_Execute, fCbuf_InsertText, fSeedRandomNumberGenerator;

	pCbuf_Execute = MemUtils::GetSymbolAddress(moduleHandle, "Cbuf_Execute");
	if (pCbuf_Execute)
	{
		ORIG_Cbuf_Execute = reinterpret_cast<_Cbuf_Execute>(pCbuf_Execute);
		EngineDevMsg("[hw dll] Found Cbuf_Execute at %p.\n", pCbuf_Execute);

		cls = MemUtils::GetSymbolAddress(moduleHandle, "cls");
		if (cls)
			EngineDevMsg("[hw dll] Found cls at %p.\n", cls);
		else
		{
			EngineDevWarning("[hw dll] Couldn't get the address of cls!\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		sv = MemUtils::GetSymbolAddress(moduleHandle, "sv");
		if (sv)
			EngineDevMsg("[hw dll] Found sv at %p.\n", sv);
		else
		{
			EngineDevWarning("[hw dll] Couldn't get the address of sv!\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(MemUtils::GetSymbolAddress(moduleHandle, "Con_Printf"));
		if (ORIG_Con_Printf)
			EngineDevMsg("[hw dll] Found Con_Printf at %p.", ORIG_Con_Printf);
		else
		{
			EngineDevWarning("[hw dll] Couldn't get the address of Con_Printf!\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		ORIG_Cbuf_InsertText = reinterpret_cast<_Cbuf_InsertText>(MemUtils::GetSymbolAddress(moduleHandle, "Cbuf_InsertText"));
		if (ORIG_Cbuf_InsertText)
			EngineDevMsg("[hw dll] Found Cbuf_InsertText at %p.\n", ORIG_Cbuf_InsertText);
		else
		{
			EngineDevWarning("[hw dll] Couldn't get the address of Cbuf_InsertText!\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		ORIG_SeedRandomNumberGenerator = reinterpret_cast<_SeedRandomNumberGenerator>(MemUtils::GetSymbolAddress(moduleHandle, "SeedRandomNumberGenerator"));
		if (ORIG_SeedRandomNumberGenerator)
			EngineDevMsg("[hw dll] Found SeedRandomNumberGenerator at %p.\n", ORIG_SeedRandomNumberGenerator);
		else
		{
			EngineDevWarning("[hw dll] Couldn't get the address of SeedRandomNumberGenerator!\n");
			ORIG_Cbuf_Execute = nullptr;
		}
	}
	else
	{
		fCbuf_Execute = std::async(MemUtils::FindUniqueSequence, moduleBase, moduleLength, Patterns::ptnsCbuf_Execute, &pCbuf_Execute);
		fCbuf_InsertText = std::async(MemUtils::FindUniqueSequence, moduleBase, moduleLength, Patterns::ptnsCbuf_InsertText, &pCbuf_InsertText);
		fSeedRandomNumberGenerator = std::async(MemUtils::FindUniqueSequence, moduleBase, moduleLength, Patterns::ptnsSeedRandomNumberGenerator, &pSeedRandomNumberGenerator);

		void *Host_AutoSave_f;
		ptnNumber = MemUtils::FindUniqueSequence(moduleHandle, moduleLength, Patterns::ptnsHost_AutoSave_f, &Host_AutoSave_f);
		if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
		{
			EngineDevMsg("[hw dll] Found Host_AutoSave_f at %p (using the %s pattern).\n", Host_AutoSave_f, Patterns::ptnsHost_AutoSave_f[ptnNumber].build.c_str());

			auto f = reinterpret_cast<uintptr_t>(Host_AutoSave_f);
			sv = *reinterpret_cast<void**>(f + 19);
			ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
				*reinterpret_cast<ptrdiff_t*>(f + 33)
				+ (f + 37)
				);
			cls = *reinterpret_cast<void**>(f + 69);
			EngineDevMsg("[hw dll] Found sv at %p.\n", sv);
			EngineDevMsg("[hw dll] Found Con_Printf at %p.\n", ORIG_Con_Printf);
			EngineDevMsg("[hw dll] Found cls at %p.\n", cls);

			ptnNumber = fCbuf_Execute.get();
			if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
			{
				ORIG_Cbuf_Execute = reinterpret_cast<_Cbuf_Execute>(pCbuf_Execute);
				EngineDevMsg("[hw dll] Found Cbuf_Execute at %p (using the %s pattern).\n", pCbuf_Execute, Patterns::ptnsCbuf_Execute[ptnNumber].build.c_str());
			}
			else
			{
				EngineDevWarning("[hw dll] Could not find Cbuf_Execute!\n");
			}

			ptnNumber = fCbuf_InsertText.get();
			if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
			{
				ORIG_Cbuf_InsertText = reinterpret_cast<_Cbuf_InsertText>(pCbuf_InsertText);
				EngineDevMsg("[hw dll] Found Cbuf_InsertText at %p (using the %s pattern).\n", pCbuf_InsertText, Patterns::ptnsCbuf_InsertText[ptnNumber].build.c_str());
			}
			else
			{
				EngineDevWarning("[hw dll] Could not find pCbuf_InsertText!\n");
				ORIG_Cbuf_Execute = nullptr;
			}

			ptnNumber = fSeedRandomNumberGenerator.get();
			if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
			{
				ORIG_SeedRandomNumberGenerator = reinterpret_cast<_SeedRandomNumberGenerator>(pSeedRandomNumberGenerator);
				EngineDevMsg("[hw dll] Found SeedRandomNumberGenerator at %p (using the %s pattern).\n", pSeedRandomNumberGenerator, Patterns::ptnsSeedRandomNumberGenerator[ptnNumber].build.c_str());

				ORIG_time = reinterpret_cast<_time>(
					*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(pSeedRandomNumberGenerator) + 3)
					+ reinterpret_cast<uintptr_t>(pSeedRandomNumberGenerator) + 7
				);
				EngineDevMsg("[hw dll] ORIG_time is %p.\n", ORIG_time);
			}
			else
			{
				EngineDevWarning("[hw dll] Could not find SeedRandomNumberGenerator!\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}
		else
		{
			EngineDevMsg("[hw dll] Could not find Host_AutoSave_f!\n");
		}
	}

	if (ORIG_Cbuf_Execute && !ORIG_time)
		ORIG_time = time;

	if (needToIntercept)
		MemUtils::Intercept(moduleName, {
			{ reinterpret_cast<void**>(&ORIG_Cbuf_Execute), reinterpret_cast<void*>(HOOKED_Cbuf_Execute) },
			{ reinterpret_cast<void**>(&ORIG_SeedRandomNumberGenerator), reinterpret_cast<void*>(HOOKED_SeedRandomNumberGenerator) },
			{ reinterpret_cast<void**>(&ORIG_time), reinterpret_cast<void*>(HOOKED_time) }
	});
}

void HwDLL::Unhook()
{
	if (m_Intercepted)
		MemUtils::RemoveInterception(m_Name, {
			{ reinterpret_cast<void**>(&ORIG_Cbuf_Execute), reinterpret_cast<void*>(HOOKED_Cbuf_Execute) },
			{ reinterpret_cast<void**>(&ORIG_SeedRandomNumberGenerator), reinterpret_cast<void*>(HOOKED_SeedRandomNumberGenerator) },
			{ reinterpret_cast<void**>(&ORIG_time), reinterpret_cast<void*>(HOOKED_time) }
	});

	Clear();
}

void HwDLL::Clear()
{
	ORIG_Cbuf_Execute = nullptr;
	ORIG_SeedRandomNumberGenerator = nullptr;
	ORIG_time = nullptr;
	ORIG_Cbuf_InsertText = nullptr;
	ORIG_Con_Printf = nullptr;
	cls = nullptr;
	sv = nullptr;
	insideSeedRNG = false;
}

HOOK_DEF_0(HwDLL, void, __cdecl, Cbuf_Execute)
{
	int state = *reinterpret_cast<int*>(cls);
	int paused = *(reinterpret_cast<int*>(sv) + 1);

	ORIG_Con_Printf("Cbuf_Execute() begin; cls.state: %d; sv.paused: %d; time: %f\n", state, paused, *reinterpret_cast<double*>(reinterpret_cast<uintptr_t>(sv) + 16));

	// If cls.state == 4 and the game isn't paused, execute "pause" right now.
	// This case happens when loading a savegame.
	if (state == 4 && !paused)
		ORIG_Cbuf_InsertText("pause\n");

	if (state == 5 && paused)
		ORIG_Cbuf_InsertText("+attack\n");

	ORIG_Cbuf_Execute();

	// If cls.state == 3 and the game isn't paused, execute "pause" on the next cycle.
	// This case happens when starting a map.
	static bool dontPauseNextCycle = false;
	if (!dontPauseNextCycle && state == 3 && !paused)
	{
		ORIG_Cbuf_InsertText("pause\n");
		dontPauseNextCycle = true;
	}
	else
		dontPauseNextCycle = false;
}

HOOK_DEF_0(HwDLL, void, __cdecl, SeedRandomNumberGenerator)
{
	insideSeedRNG = true;
	EngineMsg("Calling SeedRandomNumberGenerator!\n");
	ORIG_SeedRandomNumberGenerator();
	insideSeedRNG = false;
}

HOOK_DEF_1(HwDLL, time_t, __cdecl, time, time_t*, Time)
{
	if (insideSeedRNG)
	{
		EngineMsg("Called time from SeedRandomNumberGenerator!\n");
		return 0;
	}

	return ORIG_time(Time);
}
