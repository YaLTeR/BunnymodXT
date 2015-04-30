#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/Hooks.hpp>
#include "../bunnymodxt.hpp"
#include "../modules.hpp"
#include "conutils.hpp"

const wchar_t EVENT_NAME[] = L"BunnymodXT-Injector";

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

	if (boost::interprocess::message_queue::remove("BunnymodXT-TASView")) {
		EngineDevMsg("Cleaned up the old message queue.\n");
	}
	try {
		boost::interprocess::message_queue mq(
			boost::interprocess::create_only,
			"BunnymodXT-TASView",
			1000,
			256);

		EngineDevMsg("Created the message queue successfully.\n");
	} catch (boost::interprocess::interprocess_exception &ex) {
		EngineWarning("Failed to create the message queue, TASView integration is not available.\n");
	}

	Hooks::AddToHookedModules(&HwDLL::GetInstance());
	Hooks::AddToHookedModules(&ClientDLL::GetInstance());
	Hooks::AddToHookedModules(&ServerDLL::GetInstance());
	Hooks::Init(true);

	auto resume_event = OpenEventW(EVENT_MODIFY_STATE, FALSE, EVENT_NAME);
	if (resume_event != NULL) {
		SetEvent(resume_event);
		CloseHandle(resume_event);
		EngineDevMsg("Signaled the injector to resume the process.\n");
	}

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

		if (boost::interprocess::message_queue::remove("BunnymodXT-TASView")) {
			EngineDevMsg("Closed the message queue.\n");
		}

		ConUtils::Free();
		break;
	}

	return TRUE;
}
