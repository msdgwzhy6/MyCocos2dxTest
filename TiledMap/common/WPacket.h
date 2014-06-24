#ifndef _WPACKET_H
#define _WPACKET_H

//#include "ByteBuffer.h"
#include "RPacket.h"
namespace net{

class WPacket{

public:
	//前4个字节用于表示包长度,所以WPacket创建后已经有4个字节的有效数据
	WPacket(unsigned short size = 64):buffer(new ByteBuffer(size)),wpos(0)
	{
		buffer->WriteUint32(0,4);
		wpos += 4;
	}

	WPacket(const WPacket &o){
		buffer = o.buffer->IncRef();
		wpos = 0;
	}

	WPacket(const RPacket &r){
		buffer = const_cast<ByteBuffer*>(r.buffer)->IncRef();
		wpos = 0;
	}

	~WPacket(){
		buffer->DecRef();
	}

	void WriteUint8(unsigned char v){
		CopyOnWrite();
		unsigned int size = buffer->ReadUint32(0);
		buffer->WriteUint8(wpos,v);
		wpos += sizeof(v);
		buffer->WriteUint32(0,size+sizeof(v));
	}

	void WriteUint16(unsigned short v){
		CopyOnWrite();
		unsigned	int size = buffer->ReadUint32(0);
		buffer->WriteUint16(wpos,v);
		wpos += sizeof(v);
		buffer->WriteUint32(0,size+sizeof(v));
	}

	void WriteUint32(unsigned int v){
		CopyOnWrite();
		unsigned int size = buffer->ReadUint32(0);
		buffer->WriteUint32(wpos,v);
		wpos += sizeof(v);
		buffer->WriteUint32(0,size+sizeof(v));
	}

	void WriteUint64(unsigned long long v){
		CopyOnWrite();
		unsigned int size = buffer->ReadUint32(0);
		buffer->WriteUint64(wpos,v);
		wpos += sizeof(v);
		buffer->WriteUint32(0,size+sizeof(v));
	}

	void WriteFloat(float v){
		CopyOnWrite();
		unsigned int size = buffer->ReadUint32(0);
		buffer->WriteFloat(wpos,v);
		wpos += sizeof(v);
		buffer->WriteUint32(0,size+sizeof(v));
	}

	void WriteDouble(double v){
		CopyOnWrite();
		unsigned int size = buffer->ReadUint32(0);
		buffer->WriteDouble(wpos,v);
		wpos += sizeof(v);
		buffer->WriteUint32(0,size+sizeof(v));
	}

	void WriteBin(void *v,size_t len){
		CopyOnWrite();
		unsigned int size = buffer->ReadUint32(0);
		WriteUint32(len);//首先写入长度
		buffer->WriteBin(wpos,v,len);
		wpos += len;
		buffer->WriteUint32(0,size+4+len);
	}

	void WriteString(const char *v){
		CopyOnWrite();
		WriteBin((void*)v,strlen(v)+1);
	}

	const ByteBuffer* Buffer(){
		return buffer;
	}

	size_t Size(){
		return buffer->ReadUint32(0);
	}

private:
	WPacket& operator = (const WPacket &o){
		if(&o != this){
			if(buffer){
				buffer->DecRef();
				buffer = o.buffer->IncRef();
			}
			wpos = 0;
		}	
		return *this;
	} 

	void CopyOnWrite(){
		if(wpos == 0){
			ByteBuffer *tmp = new ByteBuffer(*buffer);
			buffer->DecRef();
			buffer = tmp;
			wpos = buffer->ReadUint32(0);
		}
	}

	size_t      wpos;    
	ByteBuffer *buffer;
};


}


#endif
