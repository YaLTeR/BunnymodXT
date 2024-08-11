#pragma once

enum class MessageType : unsigned char {
	TIME = 0x00,
	CLIP = 0x01,
	WATER = 0x02,
	FRAMETIME_REMAINDER = 0x03,
	EVENT = 0x04
};

enum class EventType : unsigned char {
	GAMEEND = 0x00,
	MAPCHANGE = 0x01,
	TIMER_RESET = 0x02,
	TIMER_START = 0x03,
	BS_ALEAPOFFAITH = 0x04
};

#define MQ_NAME "BunnymodXT-TASView"
#define BUNNYSPLIT_PIPE_NAME "BunnymodXT-BunnySplit"

// - Game constants
// Constants that are taken from the game code and you don't want to add for them the corresponding header, then leave them here.
// If possible, add a link to their source from the original repositories.
// Or if those constants got from reverse-engineering, then do not forget to tell about that in the comments.

#define MAXPRINTMSG 4096 // Con_Printf is limited to 4096 characters.

constexpr float HL1_SLOWDOWN_PLAYER_BY_USE = 0.3f; // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/pm_shared/pm_shared.c#L3028

#define COF_TRIGGER_CAMERA_FLAGS_UNSKIPPABLE 1024 // https://github.com/LogicAndTrick/sledge-formats/blob/9d11ebb3ce6d6d5e80864cf96b235c491a106958/Sledge.Formats.GameData.Tests/Resources/fgd/goldsource/cry-of-fear.fgd#L3720

constexpr unsigned int off_pev = 4; // CBaseEntity, pev

// - Custom constants
// Constants whose values you set depending on your code and preferences.

constexpr int kRenderFxTrigger = 241; // DO NOT CHANGE THIS VALUE OR YOU WILL BROKE COMPATIBILITY WITH READING SHOW TRIGGERS DATA IN OLD DEMOS! @ Smiley

#define BIG_MAP_SIZE 32768 // +-BIG_MAP_SIZE so 64k x 64k map should have value of 32k
#define BIG_MAP_MAX_VELOCITY 8192 // +-8192

// - BXT Flags
// Used to track individual enabled BXT features from demo.

#define	BXT_FLAGS_BIG_MAP (1<<0)

// - Other constants
constexpr steamid_t STEAMID64_CONST = 76561197960265728; // 0x110000100000000

// - Custom macros
#define GET_PEV(thisptr) *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + off_pev);

// - Custom enums

/*
	BXT_ENUM_GAMEDIR_STARTS_WITH, BXT_ENUM_GAMEDIR_MATCH (_Smiley):

	There's really no point in being strict in the ordering of the elements here, so I'll just rank the mods from most to least significant based on my own feelings. Yeah...
	I also tend to name these IDs after their mods and not their directories, so don't let STARTS_WITH_ or MATCH_ mislead you.
	And I don't want to use bitwise operations to check the same value for STARTS_WITH_ and MATCH_ flags, because it will really be more confusing for the average user who wants to add autostop and overall it is a more limited solution.

	So the format for filling out looks like this:
	*_ShortModName, // Mod directory (Full name of the modification)
*/

typedef enum
{
	GAMEDIR_STARTS_WITH_VALVE = 0, // valve (Half-Life)
	GAMEDIR_STARTS_WITH_OPFOR, // gearbox (Half-Life: Opposing Force)
	GAMEDIR_STARTS_WITH_CSCZDS, // czeror (Counter-Strike: Condition Zero Deleted Scenes)
	GAMEDIR_STARTS_WITH_GUNMAN, // rewolf (Gunman Chronicles)
	GAMEDIR_STARTS_WITH_HUNGER, // Hunger (They Hunger)
	GAMEDIR_STARTS_WITH_WANTED, // wantedsp (Half-Life: Wanted!)
	GAMEDIR_STARTS_WITH_AOMDC, // AoMDC (Afraid of Monsters: Director's Cut)
	GAMEDIR_STARTS_WITH_ECHOES, // echoes (Half-Life: Echoes)
	GAMEDIR_STARTS_WITH_POKE646, // poke646 (Poke646)
	GAMEDIR_STARTS_WITH_COF, // CryOfFear (Cry of Fear)
	GAMEDIR_STARTS_WITH_PARANOIA, // paranoia (PARANOIA)
	GAMEDIR_STARTS_WITH_BSHIFT // bshift (Half-Life: Blue Shift)
} BXT_ENUM_GAMEDIR_STARTS_WITH;

