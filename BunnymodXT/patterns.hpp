#pragma once

#include <SPTLib/MemUtils.hpp>
#include <SPTLib/patterns.hpp>

namespace patterns
{
	namespace engine
	{
		PATTERNS(LoadAndDecryptHwDLL,
			"HL-NGHL",
			"8B 0D ?? ?? ?? ?? 53 56 33 DB 8B 01 57 53 68 ?? ?? ?? ?? FF 74 24 18 FF 50 28 8B 0D ?? ?? ?? ?? 8B F0 6A 02 53 8B 01 56 FF 50 30 8B 0D",
			"HL-WON",
			"8B 44 24 04 53 56 57 68 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 8B F0 6A 02 6A 00 56 E8 ?? ?? ?? ?? 56 E8 ?? ?? ?? ?? 56 8B F8 E8"
		);

		PATTERNS(VGuiWrap2_ConDPrintf,
			"HL-SteamPipe",
			"55 8B EC A1 ?? ?? ?? ?? 85 C0 74 14 8B 55 08 8B 08 52 68 ?? ?? ?? ?? 50 FF 51 1C 83 C4 0C 5D C3 53 56 8B 75 08 57 8B FE 83 C9 FF 33 C0",
			"HL-4554",
			"A1 ?? ?? ?? ?? 85 C0 74 14 8B 54 24 04 8B 08 52 68 ?? ?? ?? ?? 50 FF 51 1C 83 C4 0C C3 53 56 8B 74 24 0C 57 8B FE 83 C9 FF 33 C0"
		);

		PATTERNS(VGuiWrap2_ConPrintf,
			"HL-SteamPipe",
			"55 8B EC A1 ?? ?? ?? ?? 85 C0 74 14 8B 55 08 8B 08 52 68 ?? ?? ?? ?? 50 FF 51 18 83 C4 0C 5D C3 53 56 8B 75 08 57 8B FE 83 C9 FF 33 C0",
			"HL-4554",
			"A1 ?? ?? ?? ?? 85 C0 74 14 8B 54 24 04 8B 08 52 68 ?? ?? ?? ?? 50 FF 51 18 83 C4 0C C3 53 56 8B 74 24 0C 57 8B FE 83 C9 FF 33 C0"
		);

		PATTERNS(Cbuf_Execute,
			"HL-SteamPipe-8183",
			"6A 01 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 00 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 10 C3",
			"HL-SteamPipe",
			"55 8B EC 81 EC 00 04 00 00 8B 15 ?? ?? ?? ?? 53 56 57 85 D2 0F 84 B5 00 00 00 8B 1D ?? ?? ?? ?? 33 C9 33 F6 85 D2 7E 22 8A 04 1E 3C 22 75 01 41",
			"HL-NGHL",
			"8B 15 ?? ?? ?? ?? 81 EC 00 04 00 00 53 33 DB 55 56 3B D3 57 0F 84 ?? ?? ?? ?? 8B 2D ?? ?? ?? ?? 33 C9 33 F6 3B D3 7E 22 8A 04 2E 3C 22 75 01 41",
			"HL-SteamPipe-8308",
			"6A 01 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 01 6A 00 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 14 C3"
		);

		PATTERNS(Cvar_RegisterVariable,
			"HL-SteamPipe",
			"55 8B EC 83 EC 14 53 56 8B 75 08 57 8B 06 50 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 17 8B 0E 51 68",
			"HL-NGHL",
			"83 EC 14 53 56 8B 74 24 20 57 8B 06 50 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 17 8B 0E 51 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 5F 5E 5B 83 C4 14 C3 8B 16 52 E8"
		);

		PATTERNS(Cvar_DirectSet,
			"HL-SteamPipe",
			"55 8B EC 81 EC ?? ?? ?? ?? 56 8B 75 0C 57 8B 7D 08 85 FF 0F 84 ?? ?? ?? ?? 85 F6 0F 84 ?? ?? ?? ?? ?? 47 08",
			"HL-NGHL",
			"8B 4C 24 08 81 EC 00 04 00 00 56 8B B4 24 08 04 00 00 57 8B F9 85 F6 0F 84 ?? ?? ?? ?? 85 C9 0F 84 ?? ?? ?? ?? ?? 46 08",
			"HL-WON",
			"55 8B EC 81 EC 00 04 00 00 8B 4D 0C 56 8B 75 08 57 85 F6 8B F9 0F 84 ?? ?? ?? ?? 85 C9 0F 84"
		);

		PATTERNS(Cvar_FindVar,
			"HL-SteamPipe",
			"55 8B EC 8D 45 08 56 50 FF ?? ?? ?? ?? ?? 8B 35 ?? ?? ?? ?? 83 C4 04 85 F6 74 1A 8B 0E 8B 55 08 51 52 E8",
			"HL-NGHL",
			"8D 44 24 04 56 50 FF ?? ?? ?? ?? ?? 8B 35 ?? ?? ?? ?? 83 C4 04 85 F6 74 1B 8B 0E 8B 54 24 08 51 52 E8",
			"HL-WON",
			"56 8B 35 ?? ?? ?? ?? 85 F6 57 74 1B 8B 7C 24 0C 8B 06 50 57 E8 ?? ?? ?? ?? 83 C4 08 85 C0 74 0C"
		);

		PATTERNS(Cbuf_InsertText,
			"HL-SteamPipe-8183",
			"55 8B EC 8B 45 08 6A 00 68 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 0C 5D C3",
			"HL-SteamPipe",
			"55 8B EC 53 8B 5D 08 56 8B 35 ?? ?? ?? ?? 57 53 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 03 C6 83 C4 04 3B C1 7C 12 68 ?? ?? ?? ?? E8",
			"HL-NGHL",
			"53 8B 5C 24 08 56 8B 35 ?? ?? ?? ?? 57 53 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 03 C6 83 C4 04 3B C1 7C"
		);

		PATTERNS(Cbuf_AddText,
			"HL-SteamPipe-8183",
			"55 8B EC 8B 45 08 68 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 08 5D C3 90 90 90 90 90 90 90 90 90 90 55 8B EC 56 57 8B 7D 08 57 E8",
			"HL-SteamPipe",
			"55 8B EC 56 8B 75 08 56 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 83 C4 04 03 C8 A1 ?? ?? ?? ?? 3B C8 7C 10 68",
			"HL-NGHL",
			"56 8B 74 24 08 56 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 83 C4 04 03 C8 A1 ?? ?? ?? ?? 3B C8 7C",
			"HL-WON",
			"56 8B 74 24 08 68 ?? ?? ?? ?? 56 E8 ?? ?? ?? ?? 83 C4 08 85 C0 75 07 E8 ?? ?? ?? ?? 5E C3 68 ?? ?? ?? ?? 56 E8"
		);

		PATTERNS(Cmd_AddMallocCommand,
			"HL-SteamPipe",
			"55 8B EC 56 57 8B 7D 08 57 E8 ?? ?? ?? ?? 8A 08 83 C4 04 84 C9 74 12 57 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 5F 5E 5D C3 8B 35",
			"HL-NGHL",
			"56 57 8B 7C 24 0C 57 E8 ?? ?? ?? ?? 8A 08 83 C4 04 84 C9 74 11 57 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 5F 5E C3 8B 35"
		);

		PATTERNS(Host_AutoSave_f,
			"HL-SteamPipe",
			"A1 ?? ?? ?? ?? B9 01 00 00 00 3B C1 0F 85 9F 00 00 00 A1 ?? ?? ?? ?? 85 C0 75 10 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 33 C0 C3 39 0D"
		);

		PATTERNS(Host_Tell_f,
			"HL-SteamPipe",
			"55 8B EC 83 EC 40 A1 ?? ?? ?? ?? 56 83 F8 01 75 0A E8 ?? ?? ?? ?? 5E 8B E5 5D C3 E8 ?? ?? ?? ?? 83 F8 03 0F 8C ?? ?? ?? ?? E8",
			"HL-NGHL",
			"A1 ?? ?? ?? ?? 83 EC 40 83 F8 01 56 75 09 5E 83 C4 40 E9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 F8 03 0F 8C ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F0 85 F6 0F 84",
			"HL-4554",
			"A1 ?? ?? ?? ?? 83 EC 40 83 F8 01 56 75 0A E8 ?? ?? ?? ?? 5E 83 C4 40 C3 E8 ?? ?? ?? ?? 83 F8 03 0F 8C ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F0 85 F6 0F 84",
			"HL-WON",
			"A1 ?? ?? ?? ?? 83 EC 40 83 F8 01 56 75 0A E8 ?? ?? ?? ?? 5E 83 C4 40 C3 E8 ?? ?? ?? ?? 83 F8 03 0F 8C 82 01 00 00 A1 ?? ?? ?? ?? 8D 4C 24 04 05"
		);

