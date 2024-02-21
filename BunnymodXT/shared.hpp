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

#define BIG_MAP_SIZE 32768 // +-BIG_MAP_SIZE so 64k x 64k map should have value of 32k
#define BIG_MAP_MAX_VELOCITY 8192 // +-8192

#define	BXT_FLAGS_BIG_MAP (1<<0)