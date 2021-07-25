#include "../stdafx.hpp"

#include "../bunnymodxt.hpp"
#include "../sptlib-wrapper.hpp"
#include <SPTLib/Hooks.hpp>
#include "../modules.hpp"
#include "../interprocess.hpp"

static FILE *logfile = nullptr;

static void Log(const char* prefix, const char* msg)
{
	if (logfile)
	{
		auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		auto ltime = std::localtime(&time);
		fprintf(logfile, "[%02d:%02d:%02d] [%s] %s", ltime->tm_hour, ltime->tm_min, ltime->tm_sec, prefix, msg);
		fflush(logfile);
	}
}

static void PrintMessage(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf(temp, format, args);

	va_end(args);

	printf("[Msg] %s", temp);
	Log("Msg", temp);
}

static void PrintDevMessage(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf(temp, format, args);

	va_end(args);

	printf("[DevMsg] %s", temp);
	Log("DevMsg", temp);
}

static void PrintWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf(temp, format, args);

	va_end(args);

	printf("[Warning] %s", temp);
	Log("Warning", temp);
}

static void PrintDevWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf(temp, format, args);

	va_end(args);

	printf("[DevWarning] %s", temp);
	Log("DevWarning", temp);
}

// Constructor priority must be higher than that of `modules` in SPTLib/Hooks.cpp
static __attribute__((constructor(1337))) void Construct()
{
	puts("Bunnymod XT version " BUNNYMODXT_VERSION ".");

	auto bxtLogfile = getenv("BXT_LOGFILE");
	if (bxtLogfile)
	{
		logfile = fopen(bxtLogfile, "a");
		if (!logfile)
			perror("Could not open the BXT logfile");
	}

	_EngineMsg = PrintMessage;
	_EngineDevMsg = PrintDevMessage;
	_EngineWarning = PrintWarning;
	_EngineDevWarning = PrintDevWarning;

	Interprocess::Initialize();

	Hooks::AddToHookedModules(&HwDLL::GetInstance());
	Hooks::AddToHookedModules(&ClientDLL::GetInstance());
	Hooks::AddToHookedModules(&ServerDLL::GetInstance());
	Hooks::AddToHookedModules(&SDL::GetInstance());
	Hooks::Init(false);
}

// Destructor priority must be the same as constructor.
static __attribute__((destructor(1337))) void Destruct()
{
	Hooks::Free();

	if (logfile)
		fclose(logfile);

	Interprocess::Shutdown();
}