		PATTERNS(Host_Changelevel2_f,
			"HL-SteamPipe",
			"55 8B EC 81 EC C4 00 00 00 53 56 33 DB 57 89 5D FC C7 05 ?? ?? ?? ?? 04 00 00 00 E8 ?? ?? ?? ?? 83 F8 02 7D 14 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E 5B 8B E5 5D C3 39 1D ?? ?? ?? ?? 0F 84 7F",
			"HL-WON",
			"55 8B EC 81 EC C4 00 00 00 53 56 33 DB 57 89 5D FC C7 05 ?? ?? ?? ?? 04 00 00 00 E8 ?? ?? ?? ?? 83 F8 02 7D 14 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E 5B 8B E5 5D C3 39 1D ?? ?? ?? ?? 0F 84 6D",
			"HL-NGHL",
			"81 EC C0 00 00 00 C7 05 ?? ?? ?? ?? 04 00 00 00 55 56 57 33 ED E8 ?? ?? ?? ?? 83 F8 02 7D 17 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E 5D 81 C4 C0 00 00 00 C3 A1 ?? ?? ?? ?? 85 C0 0F 84 8B",
			"HL-WON-2",
			"81 EC C0 00 00 00 C7 05 ?? ?? ?? ?? 04 00 00 00 55 56 57 33 ED E8 ?? ?? ?? ?? 83 F8 02 7D 17 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E 5D 81 C4 C0 00 00 00 C3 A1 ?? ?? ?? ?? 85 C0 0F 84 7F",
			"HL-WON-3",
			"81 EC C0 00 00 00 C7 05 ?? ?? ?? ?? 04 00 00 00 53 56 57 33 DB E8 ?? ?? ?? ?? 83 F8 02 7D 17 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E 5B 81 C4 C0 00 00 00 C3 A1 ?? ?? ?? ?? 85 C0 0F 84 60"
		);

		PATTERNS(SCR_BeginLoadingPlaque,
			"HL-SteamPipe",
			"6A 01 E8 ?? ?? ?? ?? A1 ?? ?? ?? ?? 83 C4 04 83 F8 03 74 0A 83 F8 04 74 05 83 F8 05 75 ?? 83 3D ?? ?? ?? ?? 02 75 ?? 56 E8"
		);

		PATTERNS(Host_FilterTime,
			"HL-SteamPipe",
			"55 8B EC 83 EC 08 D9 05 ?? ?? ?? ?? D8 1D ?? ?? ?? ?? DF E0 25 00 41 00 00 75 3F E8 ?? ?? ?? ?? 85 C0 75 09 A1",
			"HL-4554",
			"55 8B EC 83 E4 F8 83 EC 08 D9 05"
		);

		PATTERNS(SeedRandomNumberGenerator,
			"HL-SteamPipe",
			"6A 00 E8 ?? ?? ?? ?? F7 D8 83 C4 04 3D E8 03 00 00 A3 ?? ?? ?? ?? 7E 08 F7 D8 A3 ?? ?? ?? ?? C3 3D 18 FC FF FF 7E 0A 2D 38 AD 53 01 A3 ?? ?? ?? ?? C3"
		);

		PATTERNS(RandomFloat,
			"HL-SteamPipe",
			"55 8B EC 51 8D 45 0C 8D 4D 08 50 51 FF 15 ?? ?? ?? ?? 83 C4 08 E8",
			"HL-NGHL",
			"51 8D 44 24 0C 8D 4C 24 08 50 51 FF 15 ?? ?? ?? ?? E8"
		);

		PATTERNS(RandomLong,
			"HL-SteamPipe",
			"55 8B EC 53 8D 45 0C 56 8D 4D 08 50 51 FF 15 ?? ?? ?? ?? 8B 75 0C",
			"HL-NGHL",
			"53 8D 44 24 0C 56 8D 4C 24 0C 50 51 FF 15 ?? ?? ?? ?? 8B 74 24 18",
			"HL-4554",
			"8D 44 24 ?? 57 8D 4C 24 ?? 50 51 FF 15 ?? ?? ?? ?? 8B 7C 24"
		);

		PATTERNS(SCR_DrawFPS,
			"HL-SteamPipe",
			"D9 05 ?? ?? ?? ?? D8 1D ?? ?? ?? ?? DF E0 F6 C4 ?? ?? ?? DD 05 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? DF E0 F6 C4 41 ?? ?? DD 05"
		);

		PATTERNS(PM_PlayerTrace,
			"HL-SteamPipe",
			"55 8B EC 83 EC 44 8B 45 18 56 57 6A 00 50 A1 ?? ?? ?? ?? 8B 90 4C 02 00 00 8D 88 50 02 00 00 8B 45 14 51 8B 4D 10 52 8B 55 0C 50 51 8D 45 BC 52 50 E8",
			"HL-NGHL",
			"8B 44 24 14 83 EC 44 56 57 6A 00 50 A1 ?? ?? ?? ?? 8B 90 4C 02 00 00 8D 88 50 02 00 00 8B 44 24 64 51 8B 4C 24 64 52 8B 54 24 64 50 51 8D 44 24 20 52 50 E8"
		);

		PATTERNS(MiddleOfSV_ReadClientMessage,
			"HL-SteamPipe",
			"56 E8 ?? ?? ?? ?? 8B 86 9C 4B 00 00 89 35 ?? ?? ?? ?? A3 ?? ?? ?? ?? C7 86 44 25 00 00 FF FF FF FF C7 05",
			"HL-4554",
			"56 E8 ?? ?? ?? ?? 8B 86 9C 4B 00 00 83 C4 04 89 35 ?? ?? ?? ?? A3 ?? ?? ?? ?? C7 86 44 25 00 00 FF FF FF FF C7 05",
			"HL-WON",
			"56 E8 ?? ?? ?? ?? 8B 96 9C 4B 00 00 89 35 ?? ?? ?? ?? 89 15 ?? ?? ?? ?? C7 86 44 25 00 00 FF FF FF FF C7 05",
			"HL-WON-1712",
			"56 E8 ?? ?? ?? ?? 8B 96 94 4B 00 00 83 C4 04 89 35 ?? ?? ?? ?? 89 15 ?? ?? ?? ?? C7 86 44 25 00 00 FF FF FF FF C7 05"
		);

		PATTERNS(MiddleOfSV_RunCmd,
			"HL-SteamPipe",
			"8B 88 90 00 00 00 89 4A 40 8B 15 ?? ?? ?? ?? 83 C2 38 52 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 A1"
		);

		PATTERNS(V_FadeAlpha,
			"HL-SteamPipe",
			"55 8B EC 83 EC 08 D9 05 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? 8A 0D ?? ?? ?? ?? DF E0 F6 C4 05 7A 1C D9 05 ?? ?? ?? ?? DC 1D",
			"HL-4554",
			"D9 05 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? 8A 0D ?? ?? ?? ?? 83 EC 08 DF E0 F6 C4"
		);

		PATTERNS(SCR_UpdateScreen,
			"HL-SteamPipe",
			"55 8B EC 83 EC 10 A1 ?? ?? ?? ?? 56 33 F6 3B C6 0F 85 10 02 00 00 C7 05 ?? ?? ?? ?? 01 00 00 00 E8",
			"HL-4554",
			"A1 ?? ?? ?? ?? 83 EC 10 56 33 F6 3B C6 0F 85 ?? ?? ?? ?? C7 05 ?? ?? ?? ?? 01 00 00 00 E8"
		);

		PATTERNS(VGuiWrap_Paint,
			"HL-SteamPipe",
			"55 8B EC 83 EC 10 56 E8 ?? ?? ?? ?? 8B F0",
			"HL-NGHL",
			"D9 05 ?? ?? ?? ?? D8 1D ?? ?? ?? ?? 83 EC 10 DF E0 F6 C4 44 7B",
			"HL-4554",
			"D9 05 ?? ?? ?? ?? D8 1D ?? ?? ?? ?? 83 EC 10 DF E0 F6 C4 40 75"
		);

