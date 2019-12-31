#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include "SDL.hpp"

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

	ORIG_SDL_GetMouseState = reinterpret_cast<_SDL_GetMouseState>(MemUtils::GetSymbolAddress(m_Handle, "SDL_GetMouseState"));
	if (ORIG_SDL_GetMouseState) {
		EngineDevMsg("[sdl] Found SDL_GetMouseState at %p.\n", ORIG_SDL_GetMouseState);
	} else {
		EngineDevWarning("[sdl] Could not find SDL_GetMouseState.\n");
	}
}

void SDL::Unhook()
{
	Clear();
}

void SDL::Clear()
{
	IHookableNameFilter::Clear();
	ORIG_SDL_SetRelativeMouseMode = nullptr;
	ORIG_SDL_GetMouseState = nullptr;
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
