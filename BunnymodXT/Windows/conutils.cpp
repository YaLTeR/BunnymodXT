#include "stdafx.hpp"

#include "conutils.hpp"

namespace ConUtils
{
    HANDLE hConsoleOutput = INVALID_HANDLE_VALUE;
    WORD wStandartAttributes;

    void Init()
    {
        AllocConsole();

        hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hConsoleOutput == INVALID_HANDLE_VALUE)
			return;

        CONSOLE_SCREEN_BUFFER_INFO coninfo;
        GetConsoleScreenBufferInfo(hConsoleOutput, &coninfo);
        wStandartAttributes = coninfo.wAttributes;
        coninfo.dwSize.X = 150;
        coninfo.dwSize.Y = 500;
        SetConsoleScreenBufferSize(hConsoleOutput, coninfo.dwSize);

        SetConsoleTitle(L"Bunnymod XT Debug Console");

        ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
        ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
    }

    void Free()
    {
        FreeConsole();
    }

    void Log(const char *szText)
    {
		if (hConsoleOutput != INVALID_HANDLE_VALUE)
			WriteConsoleA(hConsoleOutput, szText, strlen(szText), NULL, NULL);
    }

    void Log(const char *szText, WORD wAttributes)
    {
		if (hConsoleOutput != INVALID_HANDLE_VALUE) {
			SetConsoleTextAttribute(hConsoleOutput, wAttributes);
			WriteConsoleA(hConsoleOutput, szText, strlen(szText), NULL, NULL);
			SetConsoleTextAttribute(hConsoleOutput, wStandartAttributes);
		}
    }

    void Log(const WCHAR *szText)
    {
		if (hConsoleOutput != INVALID_HANDLE_VALUE)
			WriteConsoleW(hConsoleOutput, szText, wcslen(szText), NULL, NULL);
    }

    void Log(const WCHAR *szText, WORD wAttributes)
    {
		if (hConsoleOutput != INVALID_HANDLE_VALUE) {
			SetConsoleTextAttribute(hConsoleOutput, wAttributes);
			WriteConsoleW(hConsoleOutput, szText, wcslen(szText), NULL, NULL);
			SetConsoleTextAttribute(hConsoleOutput, wStandartAttributes);
		}
    }
}