		PATTERNS(PF_GetPhysicsKeyValue,
			"HL-SteamPipe",
			"55 8B EC 8B 45 08 50 E8 ?? ?? ?? ?? 83 C4 04 83 F8 01 7C 33 3B 05 ?? ?? ?? ?? 7F 2B 8D 0C 80 C1 E1 08 03 C8 8D 14 48 A1",
			"HL-4554",
			"8B 44 24 04 50 E8 ?? ?? ?? ?? 83 C4 04 83 F8 01 7C 31 3B 05 ?? ?? ?? ?? 7F 29",
			"HL-WON",
			"55 8B EC 8B 45 08 50 E8 ?? ?? ?? ?? 83 C4 04 83 F8 01 7C 34 3B 05 ?? ?? ?? ?? 7F 2C 8B 15 ?? ?? ?? ?? 8D 0C 80 C1 E1 08"
		);

		PATTERNS(CL_Move,
			"HL-SteamPipe",
			"55 8B EC 81 EC 78 08 00 00 A1 ?? ?? ?? ?? 53 56 57 33 FF 3B C7 0F 84 49 06 00 00 83 F8 01 0F 84 40 06 00 00 83 F8 02 0F 84 37 06 00 00"
		);

		PATTERNS(SV_Frame,
			"HL-SteamPipe",
			"A1 ?? ?? ?? ?? 85 C0 74 ?? DD 05 ?? ?? ?? ?? A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? D9 1D ?? ?? ?? ?? A3 ?? ?? ?? ?? 89 0D"
		);

		PATTERNS(CL_Stop_f,
			"HL-SteamPipe",
			"55 8B EC 81 EC 18 02 00 00 A1 ?? ?? ?? ?? 56 83 F8 01 0F 85 ?? 02 00 00 A1 ?? ?? ?? ?? 33 F6 3B",
			"HL-NGHL",
			"A1 ?? ?? ?? ?? 81 EC 18 02 00 00 83 F8 01 ?? 0F 85 ?? 02 00 00 A1 ?? ?? ?? ?? 33 ?? 3B ?? 75 15"
		);

		PATTERNS(Host_Loadgame_f,
			"HL-SteamPipe",
			"83 3D ?? ?? ?? ?? 01 75 37 E8 ?? ?? ?? ?? 83 F8 02 74 0E 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04"
		);

		PATTERNS(Host_Reload_f,
			"HL-SteamPipe-8183",
			"55 8B EC 81 EC 04 01 00 00 A1 ?? ?? ?? ?? 85 C0 75 65 A1 ?? ?? ?? ?? 85 C0 74 5C 83 3D",
			"HL-SteamPipe",
			"55 8B EC 81 EC 04 01 00 00 A1 ?? ?? ?? ?? 85 C0 75 60 A1 ?? ?? ?? ?? 85 C0 74 57 83 3D",
			"HL-NGHL",
			"A1 ?? ?? ?? ?? 81 EC 04 01 00 00 85 C0 75 5E A1 ?? ?? ?? ?? 85 C0 74 55 83 3D ?? ?? ?? ?? 01 75"
		);

		PATTERNS(CL_RecordHUDCommand,
			"HL-SteamPipe",
			"55 8B EC 83 EC 4C C6 45 B4 03 E8 ?? ?? ?? ?? D8 25 ?? ?? ?? ?? 51 D9 1C 24 FF 15 ?? ?? ?? ?? A1",
			"HL-NGHL",
			"83 EC 4C C6 44 24 00 03 E8 ?? ?? ?? ?? D8 25 ?? ?? ?? ?? 51 D9 1C 24 FF 15 ?? ?? ?? ?? A1",
			"HL-WON",
			"83 EC 4C A1 ?? ?? ?? ?? 8D 4C 24 03 50 6A 01 6A 01 51 C6 44 24 13 03 E8 ?? ?? ?? ?? E8"
		);

		PATTERNS(CL_Record_f,
			"HL-SteamPipe",
			"55 8B EC 81 EC 18 03 00 00 53 56 57 E8 ?? ?? ?? ?? 83 F8 02 7D 14 68 ?? ?? ?? ?? E8",
			"HL-4554",
			"81 EC 18 03 00 00 53 55 56 57 E8 ?? ?? ?? ?? 83 F8 02 7D 18 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4",
			"HL-NGHL",
			"81 EC 18 03 00 00 ?? 56 57 E8 ?? ?? ?? ?? 83 F8 02 7D 17 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04"
		);

		PATTERNS(build_number,
			"HL-SteamPipe",
			"55 8B EC 83 EC 08 A1 ?? ?? ?? ?? 56 33 F6 85 C0 0F 85 9B 00 00 00 53 33 DB 8B 04 9D ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 6A 03 50 51 E8",
			"HL-4554",
			"A1 ?? ?? ?? ?? 83 EC 08 57 33 FF 85 C0 0F 85 A5 00 00 00 53 56 33 DB BE ?? ?? ?? ?? 8B 06 8B 0D",
			"HL-NGHL",
			"A1 ?? ?? ?? ?? 83 EC 08 56 33 F6 85 C0 0F 85 9F 00 00 00 53 33 DB 8B 04 9D ?? ?? ?? ?? 8B 0D"
		);

		PATTERNS(Key_Event,
			"HL-SteamPipe-8183",
			"55 8B EC 81 EC 00 04 00 00 8B 45 08 56 3D 00 01 00 00 57 0F 8D 91 03 00 00 8D 45 0C 8D 4D 08 50",
			"HL-SteamPipe",
			"55 8B EC 81 EC 00 04 00 00 8B 45 08 56 3D 00 01 00 00 57 0F 8D 8D 03 00 00 8D 45 0C 8D 4D 08 50",
			"HL-4554",
			"81 EC 00 04 00 00 8D 84 24 08 04 00 00 8D 8C 24 04 04 00 00 56 57 50 51 FF 15 ?? ?? ?? ?? 8B 84",
			"HL-WON",
			"81 EC 00 04 00 00 56 8B B4 24 08 04 00 00 57 8B BC 24 10 04 00 00 89 3C B5 ?? ?? ?? ?? A1",
			"HL-WON-2",
			"55 8B EC 81 EC 00 04 00 00 8D 45 0C 8D 4D 08 50 51 FF 15 ?? ?? ?? ?? 8B 45 08 8B 55 0C 83 C4 08"
		);

		PATTERNS(Cmd_Exec_f,
			"HL-SteamPipe",
			"55 8B EC 51 53 56 57 E8 ?? ?? ?? ?? 83 F8 02 74 14 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E",
			"HL-4554",
			"51 56 57 E8 ?? ?? ?? ?? 83 F8 02 74 11 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E 59 C3 6A 01",
			"HL-WON",
			"53 56 57 E8 ?? ?? ?? ?? 83 F8 02 74 11 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E 5B C3 6A 01",
			"HL-WON-2",
			"55 8B EC 51 56 57 E8 ?? ?? ?? ?? 83 F8 02 74 13 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E 8B",
			"HL-SteamPipe-8308",
			"55 8B EC 81 EC ?? ?? ?? ?? 53 56 57 E8 ?? ?? ?? ?? 83 F8 02 74 14 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5F 5E 5B 8B E5 5D C3 6A 01 E8 ?? ?? ?? ?? 8B F0 83 C4 04 85 F6 0F 84 ?? ?? ?? ?? 68"
		);

		PATTERNS(Cmd_ExecuteStringWithPrivilegeCheck,
			"HL-SteamPipe-8183",
			"55 8B EC 83 EC 14 8B 45 0C 53 56 57 8B 7D 08 A3 ?? ?? ?? ?? 57 E8 ?? ?? ?? ?? 83 C4 04 E8 ?? ?? ?? ?? 85 C0"
		);

		PATTERNS(Cmd_ExecuteString,
			"HL-SteamPipe",
			"55 8B EC 8B 4D 08 8B 45 0C 56 51 A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 E8 ?? ?? ?? ?? 85 C0 0F 84",
			"HL-4554",
			"8B 4C 24 04 8B 44 24 08 56 51 A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 E8 ?? ?? ?? ?? 85 C0 0F 84",
			"HL-NGHL",
			"8B 4C 24 04 8B 44 24 08 56 51 A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 E8 ?? ?? ?? ?? 85 C0 74 75"
		);

		PATTERNS(R_DrawSequentialPoly,
			"HL-SteamPipe",
			"55 8B EC 51 A1 ?? ?? ?? ?? 53 56 57 83 B8 F8 02 00 00 01 75 63 E8 ?? ?? ?? ?? 68 03 03 00 00 68 02 03 00 00",
			"HL-4554",
			"A1 ?? ?? ?? ?? 53 55 56 8B 88",
			"OpposingForce-1600",
			"A1 ?? ?? ?? ?? 53 55 BD 01 00 00 00 8B 88 F8 02 00 00 56 3B CD 57 75 62 E8 ?? ?? ?? ?? 68 03 03 00 00 68 02 03 00 00"
		);

