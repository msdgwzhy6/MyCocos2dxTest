#include "netcs.h"

namespace comm{


class Connection{
public:
	Connection(int family,int type,int protocol,on_connect cb_on_connect,
			 on_connect_fail cb_on_connect_fail,on_disconnected cb_on_disconnected)
			 :cb_on_connect(cb_on_connect),cb_on_connect_fail(cb_on_connect_fail),
			 cb_on_disconnected(cb_on_disconnected)
	{
		m_sock = new net::Socket(family,type,protocol);
		m_sock->SetUd(this);
	}

	~Connection(){
		m_sock->Close();
	}

	int  Send(net::WPacket &wpk){
		return m_sock->Send(wpk);
	}
	on_connect cb_on_connect;
	on_connect_fail cb_on_connect_fail;
	on_disconnected cb_on_disconnected;
	net::Socket *m_sock;
};


static Connection  *g_connection;
static net::Reactor *g_reactor;
static bool is_init = false; 


bool Init(){
	if(is_init) return false;
	if(!net::init()) return false;
	g_reactor = new net::Reactor;
	is_init = true;
	return true;
}

extern void client_process_packet(net::Socket*,net::RPacket&);

static void cb_disconnected(net::Socket *s){
	Connection *c = (Connection*)s->GetUd();
	c->cb_on_disconnected();
}

static void cb_connected(net::Socket *s){
	Connection *c = (Connection*)s->GetUd();
	if(s->State() == net::establish){
		s->Bind(g_reactor,client_process_packet,cb_disconnected);
		g_connection = c;
		c->cb_on_connect();
	}else{
		c->cb_on_connect_fail();
	}
}



bool Connect(const char *ip,unsigned short port,on_connect cb_on_connect,
			 on_connect_fail cb_on_connect_fail,on_disconnected cb_on_disconnected){
		Connection *c = new Connection(AF_INET, SOCK_STREAM,IPPROTO_TCP,cb_on_connect,cb_on_connect_fail,cb_on_disconnected);
		if(!c->m_sock->Connect(g_reactor,ip,port,0,cb_connected))
		{
			delete c;
			return false;
		}
		return true;
}

void Close(){
	if(g_connection){
		delete g_connection;
		g_connection = NULL;
	}
}

void Run(unsigned int ms){
	g_reactor->LoopOnce(ms);
}

void begply(){
	net::WPacket wpk;
	wpk.WriteUint16(CMD_CS_BEGPLY);
	g_connection->Send(wpk);
}

void move(unsigned long x,unsigned long y){
	net::WPacket wpk;
	wpk.WriteUint16(CMD_CS_MOV);
	wpk.WriteUint32(x);
	wpk.WriteUint32(y);
	g_connection->Send(wpk);
}

}