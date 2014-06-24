#ifndef _NET_H
#define _NET_H

#include "Socket.h"
#include "Reactor.h"
#include "RPacket.h"
#include "WPacket.h"
#include <signal.h>
namespace net{
static inline bool init(){
#ifdef _WIN	
   WSADATA wsaData;
   if(WSAStartup(0x0202,&wsaData) != 0)
   {
      WSACleanup();
      return false;
   }
#else
	signal(SIGPIPE,SIG_IGN);
#endif   
   return true;
}

static inline void clean(){
#ifdef _WIN   
   WSACleanup();
#endif   
}

}

#endif
