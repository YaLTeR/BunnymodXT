#if !defined( PARSEFUNC_H )
#define PARSEFUNC_H

typedef struct clc_func_s
{
	// Opcode
	unsigned char opcode;
	// Display Name
	char *pszname;
	// Parse function
	void (*pfnParse)(struct client_t *cl);
} clc_func_t;

typedef struct svc_func_s
{
	// Opcode
	unsigned char opcode;
	// Display Name
	char *pszname;
	// Parse function
	void (*pfnParse)(void);
} svc_func_t;

#endif
