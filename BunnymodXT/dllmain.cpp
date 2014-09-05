#include "stdafx.hpp"
#include <process.h>

#include <SPTLib\Hooks.hpp>
#include "bunnymodxt.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		_beginthreadex(NULL, 0, MainThread, NULL, 0, NULL);
		break;

	case DLL_PROCESS_DETACH:
		Hooks::getInstance().Free();
		break;
	}

	return TRUE;
}
