#pragma once

enum class MessageType : unsigned char {
	TIME = 0x00,
	CLIP = 0x01,
	WATER = 0x02,
	FRAMETIME_REMAINDER = 0x03
};

#define MQ_NAME "BunnymodXT-TASView"
