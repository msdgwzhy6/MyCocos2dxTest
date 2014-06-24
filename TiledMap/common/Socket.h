#ifndef _SOCKET_H
#define _SOCKET_H

#ifdef _WIN
#include <winsock2.h>
#include <WinBase.h>
#include <Winerror.h>
#include <stdio.h>
#include <WS2tcpip.h>

#define INVAILD_FD INVALID_SOCKET

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) expression
#endif


#else

#include    <unistd.h>
#include    <sys/select.h>
#include	<sys/types.h>	
#include	<sys/socket.h>	
#include	<sys/time.h>	
#include	<time.h>		
#include	<netinet/in.h>	
#include	<arpa/inet.h>	
#include	<errno.h>
#include	<fcntl.h>		
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	
#include	<sys/uio.h>		
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		
#include    <net/if.h>
#include    <sys/ioctl.h>
#include    <netinet/tcp.h>
#include    <fcntl.h>
#include    <stdint.h>

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression)\
	({ long int __result;\
	do __result = (long int)(expression);\
	while(__result == -1L&& errno == EINTR);\
	__result;})
#endif

typedef int SOCKET;
#define INVAILD_FD -1
#endif

#include "WPacket.h"
#include <list>
#include "dlist.h"

#define EV_READ 0x1
#define EV_WRITE (0x1 << 1)

namespace net{

enum{
	listening = 1,
	connecting,
	establish,
	timeout,
	closeing,
};

class WPacket;
class Reactor;
class RPacket;
class Socket;
typedef void (*cb_connect)(Socket*);
typedef void (*cb_accept)(Socket*);
typedef void (*cb_packet)(Socket*,RPacket&);
typedef void (*cb_disconnected)(Socket*);

class Socket:public dnode{
	friend class Reactor;
public:
	Socket(int family,int type,int protocol);
	Socket(SOCKET fd);
	bool SetNonBlock();
	int  Send(WPacket&);
	int  Bind(Reactor *reactor,cb_packet,cb_disconnected);
	void Close();
	int  Event(){return event;}
	int  State(){return state;}
	bool  Listen(Reactor*,const char *ip,int port,cb_accept);
	bool  Connect(Reactor *reactor,const char *ip,int port,unsigned long timeout,cb_connect);
	//const std::string& Err(){return err_description;}
	SOCKET Fd(){return fd;}
	void SetUd(void *ud){this->ud = ud;}
	void *GetUd(){return ud;}
private:
	int  rawSend();
	void onReadAct();
	void onWriteAct();
	void doAccept();
	void doConnect();
	void unpack();

	void IncRef(){
#ifdef _WIN
		InterlockedIncrement(&refCount);
#else
		__sync_add_and_fetch(&refCount,1);
#endif
	}

	void DecRef(){
#ifdef _WIN
		if(InterlockedDecrement(&refCount) <= 0)
#else
		if(__sync_sub_and_fetch(&refCount,1) <=0 )
#endif
			delete this;		
	}

private:
	~Socket(){}
	Socket(const Socket&);
	Socket& operator = (const Socket&);
	cb_connect _cb_connect;
	cb_accept  _cb_accept;
	cb_packet  _cb_packet;
	cb_disconnected _cb_disconnected;
	SOCKET fd;
	static const int recvbuf_size = 4096;
	static const int maxpacket_size = 4096;
	char   recvbuf[recvbuf_size];
	int    upos;
	char   unpackbuf[maxpacket_size*2];
	int    event;
	std::list<WPacket> sendlist;
	size_t wpos;
	Reactor *reactor;
	int     state;
	//std::string  err_description;
	volatile long refCount;
	unsigned long connect_timeout;
	bool    writeable;
	void    *ud;
};
}
#endif
