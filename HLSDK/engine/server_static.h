#if !defined( SERVER_STATIC_H )
#define SERVER_STATIC_H

#ifdef HLSDK10_BUILD
struct server_static_t
{
	int maxclients;
	byte align[28684];
	struct client_t *clients;
};
#else
struct server_static_t
{
	int dll_initialized;
	struct client_t *clients;
	int maxclients;
};
#endif

#endif