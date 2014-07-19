#ifndef _RPACKET_H
#define _RPACKET_H

#include "ByteBuffer.h"
namespace net{

class WPacket;
class RPacket{
	friend class WPacket;
public:
	RPacket(const ByteBuffer *buffer):buffer(const_cast<ByteBuffer*>(buffer)->IncRef()),rpos(4),
		pklen(buffer->ReadUint32(0)){
			dataremain = pklen;
	}

	RPacket(const RPacket &o):buffer(const_cast<ByteBuffer*>(o.buffer)->IncRef()),rpos(o.rpos),
		pklen(o.pklen){
		dataremain = pklen;
	}

	~RPacket(){
		(const_cast<ByteBuffer*>(buffer))->DecRef();
	}

	unsigned char ReadUint8(){
		if(dataremain < sizeof(unsigned char)) return 0;
		unsigned char ret = buffer->ReadUint8(rpos);
		rpos += sizeof(ret);
		dataremain -= sizeof(ret);
		return ret;
	}

	unsigned short ReadUint16(){
		if(dataremain < sizeof(unsigned short)) return 0;
		unsigned short ret = buffer->ReadUint16(rpos);
		rpos += sizeof(ret);
		dataremain -= sizeof(ret);
		return ret;
	}

	unsigned int ReadUint32(){
		if(dataremain < sizeof(unsigned int)) return 0;
		unsigned int ret = buffer->ReadUint32(rpos);
		rpos += sizeof(ret);
		dataremain -= sizeof(ret);
		return ret;
	}

	unsigned long long ReadUint64(){
		if(dataremain < sizeof(unsigned long long)) return 0;	
		unsigned long long ret = buffer->ReadUint64(rpos);
		rpos += sizeof(ret);
		dataremain -= sizeof(ret);
		return ret;
	}

	float ReadFloat(){
		if(dataremain < sizeof(float)) return 0;
		float ret = buffer->ReadFloat(rpos);
		rpos += sizeof(ret);
		dataremain -= sizeof(ret);
		return ret;
	}

	double ReadDouble(){
		if(dataremain < sizeof(double)) return 0;
		double ret = buffer->ReadDouble(rpos);
		rpos += sizeof(ret);
		dataremain -= sizeof(ret);
		return ret;
	}

	void *ReadBin(size_t &len){
		len = ReadUint32();
		if(!len) return NULL;
		void *ret = buffer->ReadBin(rpos);
		rpos += len;
		dataremain -= len;
		return ret;
	}

	const char *ReadString(){
		size_t len;
		return (const char*)ReadBin(len);
	}

private:

	RPacket& operator = (const RPacket &o){
		if(this != &o){
			(const_cast<ByteBuffer*>(buffer))->DecRef();
			buffer = const_cast<ByteBuffer*>(o.buffer)->IncRef();
			rpos = o.rpos;
		}
		return *this;
	}
	const ByteBuffer *buffer;
	size_t      rpos;
	size_t      pklen;
	size_t      dataremain;
};

}

#endif