		PATTERNS(R_DrawViewModel,
			"HL-SteamPipe",
			"55 8B EC 83 EC 50 D9 05 ?? ?? ?? ?? D8 1D ?? ?? ?? ?? 56 57 33 FF C7 45",
			"HL-4554",
			"83 EC ?? D9 05 ?? ?? ?? ?? D8 1D ?? ?? ?? ?? 56 57 33 FF C7 44"
		);

		PATTERNS(R_Clear,
			"HL-SteamPipe",
			"8B 15 ?? ?? ?? ?? 33 C0 83 FA 01 0F 9F C0 50 E8 ?? ?? ?? ?? D9 05 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? 83 C4 04 DF E0",
			"HL-NGHL",
			"D9 05 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? DF E0 F6 C4 ?? ?? ?? D9 05 ?? ?? ?? ?? D8 1D"
		);

		PATTERNS(Mod_LeafPVS,
			"HL-SteamPipe",
			"55 8B EC 8B 55 0C 8B 45 08 8B 8A 8C 00 00 00 3B C1 75 07 B8 ?? ?? ?? ?? 5D C3 56 8B 35 ?? ?? ?? ?? 85 F6 5E",
			"HL-4554",
			"8B 54 24 08 8B 44 24 04 8B 8A 8C 00 00 00 3B C1 75 06 B8 ?? ?? ?? ?? C3 56 8B 35 ?? ?? ?? ?? 85 F6"
		);

		PATTERNS(SV_AddLinksToPM_,
			"HL-SteamPipe",
			"55 8B EC 83 EC 14 8B 4D 08 53 8B 5D 10 56 57 8B 79 1C 8D 41 18 3B F8 0F 84 ?? ?? ?? ?? 8B 47 04 83 C7 F8 89 45 EC 8B 87 ?? ?? ?? ?? 85 C0 74 35 8B 0D ?? ?? ?? ?? 85 C9 75 14 8B 0D",
			"HL-4554",
			"83 EC 10 53 55 56 57 8B 5C 24",
			"HL-NGHL",
			"83 EC 10 53 8B 5C 24 ?? 55 56 57"
		);

		PATTERNS(SV_WriteEntitiesToClient,
			"HL-SteamPipe",
			"55 8B EC B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 53 8B 5D 08 56 8B 35 ?? ?? ?? ?? 8B 43 68 8B 93",
			"HL-4554",
			"B8 24 54 01 00 E8 ?? ?? ?? ?? 53 55 56 8B 35 ?? ?? ?? ?? 57 8B BC 24 38 54 01 00",
			"HL-NGHL",
			"B8 28 54 01 00 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 53 8B 9C 24 30 54 01 00"
		);

		PATTERNS(R_DrawSkyBox,
			"HL-SteamPipe",
			"55 8B EC 83 EC 1C A1 ?? ?? ?? ?? 53 56",
			"HL-4554",
			"83 EC 1C A1 ?? ?? ?? ?? 53 55",
			"HL-WON-1712",
			"83 EC 0C 53 55 56 57 E8 ?? ?? ?? ?? 33 FF"
		);

		PATTERNS(DispatchDirectUserMsg,
			"HL-SteamPipe",
			"55 8B EC 53 56 8B 35 ?? ?? ?? ?? 33 DB"
		);

		PATTERNS(SV_SetMoveVars,
			"HL-SteamPipe",
			"A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? A3 ?? ?? ?? ?? A1 ?? ?? ?? ?? 89 0D ?? ?? ?? ?? 8B 0D"
		);

		PATTERNS(studioapi_GetCurrentEntity,
			"HL-SteamPipe",
			"A1 ?? ?? ?? ?? C3 90 90 90 90 90 90 90 90 90 90 55 8B EC 8B 45 08 85 C0 7C 18",
			"HL-4554",
			"A1 ?? ?? ?? ?? C3 90 90 90 90 90 90 90 90 90 90 8B 44 24 04 85 C0 7C 17",
			"HL-NGHL",
			"A1 ?? ?? ?? ?? C3 90 90 90 90 90 90 90 90 90 90 8B 4C 24 04 85 C9 7C 1B",
			"HL-WON-1712",
			"A1 ?? ?? ?? ?? C3 90 90 90 90 90 90 90 90 90 90 8B 44 24 04 85 C0 7C 18"
		);

		PATTERNS(R_StudioCalcAttachments,
			"HL-SteamPipe",
			"8B 0D ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 04"
		);

		PATTERNS(EmitWaterPolys,
			"HL-SteamPipe",
			"55 8B EC 83 EC 20 56 8B 75 ?? 33 D2",
			"HL-4554",
			"83 EC 1C 33 D2 55 56 8B 74 24 28 57 8B 46 2C 8B 48 24 8B 41 44 33 C9 8A 50 0C 8A 48 0B 52 51 33 D2"
		);

		PATTERNS(S_StartDynamicSound,
			"HL-SteamPipe",
			"55 8B EC 83 EC 48 A1 ?? ?? ?? ?? 53",
			"HL-4554",
			"83 EC 48 A1 ?? ?? ?? ?? 53 55 56 85 C0 57"
		);

		PATTERNS(VGuiWrap2_NotifyOfServerConnect,
			"HL-SteamPipe",
			"55 8B EC 8B 0D ?? ?? ?? ?? 33 C0",
			"HL-NGHL",
			"8B 0D ?? ?? ?? ?? 33 C0 3B C8 74 2E A3 ?? ?? ?? ?? A2"
		);

		PATTERNS(R_StudioSetupBones,
			"HL-SteamPipe",
			"55 8B EC 83 EC 48 A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 53 56 8B 90",
			"HL-4554",
			"A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 83 EC 48 8B 90"
		);

		PATTERNS(CBaseUI__HideGameUI,
			"HL-SteamPipe",
			"56 8B F1 8B 0D ?? ?? ?? ?? 8B 01 FF 50 ?? 8B 0D ?? ?? ?? ?? 8B 11 FF 52 ?? FF 15"
		);
	}

	namespace server
	{
		PATTERNS(CTriggerVolume__Spawn,
			"HL-SteamPipe",
			"56 8B F1 57 33 FF 8B 46 04 89 B8 0C 01 00 00 8B 4E 04 89 B9 08 01 00 00 8B 46 04",
			"Echoes",
			"55 8B EC 51 89 4D ?? 8B 45 ?? 8B 48 ?? C7 81 ?? ?? ?? ?? 00 00 00 00 8B 55 ?? 8B 42 ?? C7 80 ?? ?? ?? ?? 00 00 00 00 8B 4D ?? 8B 51 ?? A1 ?? ?? ?? ?? 8B 88 ?? ?? ?? ?? 03 8A ?? ?? ?? ?? 51 8B 55 ?? 8B 42 ?? 50 E8 ?? ?? ?? ?? 83 C4 04 50 FF 15 ?? ?? ?? ?? 83 C4 08 8B 4D ?? 8B 51 ?? C7 82 ?? ?? ?? ?? 00 00 00 00",
			"PARANOIA",
			"55 8B EC 83 EC 08 89 4D ?? 8B 45 ?? 8B 48 ?? C7 81 ?? ?? ?? ?? 00 00 00 00 8B 55 ?? 8B 42 ?? C7 80 ?? ?? ?? ?? 00 00 00 00 8B 4D ?? 8B 51 ?? 8B 82 ?? ?? ?? ?? 89 45 ?? 8B 4D ?? 8B 51 ?? A1 ?? ?? ?? ?? 8B 88 ?? ?? ?? ?? 03 8A ?? ?? ?? ?? 51 8B 55 ?? 52 FF 15 ?? ?? ?? ?? 83 C4 08 8B 45 ?? 8B 48 ?? C7 81 ?? ?? ?? ?? 00 00 00 00",
			"Halfquake-Trilogy",
			"53 56 57 8B F9 33 DB 8B 47 04 89 98 0C 01 00 00 8B 47 04 89 98 08 01 00 00 8B 77 04",
			"Half-Payne",
			"56 57 8B F9 8B 47 ?? C7 80 ?? ?? ?? ?? 00 00 00 00 8B 47 ?? C7 80 ?? ?? ?? ?? 00 00 00 00 8B 77 ?? A1 ?? ?? ?? ?? 8B 96 ?? ?? ?? ?? 03 90 ?? ?? ?? ?? 52 FF B6 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 8B 47"
		);

