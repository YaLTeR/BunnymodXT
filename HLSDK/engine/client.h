#ifndef CLIENT_H
#define CLIENT_H

#include "../common/entity_state.h"

typedef struct
{
	int num_entities;
	unsigned char flags[32];
	entity_state_s *entities;
} packet_entities_t;

typedef struct
{
	int num_entities;
	unsigned char flags[128];
	entity_state_s *entities;
} packet_entities_25th_anniversary_t;

typedef struct
{
	double receivedtime;
	double latency;
	qboolean invalid;
	qboolean choked;
	entity_state_t playerstate[32];
	double time;
	clientdata_t clientdata;
	weapon_data_t weapondata[64];

	packet_entities_t packet_entities;

	unsigned short clientbytes;
	unsigned short playerinfobytes;
	unsigned short packetentitybytes;
	unsigned short tentitybytes;
	unsigned short soundbytes;
	unsigned short eventbytes;
	unsigned short usrbytes;
	unsigned short voicebytes;
	unsigned short msgbytes;
} frame_t;

typedef struct
{
	double receivedtime;
	double latency;
	qboolean invalid;
	qboolean choked;
	entity_state_t playerstate[32];
	double time;
	clientdata_t clientdata;
	weapon_data_t weapondata[64];

	packet_entities_25th_anniversary_t packet_entities;

	unsigned short clientbytes;
	unsigned short playerinfobytes;
	unsigned short packetentitybytes;
	unsigned short tentitybytes;
	unsigned short soundbytes;
	unsigned short eventbytes;
	unsigned short usrbytes;
	unsigned short voicebytes;
	unsigned short msgbytes;
} frame_25th_anniversary_t;

#endif