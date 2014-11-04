#pragma once

#include <cmath>
#include <cstddef>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "HLSDK\dlls\extdll.h"
#include "HLSDK\cl_dll\wrect.h"
#include "HLSDK\cl_dll\hud.h"

typedef int(*pfnUserMsgHook)(const char *pszName, int iSize, void *pbuf);
#include "HLSDK\engine\cdll_int.h"
