#pragma once

#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>
#endif
#if defined(__GNUC__)
#define __cdecl __attribute__((cdecl))
#define __fastcall __attribute__((fastcall))
#define __stdcall
#endif

#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <array>
#include <chrono>
#include <fstream>
#include <future>
#include <iomanip>
#include <sstream>
#include <unordered_map>

using std::uintptr_t;
using std::size_t;
using std::ptrdiff_t;

#include "HLSDK/dlls/extdll.h"
#include "HLSDK/cl_dll/wrect.h"
#include "HLSDK/cl_dll/hud.h"
#include "HLSDK/common/ref_params.h"

typedef int(*pfnUserMsgHook)(const char *pszName, int iSize, void *pbuf);
#include "HLSDK/engine/cdll_int.h"

/*
	Declare a hook. Does the following:
	- Declares a public static function;
	- Declares a public function;
	- Assigns the type of the function to hook a nice name;
	- Declares a variable of that name.

	Example:
		HOOK_DECL(void, __cdecl, PM_PlayerMove, qboolean server)
	Expands into:
		public:
			static void __cdecl HOOKED_PM_PlayerMove(qboolean server);
			void __cdecl HOOKED_PM_PlayerMove_Func(qboolean server);
		protected:
			typedef void(__cdecl *_PM_PlayerMove) (qboolean server);
			_PM_PlayerMove ORIG_PlayerMove;
*/
#define HOOK_DECL(ret, call, name, ...) \
	public: \
		static ret call HOOKED_##name(__VA_ARGS__); \
		ret call HOOKED_##name##_Func(__VA_ARGS__); \
	protected: \
		typedef	ret(call *_##name) (__VA_ARGS__); \
		_##name ORIG_##name;

/*
	Define a hook. Does the following:
	- Defines the static function to call the member function;
	- Inserts the member function definition.

	Example:
		HOOK_DEF_1(ServerDLL, void, __cdecl, PM_PlayerMove, qboolean, server)
		{
			// do stuff
		}
	Becomes:
		void __cdecl ServerDLL::HOOKED_PM_PlayerMove(qboolean server)
		{
			return ServerDLL::GetInstance().HOOKED_PM_PlayerMove_Func(server);
		}
		void __cdecl ServerDLL::HOOKED_PM_PlayerMove_Func(qboolean server)
		{
			// do stuff
		}
*/

#define HOOK_DEF_0(class, ret, call, name) \
	ret call class::HOOKED_##name() { \
		return class::GetInstance().HOOKED_##name##_Func(); \
	} \
	ret call class::HOOKED_##name##_Func()

#define HOOK_DEF_1(class, ret, call, name, t1, n1) \
	ret call class::HOOKED_##name(t1 n1) { \
		return class::GetInstance().HOOKED_##name##_Func(n1); \
	} \
	ret call class::HOOKED_##name##_Func(t1 n1)

#define HOOK_DEF_2(class, ret, call, name, t1, n1, t2, n2) \
	ret call class::HOOKED_##name(t1 n1, t2 n2) { \
		return class::GetInstance().HOOKED_##name##_Func(n1, n2); \
	} \
	ret call class::HOOKED_##name##_Func(t1 n1, t2 n2)

#define HOOK_DEF_3(class, ret, call, name, t1, n1, t2, n2, t3, n3) \
	ret call class::HOOKED_##name(t1 n1, t2 n2, t3 n3) { \
		return class::GetInstance().HOOKED_##name##_Func(n1, n2, n3); \
	} \
	ret call class::HOOKED_##name##_Func(t1 n1, t2 n2, t3 n3)
