#include "Socket.h"
#include "Reactor.h"
#include "systime.h"
namespace net{

Socket::Socket(int family,int type,int protocol):reactor(NULL),
	writeable(true),refCount(1),state(0),wpos(0),event(0),upos(0),_cb_disconnected(NULL),_cb_packet(NULL),
	_cb_accept(NULL),_cb_connect(NULL),connect_timeout(0)
{
//#ifdef _WIN
	fd = ::socket(family,type,protocol);
	if(fd < 0) exit(0);
//#else

//#endif
}

Socket::Socket(SOCKET fd):reactor(NULL),
	writeable(true),refCount(1),state(0),wpos(0),event(0),upos(0),_cb_disconnected(NULL),_cb_packet(NULL),
	_cb_accept(NULL),_cb_connect(NULL),fd(fd),connect_timeout(0){
}

bool  Socket::Listen(Reactor *reactor,const char *ip,int port,cb_accept cb)
{
	if(!reactor || !ip || !cb) return false;
	struct sockaddr_in servaddr;
	memset((void*)&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(port);
	if(::bind(fd,(const sockaddr *)&servaddr,sizeof(servaddr)) < 0)
		return false;

	if(::listen(fd,256) < 0)
		return false;
	SetNonBlock();
	_cb_accept = cb;
	reactor->Add(this,EV_READ);
	state = listening;
	return true;
}

bool Socket::Connect(Reactor *reactor,const char *ip,int port,unsigned long timeout,cb_connect cb)
{
	if(!reactor || !ip || !cb) return false;
	struct sockaddr_in remote;	
	remote.sin_family = AF_INET;
	remote.sin_port = htons(port);
	remote.sin_addr.s_addr = inet_addr(ip);
	if(!SetNonBlock()){ 
		//exit(0);
		printf("Connect SetNonBlock error\n");
		return false;
	}
	_cb_connect = cb;
	if(timeout) connect_timeout = ::GetSystemMs() + timeout;
#ifdef _WIN
	if(::connect(fd,(const sockaddr *)&remote,sizeof(remote)) != SOCKET_ERROR){
#else
	if(::connect(fd,(const sockaddr *)&remote,sizeof(remote)) == 0){
#endif
		state = establish;
		_cb_connect(this);
		return true;
	}else{
#ifdef _WIN
		if(WSAGetLastError() != WSAEWOULDBLOCK){
#else		
		if(errno != EINPROGRESS){
#endif	
			state = 0;
			_cb_connect(this);
		}
		
	}
	reactor->Add(this,EV_WRITE);
	state = connecting;
	return true;
}


void Socket::doAccept(){
	for(;;){
		SOCKET clientfd;
		struct sockaddr sa;
#ifdef _WIN		
		int   addrlen = sizeof(sa);
#else
		socklen_t addrlen = sizeof(sa);
#endif		
		if((clientfd = TEMP_FAILURE_RETRY(::accept(fd,&sa,&addrlen))) == INVAILD_FD){
			return;
		}
		Socket *client = new Socket(clientfd);
		client->state = establish;
		_cb_accept(client);
	}
}

void Socket::unpack(){
	int pos = 0;
	while(state == establish && pos < upos && upos-pos >= 4){
		int len = *(int*)&unpackbuf[pos];
		if(len <= 0 || len > maxpacket_size)
		{
			Close();
			return;
		}
		if(upos-pos >= len){
			ByteBuffer *b = new ByteBuffer(len);
			b->WriteBin(0,(void*)&unpackbuf[pos],len);
			pos += (len);
			RPacket rpk(b);
			_cb_packet(this,rpk);
			b->DecRef();
		}else
			break;
	}
	int size = upos - pos;
	if(upos - pos < maxpacket_size){
		memmove(unpackbuf,&unpackbuf[pos],size);
		upos = size;
	}
	//for test
	/*ByteBuffer *b = new ByteBuffer(upos);
	b->WriteBin(0,(void*)&unpackbuf[0],upos);
	_cb_packet(this,RPacket(b));
	upos = 0;
	b->DecRef();*/
}

void Socket::onReadAct()
{
	if(state == listening)
		doAccept();
	else if(state == connecting)
		doConnect();
	else if(state == establish){
		int n = TEMP_FAILURE_RETRY(::recv(fd,recvbuf,recvbuf_size,0));
		if(n == 0){
			Close();
#ifdef _WIN				
		}else if(n == SOCKET_ERROR){
			if(WSAGetLastError() != WSAEWOULDBLOCK)
#else
	    }else if(n < 0){
			if(errno != EWOULDBLOCK || EAGAIN)
#endif			
				Close();
		}else{
			//将数据拷贝到组包缓冲
			memcpy(&unpackbuf[upos],recvbuf,n);
			upos += n;
			unpack();
		}
	}
}

void Socket::doConnect()
{
    int err = 0;
    socklen_t len = sizeof(err);
	if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&err, &len) == -1)   
        state = 0;
    if(err)
        state = 0;
	if(state != 0)
		state = establish;
	reactor->Remove(this,EV_WRITE);
	_cb_connect(this);
	if(state != establish) this->Close();
}


int  Socket::rawSend(){
	while(writeable && !sendlist.empty()){
		WPacket wpk = sendlist.front();	
		char *buf = (char *)wpk.Buffer()->ReadBin(wpos);
		//for test
		//int len = wpk.Buffer()->Cap()-wpos;
		int len = wpk.Size()-wpos;
		int n = TEMP_FAILURE_RETRY(::send(fd,&buf[wpos],len,0));
		if(n == 0){
			//Close();
			writeable = false;
			return -1;
#ifdef _WIN			
		}else if(n == SOCKET_ERROR){
			if(WSAGetLastError() != WSAEWOULDBLOCK){
#else
	    }else if(n < 0){
			if(errno != EWOULDBLOCK || EAGAIN){
#endif	
				//Close();
				writeable = false;
				return -1;
			}
			else{
				writeable = false;
				if(!(event & EV_WRITE))
					reactor->Add(this,EV_WRITE);
				return 0;
			}
		}else{
			if(n == len){
				//一个包已经发完
				sendlist.pop_front();
				wpos = 0;
			}else{
				wpos += len;		
			}
		}			
	}
	return 0;
}

void Socket::onWriteAct()
{
	if(state == connecting){
		doConnect();
	}else if(state == establish){
		writeable = true;
		if(-1 == rawSend()){
			Close();
		}
	}
}

void  Socket::Close()
{
	if(state != closeing){
		state = closeing;
#ifdef _WIN
		::closesocket(fd);
#else
		::close(fd);
#endif
		if(reactor)
			reactor->Remove(this,EV_WRITE|EV_READ);
		if(_cb_disconnected) _cb_disconnected(this);
		DecRef();
	}
}

int  Socket::Send(WPacket &wpk){
	if(state != establish) return -1;
	sendlist.push_back(wpk);
	return rawSend();
}

int Socket::Bind(Reactor *reactor,cb_packet cb1,cb_disconnected cb2){
	if(state == establish){
		this->reactor = reactor;
		this->reactor->Add(this,EV_READ);
		_cb_packet = cb1;
		_cb_disconnected = cb2;
		return 0;
	}
	return -1;
}

bool Socket::SetNonBlock(){
		int ret;
#ifdef _WIN
		int ioctlvar;

		ioctlvar = 1;
		ret = ioctlsocket(fd, FIONBIO, (unsigned long*)&ioctlvar);
#else
		ret = fcntl(fd, F_SETFL, O_NONBLOCK | O_RDWR);
#endif
		if (ret != 0) {
#ifdef _WIN
			errno = WSAGetLastError();
#endif
			return false;
		}
		return true;
}

}
