#ifndef DELTA_H
#define DELTA_H
#ifdef _WIN32
#pragma once
#endif

typedef void (*encoder_t)(struct delta_s *, byte *, byte *);

typedef struct delta_definition_s
{
	char *fieldName;
	int fieldOffset;
} delta_definition_t;

typedef struct delta_definition_list_s
{
	struct delta_definition_list_s *next;
	char *ptypename;
	int numelements;
	delta_definition_t *pdefinition;
} delta_definition_list_t;

typedef struct delta_stats_s
{
	int sendcount;
	int receivedcount;
} delta_stats_t;

typedef struct delta_description_s
{
	int fieldType;
	char fieldName[32];
	int fieldOffset;
	short fieldSize;
	int significant_bits;
	float premultiply;
	float postmultiply;
	short flags;
	delta_stats_t stats;
} delta_description_t;

typedef struct delta_link_s
{
	struct delta_link_s *next;
	delta_description_t *delta;
} delta_link_t;

typedef struct delta_s 
{
	int dynamic;
	int fieldCount;
	char conditionalencodename[32];
	encoder_t conditionalencode;
	struct delta_description_s *pdd;
} delta_t;

typedef struct delta_registry_s
{
	struct delta_registry_s *next;
	char *name;
	delta_t *pdesc;
} delta_registry_t;

typedef struct delta_encoder_s
{
	struct delta_encoder_s *next;
	char *name;
	encoder_t conditionalencode;
} delta_encoder_t;

typedef struct delta_info_s
{
	struct delta_info_s *next;
	char *name;
	char *loadfile;
	delta_t *delta;
} delta_info_t;

#endif