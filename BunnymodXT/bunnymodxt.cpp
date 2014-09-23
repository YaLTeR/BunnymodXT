#include "stdafx.hpp"

#include "sptlib-wrapper.hpp"
#include <SPTLib\Hooks.hpp>
#include "conutils.hpp"
#include "modules.hpp"

#define BUNNYMODXT_VERSION "0.3-beta"

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

unsigned int __stdcall MainThread(void *args)
{
	ConUtils::Init();
	ConUtils::Log("Bunnymod XT version " BUNNYMODXT_VERSION ".\n", FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

	_EngineMsg = PrintMessage;
	_EngineDevMsg = PrintDevMessage;
	_EngineWarning = PrintWarning;
	_EngineDevWarning = PrintDevWarning;

	Hooks::getInstance().AddToHookedModules(&clientDLL);
	Hooks::getInstance().AddToHookedModules(&serverDLL);
	Hooks::getInstance().Init();

	return 0;
}
