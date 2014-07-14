#ifndef _NETCS_H
#define _NETCS_H

#include "netcmd.h"
#include "net.h"

namespace comm{

bool Init();

typedef void (*on_connect)();
typedef void (*on_connect_fail)();
typedef void (*on_disconnected)();

bool Connect(const char *ip,unsigned short port,on_connect,on_connect_fail,on_disconnected);

void Close();

void Run(unsigned int ms = 0);//运行主消息循环


//CMD_CA_LOGIN


void LOGIN(const std::string &actname);

void CREATE(const std::string &nickname);

void ENTERMAP();

void MOV(int x,int y);


//void begply();

//void move(unsigned long x,unsigned long y);

/*
void entermap(unsigned char mapid,const std::string &name,unsigned char avatid,double speed);

void attack(unsigned char mapid,unsigned long long sufferer,unsigned short skill,unsigned short damage,unsigned char dir);

void move(unsigned char mapid,unsigned int x,unsigned int y,double speed);

void move_stop(unsigned char mapid,unsigned int x,unsigned int y,unsigned char dir);

void speed_change(unsigned char mapid,double speed);

void hp_change(unsigned char mapid,short hpchange);

void hit(unsigned char mapid,unsigned long long id);

void hide(unsigned char mapid,bool hide);
*/

}

#endif