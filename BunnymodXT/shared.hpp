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
#define LIVESPLIT_PIPE_NAME "LiveSplit"

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