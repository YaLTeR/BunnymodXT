#pragma once

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cmath>
#include <cstddef>

#include <fstream>
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