		PATTERNS(CBasePlayer__ForceClientDllUpdate,
			"HL-SteamPipe",
			"83 C8 FF C7 81 6C 04 00 00 01 00 00 00 89 81 98 04 00 00 89 81 9C 04 00 00 8B 81 74 04 00 00 0C C0 89 81 74 04 00 00 33 C0 89 81 78 04 00 00",
			"OpposingForce",
			"83 C8 FF 89 81 CC 04 00 00 89 81 D0 04 00 00 8B 81 A8 04 00 00 0C C0 89 81 A8 04 00 00 33 C0 89 81 AC 04 00 00 89 81 30 03 00 00 8B 01 FF A0 0C 02 00 00",
			"HazardousCourse2",
			"83 C8 FF C7 81 58 04 00 00 01 00 00 00 89 81 84 04 00 00 89 81 88 04 00 00 8B 81 60 04 00 00 0C C0 89 81 60 04 00 00 33 C0 89 81 64 04 00 00",
			"Gunman",
			"8B 91 48 04 00 00 83 C8 FF 89 81 68 04 00 00 89 81 6C 04 00 00 33 C0 80 CA C0 88 81 79 02 00 00 89 81 4C 04 00 00 89 81 D0 02 00 00 8B 01",
			"HL-SteamPipe-8308",
			"83 C8 FF C7 81 70 04 00 00 01 00 00 00 89 81 9C 04 00 00 89 81 A0 04 00 00 8B 81 78 04 00 00 0C C0 89 81 78 04 00 00 33 C0 89 81 7C 04 00 00",
			"TWHL-Tower-2",
			"83 89 34 05 00 00 FF 83 89 38 05 00 00 FF 80 89 10 05 00 00 C0 83 A1 14 05 00 00 00 83 A1 98 03 00 00 00",
			"Halfquake-Trilogy",
			"83 89 34 05 00 00 FF 83 89 38 05 00 00 FF 81 89 10 05 00 00 C0 00 00 00 83 A1 14 05 00 00 00 83 A1 98 03 00 00 00",
			"Echoes",
			"55 8B EC 51 89 4D FC 8B 45 FC C7 80 34 05 00 00 FF FF FF FF 8B 4D FC C7 81 38 05 00 00 FF FF FF FF 8B 55 FC 8B 82 10 05 00 00 0C C0",
			"Decay",
			"55 8B EC 83 EC 44 53 56 57 89 4D FC 8B 45 FC C7 80 88 04 00 00 FF FF FF FF 8B 45 FC C7 80 8C 04 00 00 FF FF FF FF 8B 45 FC 8B 88 64 04 00 00",
			"AoMDC",
			"55 8B EC 51 89 4D FC 8B 45 FC C7 80 BC 04 00 00 FF FF FF FF 8B 4D FC C7 81 C0 04 00 00 FF FF FF FF 8B 55 FC 8B 82 98 04 00 00 0C C0",
			"PARANOIA",
			"55 8B EC 51 89 4D FC 8B 45 FC C7 80 70 05 00 00 FF FF FF FF 8B 4D FC C7 81 74 05 00 00 FF FF FF FF 8B 55 FC 8B 82 4C 05 00 00 0C C0",
			"OpposingForce-8684",
			"83 C8 FF 89 81 D0 04 00 00 89 81 D4 04 00 00 8B 81 AC 04 00 00 0C C0 89 81 AC 04 00 00 33 C0 89 81 B0 04 00 00 89 81 34 03 00 00 8B 01 FF A0 0C 02 00 00",
			"HL-WON",
			"83 C8 FF 89 81 50 04 00 00 89 81 54 04 00 00 8B 81 2C 04 00 00 0C C0 89 81 2C 04 00 00 33 C0 89 81 30 04 00 00 89 81 B4 02 00 00 8B 01 FF A0 04 02 00 00",
			"OpposingForce-WON",
			"83 C8 FF 89 81 90 04 00 00 89 81 94 04 00 00 8B 81 6C 04 00 00 0C C0 89 81 6C 04 00 00 33 C0 89 81 70 04 00 00 89 81 F4 02 00 00 8B 01 FF A0 08 02 00 00",
			"CSCZDS",
			"8B D1 83 C9 FF 57 8B 82 CC 05 00 00 89 8A 64 06 00 00 0C C0 89 8A 68 06 00 00 89 82 CC 05 00 00 33 C0 89 8A 6C 06 00 00",
			"Black-Ops",
			"83 C8 FF 89 81 98 04 00 00 89 81 9C 04 00 00 8B 81 74 04 00 00 0C C0 89 81 74 04 00 00 33 C0 89 81 78 04 00 00 89 81 FC 02 00 00 8B 01 FF A0 0C 02 00 00",
			"Invasion",
			"8B 91 34 04 00 00 83 C8 FF 80 CA C0 89 81 58 04 00 00 89 91 34 04 00 00 33 D2 89 81 5C 04 00 00 89 81 1C 06 00 00 89 81 20 06 00 00 8B 01 89 91 38 04 00 00 89 91 BC 02 00 00 FF A0 08 02 00 00",
			"Arctic-Incident",
			"55 8B EC 51 89 4D FC 8B 45 FC C7 80 FC 04 00 00 FF FF FF FF 8B 4D FC C7 81 00 05 00 00 FF FF FF FF 8B 55 FC 8B 82 D8 04 00 00 0C C0"
		);

		PATTERNS(CBasePlayer__TakeDamage,
			"HL-SteamPipe",
			"83 EC ?? 53 8B 5C 24 ?? 55 56 8B F1 57 F6 C3 40 8B 46 04 BF 01 00 00 00 C7 44 24 14 00 00 00 3F 8B 88 60 01 00 00",
			"AoMDC",
			"55 8B EC 83 EC 3C 89 4D D0 8B 45 14 89 45 F4 C7 45 F8 01 00 00 00 8B 4D D0 8B 51 04 8B 82 60 01 00 00",
			"TWHL-Tower-2",
			"55 8B EC 83 EC 14 8B 45 14 56 8B F1 8B 4E 04 8B 89 60 01 00 00 57 33 FF 47 A8 40",
			"Halfquake-Trilogy",
			"55 8B EC 83 EC 10 53 33 C0 56 8B F1 8B 4D 14 89 4D FC 8B D9 57 8D 78 01 8B 46 04 F3 0F 10 80 60 01 00 00",
			"Echoes",
			"55 8B EC 83 EC 44 89 4D C8 8B 45 14 89 45 F4 C7 45 F8 01 00 00 00 8B 4D C8 8B 51 04 8B 82 60 01 00 00",
			"PARANOIA",
			"55 8B EC 81 EC C4 00 00 00 89 8D 4C FF FF FF 8B 45 14 89 45 F4 C7 45 F8 01 00 00 00 8B 8D 4C FF FF FF 8B 51 04 8B 82 60 01 00 00",
			"CSCZDS",
			"83 EC 4C 53 8B 5C 24 60 55 8B E9 8B D3 56 8B 45 04 81 E2 00 00 00 80 83 FA 01 57 8B 88 60 01 00 00"
		);

		PATTERNS(CPushable__Move,
			"HL-SteamPipe",
			"53 56 8B F1 8B 4C 24 0C 57 33 DB 8B 79 04 8B 87 A4 01 00 00 F6 C4 02 74 3A 8B 87 9C 01 00 00 85 C0 74 30 8D 90 80 00 00 00 8B 46 04",
			"HL-WON",
			"53 56 8B F1 57 8B 4C 24 10 33 DB 8B 79 04 8B 87 A4 01 00 00 F6 C4 02 74 40 8B 87 9C 01 00 00 85 C0 74 36 75 04 33 D2 EB 06 8D 90 80 00 00 00 8B 46 04",
			"TWHL-Tower-2",
			"55 8B EC 83 EC 10 83 65 FC 00 53 56 8B F1 8B 4D 08 57 8B 79 04 BB 00 02 00 00 85 9F A4 01 00 00 74 45",
			"Halfquake-Trilogy",
			"55 8B EC 8B 55 08 83 65 08 00 56 57 8B 7A 04 8B F1 F7 87 A4 01 00 00 00 02 00 00 74 58",
			"Echoes",
			"55 8B EC 83 EC 14 56 89 4D EC 8B 45 08 8B 48 04 89 4D F8 C7 45 F4 00 00 00 00 8B 55 F8 8B 82 A4 01 00 00 25 00 02 00 00",
			"PARANOIA",
			"55 8B EC 83 EC 38 89 4D C8 8B 45 08 8B 48 04 89 4D F8 C7 45 F4 00 00 00 00 8B 55 F8 8B 82 A4 01 00 00 25 00 02 00 00",
			"CSCZDS",
			"83 EC 1C 53 56 8B F1 8B 4C 24 28 57 33 DB 8B 79 04 8B 87 A4 01 00 00 F6 C4 02 74 3D 8B 87 9C 01 00 00 85 C0 74 33 8D 90 80 00 00 00 8B 46 04"
		);

