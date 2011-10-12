#ifndef FILEIO_H
#define FILEIO_H

#include "cvm.h"

// Copyright (C) 2000,2001 Bob Dayley


class ICVMGenericFile{
	int err; // non-zero means error occured on read
	int reverseendian; // if non-zero, endian will be reversed on return

public:

	ICVMGenericFile(){
		reverseendian=0;
		err=0;
	};

	virtual void Read(void *to,u32 len)=0; // must be defined by derived class
	virtual void Write(void *from,u32 len)=0; // must be defined by derived class
	virtual void SetAbsPos(u32 pos)=0; // must be defined by derived class
	virtual long GetAbsPos(void)=0; // must be defined by derived class
	virtual void SetRelPos(u32 pos)=0; // must be defined by derived class
	
	inline int Error(void){return err;};
	inline void Error(int terr){err=terr;};

	inline int GetReverseEndian(void){return reverseendian;};

	inline void SetReverseEndian(int flag){
		reverseendian=flag;
	};

	inline void Read(char *s){
		u8  c;
		do{
			Read(c);
			*s++ =c;
		}while(c);	
	};

								
	inline void Read(char *s,int maxchars){
		u8  c=1;
		int cnt=0;
		while(c && cnt<maxchars){
			Read(c);
			*s++ =c;
			++cnt;
		};	
		*s=0;
	};


	inline void Read(u8 &c){
		Read(&c,1);
	};

	inline void Read(s8 &c){Read((u8 &)c);};


	inline void Read(u16 &c){
		u8 buf[2];
		Read(buf,sizeof(buf));
		if(reverseendian)
			c=(((u16) buf[0])<<8) | ((u16) buf[1]);
		else
			c=(((u16) buf[1])<<8) | ((u16) buf[0]);
	};


	inline void Read(s16 &c){Read((u16 &)c);};

	inline void Read(u32 &c){
		u8 buf[4];
		Read(buf,sizeof(buf));
		if(reverseendian)
			c=(((u32) buf[0])<<24) | (((u32) buf[1])<<16) |
				(((u32) buf[2])<<8) | ((u32) buf[3]);
		else
			c=(((u32) buf[3])<<24) | (((u32) buf[2])<<16) |
				(((u32) buf[1])<<8) | ((u32) buf[0]);
	};

	inline void Read(s32 &c){Read((u32 &) c);};


	inline void Write(char *s){
		u8 c;
		do{
			c=*s++;
			Write(c);
		}while(c);	
	};

	inline void Write(u8 c){
		Write(&c,sizeof(c));
	};

	inline void Write(s8 c){Write((u8)c);};

	inline void Write(u16 c){
		u8 buf[2];
		if(reverseendian){
			buf[0]=c>>8;
			buf[1]=c;
		}else{
			buf[1]=c>>8;
			buf[0]=c;

		}
		Write(buf,sizeof(buf));
	};

	inline void Write(s16 c){Write((u16)c);};

	inline void Write(u32 c){
		u8 buf[4];
		if(reverseendian){
			buf[0]=c>>24;
			buf[1]=c>>16;
			buf[2]=c>>8;
			buf[3]=c;
		}else{
			buf[3]=c>>24;
			buf[2]=c>>16;
			buf[1]=c>>8;
			buf[0]=c;
		}
		Write(buf,sizeof(buf));

	};

	inline void Write(s32 c){Write((u32)c);};


};


class ICVMFile : public ICVMGenericFile{
	FILE *thefile;
public:

	ICVMFile(FILE *f) : ICVMGenericFile(){thefile=f;};

	inline FILE *GetFile(void){return thefile;};

	virtual void Read(void *where,u32 len){
		if(Error())
			return;
		if(fread(where,1,len,GetFile())!=len)
			Error(1);
	};
											
	virtual void Write(void *where,u32 len){
		if(Error())
			return;
		if(fwrite(where,1,len,GetFile())!=len)
			Error(1);
	};
											
	virtual void SetAbsPos(u32 pos){
		if(Error())
			return;
		Error(fseek(GetFile(),pos,SEEK_SET));
	};

	virtual long GetAbsPos(void){
		if(Error())
			return 0;
		return ftell(GetFile());
	};

	virtual void SetRelPos(u32 pos){
		if(Error())
			return;
		Error(fseek(GetFile(),pos,SEEK_CUR));
	};


};


// read/write memory as a generic file
												
class ICVMMemFile : public ICVMGenericFile{
	char *basemem;
	long pos;	
	long size;
public:

	ICVMMemFile(void *baseaddr,long tsize) : ICVMGenericFile(){
		basemem=(char *)baseaddr;
		pos=0;
		size=tsize;
	};


	virtual void Read(void *where,u32 len){
		if((pos+len)>size)
			Error(1);
		if(Error())
			return;
		memcpy(where,(void *)(basemem+pos),len);
		pos+=len;
	};
											
	virtual void Write(void *where,u32 len){
		if((pos+len)>size)
			Error(1);
		if(Error())
			return;
		memcpy((void *)(basemem+pos),where,len);
		pos+=len;
	};
											
	virtual void SetAbsPos(u32 npos){
		if(npos>size || npos<0)
			Error(1);
		if(Error())
			return;
		pos=npos;
	};

	virtual long GetAbsPos(void){
		if(Error())
			return 0;
		return pos;
	};

	virtual void SetRelPos(u32 npos){
		if((pos+npos)>size || (pos+npos)<0)
			Error(1);
		if(Error())
			return;
		pos+=npos;
	};


};




#endif



