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

typedef unsigned char uchar;
typedef void (* encoder_t)(struct delta_s *, uchar *, uchar *);

struct delta_stats_t {
    int sendcount;
    int receivedcount;
};

struct delta_description_s {
    int fieldType;
    char fieldName[32];
    int fieldOffset;
    short fieldSize;
    int significant_bits;
    float premultiply;
    float postmultiply;
    short flags;
    struct delta_stats_t stats;
};

struct delta_s {
    int dynamic;
    int fieldCount;
    char conditionalencodename[32];
    encoder_t conditionalencode;
    struct delta_description_s * pdd;
};

struct delta_info_s {
    struct delta_info_s * next;
    char * name;
    char * loadfile;
    delta_s * delta;
};