		PATTERNS(DispatchRestore,
			"HL-SteamPipe",
			"81 EC E0 02 00 00 53 55 8B AC 24 EC 02 00 00 33 DB 56 3B EB 57 0F 84 EE 02 00 00 8B 75 7C 3B F3"
		);

		PATTERNS(CGraph__InitGraph,
			"HL-SteamPipe",
			"56 8B F1 57 33 FF 8B 46 10 89 3E 3B C7 89 7E 04 89 7E 08 74 0C 50 E8 ?? ?? ?? ?? 83 C4 04 89 7E 10 8B 46 0C",
			"TWHL-Tower-2",
			"53 56 8B F1 8B 46 10 33 DB 3B C3 57 89 1E 89 5E 04 89 5E 08 74 0A 50 E8 ?? ?? ?? ?? 59",
			"Echoes",
			"55 8B EC 51 89 4D ?? 8B 45 ?? C7 00 00 00 00 00 8B 4D ?? C7 41 ?? 00 00 00 00 8B 55 ?? C7 42 ?? 00 00 00 00 8B 45 ?? 83 78 ?? 00",
			"Decay",
			"55 8B EC 83 EC 44 53 56 57 89 4D ?? 8B 45 ?? C7 00 00 00 00 00 8B 45 ?? C7 40 ?? 00 00 00 00 8B 45",
			"Halfquake-Trilogy",
			"56 8B F1 57 33 FF 89 3E 89 7E 04 89 7E 08 39 7E 10 74 0C FF 76 10 E8 ?? ?? ?? ?? 59"
		);

		PATTERNS(MiddleOfCNihilanth__NextActivity,
			"HL-SteamPipe-Linux",
			"55 57 56 53 81 EC ?? ?? ?? ?? 8B 9C 24 ?? ?? ?? ?? 8B 43 ?? 83 C0 ?? 89 04 24 E8 ?? ?? ?? ?? 83 BB",
			"HL-SteamPipe",
			"D9 05 ?? ?? ?? ?? D8 0D ?? ?? ?? ?? 8B 56 04 D8 9A ?? ?? ?? ?? DF E0 25 ?? ?? ?? ?? 74 0D 83 BE",
			"HL-WON",
			"D9 05 ?? ?? ?? ?? D8 0D ?? ?? ?? ?? 8B 56 04 D8 9A ?? ?? ?? ?? DF E0 F6 C4 41 74 0D 83 BE",
			"Half-Payne",
			"F3 0F 10 05 ?? ?? ?? ?? 8B 47 04 F3 0F 59 05 ?? ?? ?? ?? 0F 2F 80 60 01 00 00 77 0D 83 BF"
		);

		PATTERNS(CNihilanth__EmitSphere,
			"HL-SteamPipe-Linux",
			"57 31 C9 56 31 FF 53 31 DB 83 EC ?? 8B 74 24 ?? 89 8E ?? ?? ?? ?? EB ?? 90",
			"HL-SteamPipe",
			"83 EC 18 53 55 56 8B F1 33 ED 57 89 AE ?? ?? ?? ?? 33 FF 8D 9E ?? ?? ?? ?? 8B CB E8",
			"Half-Payne",
			"55 8B EC 83 EC 28 56 8B F1 57 33 FF 33 C9 C7 86 DC 03 00 00 00 00 00 00 8D 96 3C 03 00 00 66 90"
		);

		PATTERNS(CBasePlayer__CheatImpulseCommands,
			"HL-SteamPipe",
			"81 EC ?? ?? ?? ?? 56 57 68 ?? ?? ?? ?? 8B F1 FF 15 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? 83 C4 04 DF E0 F6 C4 44 0F 8B ?? ?? ?? ?? 8B 84 24",
			"TWHL-Tower-2",
			"55 8D 6C 24 8C 81 EC C4 00 00 00 D9 05 ?? ?? ?? ?? 56 DD 05 ?? ?? ?? ?? 57 DA E9 8B F1 DF E0 F6 C4 44 0F 8B AD 07 00 00",
			"Echoes",
			"55 8B EC 81 EC ?? ?? 00 00 89 8D ?? ?? ?? ?? D9 05 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? DF E0 F6 C4 40 74 05",
			"Halfquake-Trilogy",
			"55 8B EC F3 0F 10 05 ?? ?? ?? ?? 81 EC 88 00 00 00 0F 2E 05 ?? ?? ?? ?? 53 57 8B F9 9F F6 C4 44 0F 8B 25 08 00 00",
			"Half-Payne",
			"53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B ?? 89 6C 24 ?? 8B EC A1",
			"HL-SteamPipe-6153",
			"D9 05 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? 81 EC ?? ?? ?? ?? 56 57 DF E0 8B F1 F6 C4 ?? 0F ?? ?? ?? ?? ?? 8B 84 24",
			"Gunman",
			"D9 05 ?? ?? ?? ?? D8 1D ?? ?? ?? ?? 81 EC E4 01 00 00",
			"Invasion",
			"D9 05 ?? ?? ?? ?? DC 1D ?? ?? ?? ?? 81 EC ?? ?? ?? ?? 56 57 DF E0 8B F1 F6 C4 ?? 0F ?? ?? ?? ?? ?? A1 ?? ?? ?? ?? 85 C0 0F ?? ?? ?? ?? ?? 8B 84 24"
		);

		PATTERNS(CZDS_Velocity_Byte,
			"HL-SteamPipe",
			"C7 05 ?? ?? ?? ?? 01 00 00 00 5E 5B 59"
		);

		PATTERNS(CBaseMonster__Killed,
			"Wanted!",
			"56 8B F1 8B 86 ?? ?? ?? ?? A8 80 74 1D 8B 44 24 0C 50 E8 ?? ?? ?? ?? 85 C0",
			"Crowbar of Time",
			"56 8B F1 8B 86 ?? ?? ?? ?? 84 C0 79 1B 8B 44 24 0C 50 E8 ?? ?? ?? ?? 85 C0",
			"OpposingForce",
			"56 8B F1 57 8B 86 ?? ?? ?? ?? 84 C0",
			"OpposingForce-WON",
			"56 8B F1 57 8B 86 ?? ?? ?? ?? A8 80",
			"TWHL-Tower-2",
			"56 8B F1 8B 86 ?? ?? ?? ?? 84 C0 79 1B FF 74 24 0C E8 ?? ?? ?? ?? 85 C0",
			"Invasion",
			"56 8B F1 8B 46 04 83 B8 ?? ?? ?? ?? ?? 75 0A C7 80 ?? ?? ?? ?? 00 00 00 00 8B 86 ?? ?? ?? ?? A8 80 74 1F 8B 44 24 0C 8B CE 50 E8 ?? ?? ?? ?? 85 C0"
		);

		PATTERNS(CChangeLevel__InTransitionVolume,
			"HL-SteamPipe",
			"53 55 56 8B 74 24 ?? 57 8B CE 8B 06 FF 50"
		);
	}

	namespace client
	{
		PATTERNS(StudioCalcAttachments,
			"HL-WON",
			"55 57 8B F9 8B 4F 44 8B 91 D4 00 00 00 83 FA 04 7E 1F 8B 47 30 8B 88 94 0B 00 00 51 68 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 83 C4 08 6A FF E8 ?? ?? ?? ?? 8B 81 D8 00 00 00 33 ED",
			"CSCZDS",
			"57 8B F9 8B 4F 44 8B 91 D4 00 00 00 83 FA 04 7E 1F 8B 47 30 8B 88 94 0B 00 00 51 68 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 83 C4 08 6A FF E8 ?? ?? ?? ?? 8B 81 D8 00 00 00 55",
			"Echoes",
			"55 8B EC 83 EC 0C 89 4D ?? 8B 45 ?? 8B 48 ?? 83 B9 ?? ?? ?? ?? 04",
			"AoMDC",
			"55 8B EC 83 EC 4C 53 56 57 89 4D ?? 8B 45 ?? 8B 48 ?? 83 B9 ?? ?? ?? ?? 04",
			"TWHL-Tower-2",
			"56 8B F1 8B 46 44 8B 90 D4 00 00 00 83 FA 04 7E 1D 8B 46 30 FF B0 94 0B 00 00 68 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 59 59 6A FF E8 ?? ?? ?? ?? 8B 4E 44 8B 81 D8 00 00 00 53"
		);

