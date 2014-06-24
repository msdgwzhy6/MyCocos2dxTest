#ifndef _NETSC_H
#define _NETSC_H
#pragma  once

#include "net.h"
#include "netcmd.h"
#include "entitydef.h"

namespace comm{

struct st{};

struct cmd_handler{
	void  (*handle_func)(st*);
	st* (*parse_func)(net::RPacket &);
};

typedef void (*__handle_func)(st*);

extern void SetHandleFunc(unsigned short cmd,void  (*handle_func)(st*));

//CMD_SC_BEGPLY
struct stBegPly : public st{
	unsigned long long id;
};

//	CMD_SC_ENTERSEE
struct stEnterSee : public st{
	unsigned long long id;
	int x;
	int y;
	unsigned char  dir;
};


//CMD_SC_LEVSEE
struct stLevSee : public st{
	unsigned long long id;
};

//	CMD_SC_ENDPLY
struct stEndPly : public st{
	
};

//	CMD_SC_MOV
struct stMov : public st{
	unsigned long long id;
	int      x;
	int      y;
};

}

#define SET_HANDLE_FUNC(CMD,FUNC) comm::SetHandleFunc(CMD,(comm::__handle_func)FUNC)
#endif