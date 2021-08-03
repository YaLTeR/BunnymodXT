#pragma once

#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#include <Windows.h>
#include <process.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#endif

#if defined(__GNUC__)
#define __cdecl __attribute__((cdecl))
#define __fastcall __attribute__((fastcall))
#define __stdcall
#endif

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <utility>

using std::uintptr_t;
using std::size_t;
using std::ptrdiff_t;

#include <hltas.hpp>
#include <hlstrafe.hpp>

#include "HLSDK/dlls/extdll.h"
#include "HLSDK/cl_dll/wrect.h"
#include "HLSDK/cl_dll/hud.h"
#include "HLSDK/common/ref_params.h"
#include "HLSDK/common/usercmd.h"
#include "HLSDK/common/triangleapi.h"
#include "HLSDK/common/pmtrace.h"
#include "HLSDK/common/event_api.h"
#include "HLSDK/common/com_model.h"
#include "HLSDK/common/cl_entity.h"
#include "HLSDK/common/event_args.h"

typedef int(*pfnUserMsgHook)(const char *pszName, int iSize, void *pbuf);
#include "HLSDK/engine/cdll_int.h"

#ifndef M_PI
const double M_PI = 3.14159265358979323846;
#endif

const unsigned FCAP_CUSTOMSAVE = 0x00000001;
const unsigned FCAP_ACROSS_TRANSITION = 0x00000002;
const unsigned FCAP_MUST_SPAWN = 0x00000004;
const unsigned FCAP_DONT_SAVE = 0x80000000;
const unsigned FCAP_IMPULSE_USE = 0x00000008;
const unsigned FCAP_CONTINUOUS_USE = 0x00000010;
const unsigned FCAP_ONOFF_USE = 0x00000020;
const unsigned FCAP_DIRECTIONAL_USE = 0x00000040;
const unsigned FCAP_MASTER = 0x00000080;

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
		ret HOOKED_##name##_Func(__VA_ARGS__); \
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
	ret class::HOOKED_##name##_Func()

#define HOOK_DEF_1(class, ret, call, name, t1, n1) \
	ret call class::HOOKED_##name(t1 n1) { \
		return class::GetInstance().HOOKED_##name##_Func(n1); \
	} \
	ret class::HOOKED_##name##_Func(t1 n1)

#define HOOK_DEF_2(class, ret, call, name, t1, n1, t2, n2) \
	ret call class::HOOKED_##name(t1 n1, t2 n2) { \
		return class::GetInstance().HOOKED_##name##_Func(n1, n2); \
	} \
	ret class::HOOKED_##name##_Func(t1 n1, t2 n2)

#define HOOK_DEF_3(class, ret, call, name, t1, n1, t2, n2, t3, n3) \
	ret call class::HOOKED_##name(t1 n1, t2 n2, t3 n3) { \
		return class::GetInstance().HOOKED_##name##_Func(n1, n2, n3); \
	} \
	ret class::HOOKED_##name##_Func(t1 n1, t2 n2, t3 n3)

#define HOOK_DEF_4(class, ret, call, name, t1, n1, t2, n2, t3, n3, t4, n4) \
	ret call class::HOOKED_##name(t1 n1, t2 n2, t3 n3, t4 n4) { \
		return class::GetInstance().HOOKED_##name##_Func(n1, n2, n3, n4); \
	} \
	ret class::HOOKED_##name##_Func(t1 n1, t2 n2, t3 n3, t4 n4)

#define HOOK_DEF_5(class, ret, call, name, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5) \
	ret call class::HOOKED_##name(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5) { \
		return class::GetInstance().HOOKED_##name##_Func(n1, n2, n3, n4, n5); \
	} \
	ret class::HOOKED_##name##_Func(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5)

#define HOOK_DEF_6(class, ret, call, name, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6) \
	ret call class::HOOKED_##name(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6) { \
		return class::GetInstance().HOOKED_##name##_Func(n1, n2, n3, n4, n5, n6); \
	} \
	ret class::HOOKED_##name##_Func(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6)

#define HOOK_DEF_7(class, ret, call, name, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7) \
	ret call class::HOOKED_##name(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7) { \
		return class::GetInstance().HOOKED_##name##_Func(n1, n2, n3, n4, n5, n6, n7); \
	} \
	ret class::HOOKED_##name##_Func(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7)

#define HOOK_DEF_11(class, ret, call, name, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11) \
	ret call class::HOOKED_##name(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11) { \
		return class::GetInstance().HOOKED_##name##_Func(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11); \
	} \
	ret class::HOOKED_##name##_Func(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11)