		PATTERNS(EV_GetDefaultShellInfo,
			"HL-WON",
			"83 EC 10 53 55 56 57 8B 7C 24",
			"Echoes",
			"55 8B EC 83 EC 1C 56 8D 4D ?? E8 ?? ?? ?? ?? 8B 45",
			"AoMDC",
			"55 8B EC 83 EC 5C 53 56 57 8D 4D ?? E8 ?? ?? ?? ?? 8B 45",
			"TWHL-Tower-2",
			"55 8B EC 83 EC 20 56 D9 EE"
		);

		PATTERNS(CStudioModelRenderer__StudioSetupBones,
			"HL-SteamPipe",
			"83 EC 48 53 8B D9 55 56 8B 43 30 8B 4B 44",
			"HL-WON",
			"83 EC 48 53 55 56 8B F1 57 8B 46 30 8B 4E 44",
			"CSCZDS",
			"83 EC 4C 53 55 8B E9 56 57 8B 45 44",
			"HL-Restored",
			"55 8B EC 83 E4 F8 83 EC 54 53",
			"Echoes",
			"55 8B EC 81 EC 90 00 00 00 89 4D",
			"They Hunger Trilogy",
			"55 8B EC 81 EC 94 00 00 00 89 4D",
			"PARANOIA",
			"55 8B EC 81 EC A4 00 00 00 89 4D",
			"AoMDC",
			"55 8B EC 81 EC D0 00 00 00 53 56 57 89 4D",
			"TWHL-Tower-2",
			"55 8B EC 83 EC 48 53 56 8B F1 8B 46 30 8B 4E 44",
			"Invasion",
			"83 EC 44 53 55 56 8B F1 57 8B 46 30 8B 4E 44"
		);
	}

	namespace shared
	{
		PATTERNS(PM_Jump,
			"HL-SteamPipe",
			"51 A1 ?? ?? ?? ?? 53 56 8B 88 D0 00 00 00 85 C9 74 13 8B 88 C8 00 00 00 5E 83 C9 02 5B 89 88 C8 00 00 00 59 C3 8D 88 F0 F3 04 00 68",
			"gunman",
			"51 A1 ?? ?? ?? ?? 53 8B 88 D0 00 00 00 85 C9 74 12 8B 88 C8 00 00 00 5B 83 C9 02 89 88 C8 00 00 00 59 C3 8D 88 F0 F3 04 00 68",
			"AG-Server",
			"51 8B 0D ?? ?? ?? ?? 53 8B 81 D0 00 00 00 85 C0 74 11 8B 81 C8 00 00 00 5B 0C 02 89 81 C8 00 00 00 59 C3 D9 81 CC 00 00 00 D8 1D",
			"AG-Client",
			"51 8B 0D ?? ?? ?? ?? 8B 81 D0 00 00 00 85 C0 74 09 83 89 C8 00 00 00 02 59 C3 D9 05 ?? ?? ?? ?? D9 81 CC 00 00 00 DA E9",
			"BigLolly",
			"55 8B EC 83 EC ?? C7 45 F4 00 00 00 00 C7 45 FC 00 00 00 00 A1 ?? ?? ?? ?? 83 B8 D0 00 00 00 00 74 1F 8B 0D ?? ?? ?? ?? 8B 91 C8 00 00 00 83 CA 02 A1",
			"TWHL-Tower-2",
			"55 8B EC 51 A1 ?? ?? ?? ?? 57 33 FF 39 B8 D0 00 00 00 74 0C 83 88 C8 00 00 00 02 E9 FC 02 00 00 56 8D 88 F0 F3 04 00 68",
			"Decay",
			"55 8B EC 83 EC 50 53 56 57 C7 45 ?? 00 00 00 00 C7 45 ?? 00 00 00 00 A1 ?? ?? ?? ?? 83 B8 D0 00 00 00 00 74 1F",
			"Halfquake-Trilogy",
			"55 8B EC 51 8B 0D ?? ?? ?? ?? 56 33 F6 39 B1 D0 00 00 00 74 0C 83 89 C8 00 00 00 02 E9 44 03 00 00 53 8D 81 F0 F3 04 00 68",
			"Half-Payne",
			"55 8B EC 83 E4 F8 8B 0D ?? ?? ?? ?? 56 57 F7 81 B8 00 00 00 00 00 08 00 0F 85 52 03 00 00 83 3D ?? ?? ?? ?? 00 0F 85",
			"DSM-Demo-1",
			"55 8B EC 83 E4 F8 8B 0D ?? ?? ?? ?? 83 EC 08 83 B9 D0 00 00 00 00 53 56 74 ?? 83 89 C8 00 00 00 02 5E 5B 8B E5 5D C3 8D 81 F0 F3 04 00 68"
		);

		PATTERNS(PM_PreventMegaBunnyJumping,
			"HL-SteamPipe",
			"51 8B 0D ?? ?? ?? ?? D9 81 ?? ?? ?? ?? D8 0D ?? ?? ?? ?? D9 54 24 00 D8 1D ?? ?? ?? ?? DF E0 F6 C4 41 ?? 3B 83 C1",
			"Echoes",
			"55 8B EC 83 EC 0C A1 ?? ?? ?? ?? D9 05 ?? ?? ?? ?? D8 88 F4 01 00 00 D9 55 FC D8 1D ?? ?? ?? ?? DF E0 F6 C4 41",
			"Decay",
			"55 8B EC 83 EC 4C 53 56 57 A1 ?? ?? ?? ?? D9 05 ?? ?? ?? ?? D8 88 F4 01 00 00 D9 5D F4 D9 45 F4 D8 1D ?? ?? ?? ?? DF E0 F6 C4 41",
			"TWHL-Tower-2",
			"55 8B EC 51 8B 0D ?? ?? ?? ?? D9 81 F4 01 00 00 D8 0D ?? ?? ?? ?? D9 55 FC D8 1D ?? ?? ?? ?? DF E0 F6 C4 41",
			"Halfquake-Trilogy",
			"55 8B EC 51 51 A1 ?? ?? ?? ?? 0F 57 C0 F3 0F 10 88 ?? ?? ?? ?? F3 0F 59 0D ?? ?? ?? ?? 0F 2F C1 F3 0F 11 4D F8 73 44 83 C0"
		);

		PATTERNS(Bhopcap,
			"HL-SteamPipe",
			"D9 05 ?? ?? ?? ?? BA FF FF FF FF D8 89 ?? ?? ?? ?? D9 C9 89 91 ?? ?? ?? ?? DF E9 0F 82"
		);

		PATTERNS(Bhopcap_Windows,
			"DSM-Demo-1",
			"F3 0F 10 86 ?? ?? ?? ?? F3 0F 59 05 ?? ?? ?? ?? C7 86 ?? ?? ?? ?? FF FF FF FF 0F 2F D8 F3 0F 11 44 24 08 0F 83 ?? ?? ?? ?? F3 0F"
		);

		PATTERNS(PM_PlayerMove,
			"HL-SteamPipe",
			"A1 ?? ?? ?? ?? 8B 4C 24 04 55 57 33 FF 89 48 04 E8 ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? 33 C9 89 BA 8C 54 04 00 A1 ?? ?? ?? ?? 8A 88 5A 54 04 00 89",
			"BigLolly",
			"55 8B EC 83 EC 0C C7 45 FC 00 00 00 00 A1 ?? ?? ?? ?? 8B 4D 08 89 48 04 E8 ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? C7 82 8C 54 04 00 00 00 00 00 A1",
			"TWHL-Tower-2",
			"55 8B EC 51 A1 ?? ?? ?? ?? 8B 4D 08 53 56 57 33 FF 89 7D FC 89 48 04 E8 D8 FC FF FF A1 ?? ?? ?? ?? 89 B8 8C 54 04 00 A1 ?? ?? ?? ?? 0F B6 88 5A 54 04 00",
			"Halfquake-Trilogy",
			"55 8B EC 8B 0D ?? ?? ?? ?? 8B 45 08 56 57 33 F6 89 41 04 8B FE E8 ?? ?? ?? ?? A1 ?? ?? ?? ?? 89 B0 8C 54 04 00 8B 0D ?? ?? ?? ?? 0F B6 81 5A 54 04 00",
			"CSCZDS",
			"A1 ?? ?? ?? ?? 8B 4C 24 04 55 56 57 33 ED 33 ?? 89 48 04 E8 ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? 33 C9 89 AA 8C 54 04 00 A1 ?? ?? ?? ?? 8A 88 5A 54 04 00 89"
		);

