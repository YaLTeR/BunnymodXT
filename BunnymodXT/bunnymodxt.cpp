#include "stdafx.hpp"

unsigned int __stdcall MainThread(void *args)
{
	MessageBox(NULL, TEXT("MainThread"), TEXT("Success"), MB_OK);
	return 0;
}
