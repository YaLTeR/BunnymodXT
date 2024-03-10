#if !defined( STATES_H )
#define STATES_H

typedef enum 
{
	// No map loaded
	ss_dead,
	// Spawning level edicts
	ss_loading,
	// Actively running
	ss_active
} server_state_t;

typedef enum
{
	// A dedicated server with no ability to start a client
	ca_dedicated,
	// Full screen console with no connection
	ca_disconnected,
	// Challenge requested, waiting for response or to resend connection request.
	ca_connecting,
	// valid netcon, talking to a server, waiting for server data
	ca_connected,
	// valid netcon, autodownloading
	ca_uninitialized,
	// d/l complete, ready game views should be displayed
	ca_active
} cactive_t;

#endif
