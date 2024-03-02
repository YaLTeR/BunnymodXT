// cmd.h -- Command buffer and command execution

//===========================================================================

/*
Any number of commands can be added in a frame, from several different sources.
Most commands come from either keybindings or console line input, but remote
servers can also send across commands and entire text files can be execed.
The + command line options are also added to the command buffer.
The game starts with a Cbuf_AddText ("exec quake.rc\n"); Cbuf_Execute ();
*/

//===========================================================================

/*
Command execution takes a null terminated string, breaks it into tokens,
then searches for a command or variable that matches the first token.
Commands can come from three sources, but the handler functions may choose
to dissallow the action or forward it to a remote server if the source is
not apropriate.
*/

#if !defined( CMDH )
#define CMDH
#ifdef _WIN32
#pragma once
#endif

typedef void (*xcommand_t) (void);

typedef struct cmd_function_s
{
	struct cmd_function_s	*next;
	char					*name;
	xcommand_t				function;

	// Align of structure is exactly the same as in the Quake code and latest GoldSrc engine up to this point.

	int						flags;
} cmd_function_t;

typedef struct sizebuf_t
{
	char *buffername; // char *
	unsigned flags; // unsigned short
	char *data; // byte *
	unsigned maxsize; // int
	unsigned cursize; // int
} sizebuf_s;

#endif
