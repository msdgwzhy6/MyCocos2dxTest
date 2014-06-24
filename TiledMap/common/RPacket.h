#ifndef _RPACKET_H
#define _RPACKET_H

#include "ByteBuffer.h"
namespace net{

class WPacket;
class RPacket{
	friend class WPacket;
public:
	RPacket(const ByteBuffer *buffer):buffer(const_cast<ByteBuffer*>(buffer)->IncRef()),rpos(4){}

	RPacket(const RPacket &o):buffer(const_cast<ByteBuffer*>(o.buffer)->IncRef()),rpos(o.rpos){}

	~RPacket(){
		(const_cast<ByteBuffer*>(buffer))->DecRef();
	}

	unsigned char ReadUint8(){
		unsigned char ret = buffer->ReadUint8(rpos);
		rpos += sizeof(ret);
		return ret;
	}

	unsigned short ReadUint16(){
		unsigned short ret = buffer->ReadUint16(rpos);
		rpos += sizeof(ret);
		return ret;
	}

	unsigned int ReadUint32(){
		unsigned int ret = buffer->ReadUint32(rpos);
		rpos += sizeof(ret);
		return ret;
	}

	unsigned long long ReadUint64(){
		unsigned long long ret = buffer->ReadUint64(rpos);
		rpos += sizeof(ret);
		return ret;
	}

	float ReadFloat(){
		float ret = buffer->ReadFloat(rpos);
		rpos += sizeof(ret);
		return ret;
	}

	double ReadDouble(){
		double ret = buffer->ReadDouble(rpos);
		rpos += sizeof(ret);
		return ret;
	}

	void *ReadBin(size_t &len){
		len = ReadUint32();
		void *ret = buffer->ReadBin(rpos);
		rpos += len;
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
};

}

#endif