typedef enum
{
	GAMEDIR_MATCH_VALVE = 0, // valve (Half-Life)
	GAMEDIR_MATCH_OPFOR, // gearbox (Half-Life: Opposing Force)
	GAMEDIR_MATCH_CSCZDS, // czeror (Counter-Strike: Condition Zero Deleted Scenes)
	GAMEDIR_MATCH_GUNMAN, // rewolf (Gunman Chronicles)
	GAMEDIR_MATCH_HUNGER, // Hunger (They Hunger)
	GAMEDIR_MATCH_WANTED, // wantedsp (Half-Life: Wanted!)
	GAMEDIR_MATCH_AOMDC, // AoMDC (Afraid of Monsters: Director's Cut)
	GAMEDIR_MATCH_ECHOES, // echoes (Half-Life: Echoes)
	GAMEDIR_MATCH_POKE646, // poke646 (Poke646)
	GAMEDIR_MATCH_COF, // CryOfFear (Cry of Fear)
	GAMEDIR_MATCH_PARANOIA, // paranoia (PARANOIA)
	GAMEDIR_MATCH_BSHIFT, // bshift (Half-Life: Blue Shift)
	GAMEDIR_MATCH_TWHLTOWER2, // twhltower2 (TWHL Tower 2)
	GAMEDIR_MATCH_URBICIDE, // hl_urbicide (Half-Life: Urbicide)
	GAMEDIR_MATCH_VISITORS, // visitors (Half-Life: Visitors)
	GAMEDIR_MATCH_HLRATS_PARASOMNIA // hrp (Half-Rats: Parasomnia)
	GAMEDIR_MATCH_DECAY, // decay (Half-Life: Decay, fan-made port)
	GAMEDIR_MATCH_CAGED, // caged_fgs (Half-Life: C.A.G.E.D)
	GAMEDIR_MATCH_HC2, // hc (Hazardous Course 2)
	GAMEDIR_MATCH_BLACKOPS, // blackops (Black Ops)
	GAMEDIR_MATCH_TIMELINE2, // timeline2 (Timeline 2)
	GAMEDIR_MATCH_UPLINK, // SteamLink (Half-Life: Uplink)
	GAMEDIR_MATCH_HALFQUAKE, // hqtrilogy (Halfquake Trilogy)
	GAMEDIR_MATCH_AZURESHEEP, // azuresheep (Azure Sheep)
	GAMEDIR_MATCH_HLSWEET, // SHL (Half-Life: Sweet)
	GAMEDIR_MATCH_BIGLOLLY, // biglolly (Big Lolly)
	GAMEDIR_MATCH_TXP, // lm_txp (The Xeno Project)
	GAMEDIR_MATCH_HTC, // htc (Hunt the Cunt)
	GAMEDIR_MATCH_INVASION, // invasion (Half-Life: Invasion)

	// Multiplayer-only games:
	GAMEDIR_MATCH_MP_TFC, // tfc (Team Fortress Classic)
	GAMEDIR_MATCH_MP_CS, // cstrike (Counter-Strike)
	GAMEDIR_MATCH_MP_CSCZ, // czero (Counter-Strike: Condition Zero)
	GAMEDIR_MATCH_MP_DOD, // dod (Day of Defeat)
	GAMEDIR_MATCH_MP_DMC, // dmc (Deathmatch Classic)
	GAMEDIR_MATCH_MP_RICOCHET, // ricochet (Ricochet)

	// Less worth mods start here:
	GAMEDIR_MATCH_BEFORE, // before (Half-Life: Before)
	GAMEDIR_MATCH_CROWBAROFTIME, // tetsu0_cot (Crowbar of Time)
	GAMEDIR_MATCH_MINIMICUS, // MINIMICUS (Minimicus)
	GAMEDIR_MATCH_GLOOM, // gloom (Gloom)
	GAMEDIR_MATCH_CONSTRUCTION, // construction (Construction)
	GAMEDIR_MATCH_PLAGUE // plague (Plague)
} BXT_ENUM_GAMEDIR_MATCH;
