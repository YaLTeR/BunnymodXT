#if !defined( USERMSG_H )
#define USERMSG_H
#ifdef _WIN32
#pragma once
#endif

typedef int (*pfnUserMsgHook)(const char *pszName, int iSize, void *pbuf);

typedef struct _UserMsg
{
	int iMsg;
	int iSize;
	char szName[16];
	struct _UserMsg *next;
	pfnUserMsgHook pfn;
} UserMsg;

#endif
