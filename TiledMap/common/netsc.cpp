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

static inline st* ON_GC_CREATE(net::RPacket &rpk){
	stCreate *st = new stCreate;
	return st;
}
REG_HANDLER(CMD_GC_CREATE,ON_GC_CREATE);

static inline st* ON_GC_BEGINPLY(net::RPacket &rpk){
	stBegPly *st = new stBegPly;
	return st;
}
REG_HANDLER(CMD_GC_BEGINPLY,ON_GC_BEGINPLY);

static inline st* ON_SC_ENTERMAP(net::RPacket &rpk){
	stEnterMap *st = new stEnterMap;
	st->maptype = rpk.ReadUint16();
	st->id = rpk.ReadUint32();
	return st;
}
REG_HANDLER(CMD_SC_ENTERMAP,ON_SC_ENTERMAP);

static inline st* ON_SC_ENTERSEE(net::RPacket &rpk){
	stEnterSee *st = new stEnterSee;
	st->id = rpk.ReadUint32();
	st->avattype = rpk.ReadUint8();
	st->avatid = rpk.ReadUint16();
	st->nickname = rpk.ReadString();
	st->x = rpk.ReadUint16();
	st->y = rpk.ReadUint16();
	st->dir = rpk.ReadUint8();
	return st;
}
REG_HANDLER(CMD_SC_ENTERSEE,ON_SC_ENTERSEE);

static inline st* ON_SC_LEVSEE(net::RPacket &rpk){
	stLevSee *st = new stLevSee;
	st->id = rpk.ReadUint32();
	return st;
}
REG_HANDLER(CMD_SC_LEAVESEE,ON_SC_LEVSEE);

static inline st* ON_SC_MOV(net::RPacket &rpk){
	stMov *st = new stMov;
	st->id = rpk.ReadUint32();
	st->x = rpk.ReadUint16();
	st->y = rpk.ReadUint16();
	return st;
}
REG_HANDLER(CMD_SC_MOV,ON_SC_MOV);

}