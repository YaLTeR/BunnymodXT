#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/Hooks.hpp>
#include "../bunnymodxt.hpp"
#include "../modules.hpp"
#include "conutils.hpp"

void PrintMessage(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf_s(temp, format, args);

	va_end(args);

	ConUtils::Log(temp);
}

void PrintDevMessage(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf_s(temp, format, args);

	va_end(args);

	ConUtils::Log(temp, FOREGROUND_RED | FOREGROUND_GREEN);
}

void PrintWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf_s(temp, format, args);

	va_end(args);

	ConUtils::Log(temp, FOREGROUND_RED | FOREGROUND_INTENSITY);
}

void PrintDevWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf_s(temp, format, args);

	va_end(args);

	ConUtils::Log(temp, FOREGROUND_RED);
}

unsigned int __stdcall MainThread(void* args)
{
	ConUtils::Init();
	ConUtils::Log("Bunnymod XT version " BUNNYMODXT_VERSION ".\n", FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

	_EngineMsg = PrintMessage;
	_EngineDevMsg = PrintDevMessage;
	_EngineWarning = PrintWarning;
	_EngineDevWarning = PrintDevWarning;

	Hooks::AddToHookedModules(&HwDLL::GetInstance());
	Hooks::AddToHookedModules(&ClientDLL::GetInstance());
	Hooks::AddToHookedModules(&ServerDLL::GetInstance());
	Hooks::Init(true);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		_beginthreadex(NULL, 0, MainThread, NULL, 0, NULL);
		break;

	case DLL_PROCESS_DETACH:
		Hooks::Free();
		ConUtils::Free();
		break;
	}

	return TRUE;
}
