#include "netsc.h"
#include <assert.h>

namespace comm{

static cmd_handler* g_handler[65536];

#define REG_HANDLER(CMD,PARSE_FUNC)\
struct st##CMD:public cmd_handler{\
	st##CMD(){\
	RegHandler(CMD,this,PARSE_FUNC);\
}\
};st##CMD ___st##CMD;

void RegHandler(unsigned short cmd,cmd_handler *handler,st* (*parse_func)(net::RPacket &)){
	g_handler[cmd] = handler;
	g_handler[cmd]->parse_func = parse_func;
}

void SetHandleFunc(unsigned short cmd,void  (*handle_func)(st*)){
	assert(g_handler[cmd]);
	g_handler[cmd]->handle_func = handle_func;
}

void client_process_packet(net::Socket *s,net::RPacket &rpk)
{
	unsigned short cmd = rpk.ReadUint16();
	if(g_handler[cmd]->handle_func)
	{
		st *_st = g_handler[cmd]->parse_func(rpk);
		g_handler[cmd]->handle_func(_st);
		delete _st;
	}
}
static inline st* on_cmd_begply(net::RPacket &rpk){
	stBegPly *st = new stBegPly;
	st->id = rpk.ReadUint64();
	return st;
}
REG_HANDLER(CMD_SC_BEGPLY,on_cmd_begply);

static inline st* on_cmd_entersee(net::RPacket &rpk){
	stEnterSee *st = new stEnterSee;
	st->id = rpk.ReadUint64();
	st->dir = rpk.ReadUint8();
	st->x = rpk.ReadUint32();
	st->y = rpk.ReadUint32();
	return st;
}
REG_HANDLER(CMD_SC_ENTERSEE,on_cmd_entersee);

static inline st* on_cmd_levsee(net::RPacket &rpk){
	stLevSee *st = new stLevSee;
	st->id = rpk.ReadUint64();
	return st;
}
REG_HANDLER(CMD_SC_LEVSEE,on_cmd_levsee);

static inline st* on_cmd_endply(net::RPacket &rpk){
	stEndPly *st = new stEndPly;
	return st;
}
REG_HANDLER(CMD_SC_ENDPLY,on_cmd_endply);

static inline st* on_cmd_mov(net::RPacket &rpk){
	stMov *st = new stMov;
	st->id= rpk.ReadUint64();
	st->x = rpk.ReadUint32();
	st->y = rpk.ReadUint32();
	return st;
}
REG_HANDLER(CMD_SC_MOV,on_cmd_mov);
	

}