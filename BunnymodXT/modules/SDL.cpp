#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include "../simulation_ipc.hpp"
#include "SDL.hpp"

#ifndef _WIN32
extern "C" int __cdecl SDL_WaitEventTimeout(void *event, int time)
{
	return SDL::HOOKED_SDL_WaitEventTimeout(event, time);
}

extern "C" void __cdecl SDL_WarpMouseInWindow(void *window, int x, int y)
{
	return SDL::HOOKED_SDL_WarpMouseInWindow(window, x, y);
}
#endif

void SDL::Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept)
{
	Clear(); // Just in case.

	m_Handle = moduleHandle;
	m_Base = moduleBase;
	m_Length = moduleLength;
	m_Name = moduleName;
	m_Intercepted = needToIntercept;

	ORIG_SDL_SetRelativeMouseMode = reinterpret_cast<_SDL_SetRelativeMouseMode>(MemUtils::GetSymbolAddress(m_Handle, "SDL_SetRelativeMouseMode"));
	if (ORIG_SDL_SetRelativeMouseMode) {
		EngineDevMsg("[sdl] Found SDL_SetRelativeMouseMode at %p.\n", ORIG_SDL_SetRelativeMouseMode);
	} else {
		EngineDevWarning("[sdl] Could not find SDL_SetRelativeMouseMode.\n");
	}

	ORIG_SDL_WarpMouseInWindow = reinterpret_cast<_SDL_WarpMouseInWindow>(MemUtils::GetSymbolAddress(m_Handle, "SDL_WarpMouseInWindow"));
	if (ORIG_SDL_WarpMouseInWindow) {
		EngineDevMsg("[sdl] Found SDL_WarpMouseInWindow at %p.\n", ORIG_SDL_WarpMouseInWindow);
	} else {
		EngineDevWarning("[sdl] Could not find SDL_WarpMouseInWindow.\n");
	}

	ORIG_SDL_GetMouseState = reinterpret_cast<_SDL_GetMouseState>(MemUtils::GetSymbolAddress(m_Handle, "SDL_GetMouseState"));
	if (ORIG_SDL_GetMouseState) {
		EngineDevMsg("[sdl] Found SDL_GetMouseState at %p.\n", ORIG_SDL_GetMouseState);
	} else {
		EngineDevWarning("[sdl] Could not find SDL_GetMouseState.\n");
	}

	ORIG_SDL_WaitEventTimeout = reinterpret_cast<_SDL_WaitEventTimeout>(MemUtils::GetSymbolAddress(m_Handle, "SDL_WaitEventTimeout"));
	if (ORIG_SDL_WaitEventTimeout) {
		EngineDevMsg("[sdl] Found SDL_WaitEventTimeout at %p.\n", ORIG_SDL_WaitEventTimeout);
	} else {
		EngineDevWarning("[sdl] Could not find SDL_WaitEventTimeout.\n");
	}

	if (needToIntercept) {
		MemUtils::Intercept(
			moduleName,
			ORIG_SDL_WaitEventTimeout, HOOKED_SDL_WaitEventTimeout,
			ORIG_SDL_WarpMouseInWindow, HOOKED_SDL_WarpMouseInWindow);
	}
}

void SDL::Unhook()
{
	if (m_Intercepted) {
		MemUtils::RemoveInterception(
			m_Name,
			ORIG_SDL_WaitEventTimeout,
			ORIG_SDL_WarpMouseInWindow);
	}

	Clear();
}

void SDL::Clear()
{
	IHookableNameFilter::Clear();
	ORIG_SDL_SetRelativeMouseMode = nullptr;
	ORIG_SDL_WarpMouseInWindow = nullptr;
	ORIG_SDL_GetMouseState = nullptr;
	ORIG_SDL_WaitEventTimeout = nullptr;
}

bool SDL::Found() const
{
	return ORIG_SDL_SetRelativeMouseMode != nullptr && ORIG_SDL_GetMouseState != nullptr;
}

void SDL::SetRelativeMouseMode(bool relative) const
{
	if (ORIG_SDL_SetRelativeMouseMode != nullptr)
		ORIG_SDL_SetRelativeMouseMode(relative ? 1 : 0);
}

uint32_t SDL::GetMouseState(int *x, int *y) const
{
	if (ORIG_SDL_GetMouseState != nullptr)
		return ORIG_SDL_GetMouseState(x, y);

	*x = 0;
	*y = 0;
	return 0;
}

HOOK_DEF_2(SDL, int, __cdecl, SDL_WaitEventTimeout, void*, event, int, time)
{
	return ORIG_SDL_WaitEventTimeout(event, 0);
}

HOOK_DEF_3(SDL, void, __cdecl, SDL_WarpMouseInWindow, void*, window, int, x, int, y)
{
	// If we're the simulator client, don't mess with the mouse.
	if (simulation_ipc::is_client_initialized())
		return;

	ORIG_SDL_WarpMouseInWindow(window, x, y);
}