		PATTERNS(PM_ClipVelocity,
			"HL-SteamPipe",
			"8B 4C 24 08 53 33 DB 56 D9 41 08 D8 15 ?? ?? ?? ?? 57 DF E0 25 00 41 00 00 75 05 BB 01 00 00 00 D8 1D ?? ?? ?? ?? DF E0 F6 C4 44 7A 03 83 CB 02",
			"HL-WON",
			"8B 4C 24 08 53 33 DB 56 D9 41 08 D8 15 ?? ?? ?? ?? 57 DF E0 F6 C4 41 75 05 BB 01 00 00 00 D8 1D ?? ?? ?? ?? DF E0 F6 C4 40 74 03 83 CB 02 8B 44",
			"BigLolly",
			"55 8B EC 83 EC 14 8B 45 0C 8B 48 08 89 4D F8 C7 45 F0 00 00 00 00 D9 45 F8 D8 1D ?? ?? ?? ?? DF E0 F6 C4 41 75 09 8B 55 F0 83 CA 01 89 55 F0 D9 45 F8 D8 1D",
			"TWHL-Tower-2",
			"55 8B EC 8B 4D 0C D9 41 08 53 D8 15 ?? ?? ?? ?? 56 33 DB 57 DF E0 F6 C4 41 75 01 43 D9 05 ?? ?? ?? ?? D9 C1 DA E9",
			"Halfquake-Trilogy",
			"55 8B EC 53 56 8B 75 0C 33 C0 40 33 C9 0F 57 C0 57 F3 0F 10 4E 08 0F 2F C8 0F 47 C8 0F 2E C8 9F F6 C4 44 7A 03 83 C9 02"
		);

		PATTERNS(PM_WaterMove,
			"HL-SteamPipe",
			"8B 15 ?? ?? ?? ?? 81 EC CC 00 00 00 33 C0 8D 4A 20 D9 41 F4 D8 8A 68 54 04 00 D9 82 6C 54 04 00 D8 09 40 83 C1 04",
			"HL-WON",
			"8B 0D ?? ?? ?? ?? 81 EC 88 00 00 00 8D 54 24 08 56 8D 41 20 BE 03 00 00 00 D9 40 F4 D8 89 68 54 04 00 D9 00 D8 89",
			"BigLolly",
			"55 8B EC 81 EC DC 00 00 00 56 57 C7 45 88 00 00 00 00 EB 09 8B 45 88 83 C0 01 89 45 88 83 7D 88 03 7D 3B 8B 4D 88 8B 15",
			"TWHL-Tower-2",
			"55 8D 6C 24 88 81 EC CC 00 00 00 8B 0D ?? ?? ?? ?? 33 D2 8D 41 20 D9 40 F4 42 D8 89 68 54 04 00 D9 81 6C 54 04 00",
			"Halfquake-Trilogy",
			"55 8B EC 81 EC D0 00 00 00 A1 ?? ?? ?? ?? 33 C5 89 45 ?? 8B 0D ?? ?? ?? ?? 33 C0 8D 51 20 F3 0F 10 4A F4 F3 0F 10 02 83 C2 04 F3 0F 59 89 68 54 04 00 F3 0F 59 81 6C 54 04 00"
		);

		PATTERNS(PM_Ladder,
			"HL-SteamPipe",
			"8B 0D ?? ?? ?? ?? 83 EC 0C 8B 81 50 0F 02 00 53 55 33 DB 56 57 85 C0 0F 8E 94 00 00 00 33 FF 8B 84 0F 84 0F 02 00",
			"TWHL-Tower-2",
			"55 8B EC 83 EC 10 A1 ?? ?? ?? ?? 53 56 57 33 FF 39 B8 50 0F 02 00 89 7D FC 0F 8E 85 00 00 00",
			"Echoes",
			"55 8B EC 83 EC 1C C7 45 ?? 00 00 00 00 EB ?? 8B 45 F0 83 C0 01 89 45 F0 8B 0D ?? ?? ?? ?? 8B 55 F0 3B 91 50 0F 02 00",
			"Halfquake-Trilogy",
			"55 8B EC 83 EC 14 A1 ?? ?? ?? ?? 33 C5 89 45 FC A1 ?? ?? ?? ?? 53 56 33 F6 57 39 B0 50 0F 02 00 0F 8E 9E 00 00 00"
		);

		PATTERNS(PM_WalkMove,
			"HL-SteamPipe",
			"81 EC E4 00 00 00 A1 ?? ?? ?? ?? 53 55 33 ED 8B 88 68 54 04 00 8B 90 6C 54 04 00 89 68 1C A1 ?? ?? ?? ?? 89 4C 24 54 56 89 68 28 8B 0D ?? ?? ?? ?? 83 C1 14",
			"TWHL-Tower-2",
			"55 8B EC 83 EC 20 A1 ?? ?? ?? ?? D9 EE 8B 88 68 54 04 00 89 4D F8 8B 88 6C 54 04 00 D9 58 1C A1",
			"Echoes",
			"55 8B EC 81 EC 94 01 00 00 56 57 A1 ?? ?? ?? ?? 8B 88 68 54 04 00 89 4D 88 8B 15 ?? ?? ?? ?? 8B 82 6C 54 04 00",
			"Halfquake-Trilogy",
			"55 8B EC 81 EC E0 00 00 00 A1 ?? ?? ?? ?? 33 C5 89 45 FC A1 ?? ?? ?? ?? F3 0F 10 80 68 54 04 00 F3 0F 11 45 D4 F3 0F 10 80 6C 54 04 00 83 60 1C 00"
		);

		PATTERNS(PM_FlyMove,
			"HL-SteamPipe",
			"81 EC 10 01 00 00 53 55 56 8B 35 ?? ?? ?? ?? 33 DB 57 8B 46 5C 8B 4E 60 8B 56 64 89 44 24 18 89 44 24 38 8B 46 10 89 5C 24 10 89 4C 24 1C 89 54 24 20",
			"HL-WON",
			"81 EC 10 01 00 00 8B 0D ?? ?? ?? ?? 53 55 33 DB 8B 41 5C 56 89 44 24 14 8B 51 60 89 54 24 18 8B",
			"TWHL-Tower-2",
			"55 8D 6C 24 88 81 EC 14 01 00 00 53 D9 EE 56 D9 5D 5C 8B 35 ?? ?? ?? ?? 8B 46 5C 89 45 60",
			"Echoes",
			"55 8B EC 81 EC 28 01 00 00 56 57 C7 85 ?? ?? ?? ?? 04 00 00 00 C7 85 ?? ?? ?? ?? 00 00 00 00 C7 45 ?? 00 00 00 00 A1",
			"Halfquake-Trilogy",
			"55 8B EC 81 EC 18 01 00 00 A1 ?? ?? ?? ?? 33 C5 89 45 FC 53 56 8B 35 ?? ?? ?? ?? 33 C0 57 0F 57 C9 89 45 88 33 DB F3 0F 11 4D 80 F3 0F 10 56 10 8D 4E 5C F3 0F 10 01 8D 56 60"
		);

		PATTERNS(PM_AddToTouched,
			"HL-SteamPipe",
			"56 8B 35 ?? ?? ?? ?? 33 C0 57 8B 8E 8C 54 04 00 85 C9 7E 16 8B 7C 24 3C 8D 96 C0 54 04 00 39 3A 74 08 40 83 C2 44",
			"TWHL-Tower-2",
			"55 8B EC A1 ?? ?? ?? ?? 8B 90 8C 54 04 00 56 33 F6 85 D2 57 7E 15 8D 88 C0 54 04 00",
			"Echoes",
			"55 8B EC 51 56 57 C7 45 ?? 00 00 00 00 EB 09 8B 45 FC 83 C0 01 89 45 FC 8B 0D ?? ?? ?? ?? 8B 55 FC 3B 91 8C 54 04 00",
			"Halfquake-Trilogy",
			"55 8B EC 83 EC 48 A1 ?? ?? ?? ?? 33 C5 89 45 FC 56 57 6A 11 59 8D 75 08 33 C0 8D 7D B8 F3 A5 8B 0D ?? ?? ?? ?? 8B 91 8C 54 04 00 85 D2 7E 15 8B 7D E8 8D B1 C0 54 04 00"
		);
	}
}
