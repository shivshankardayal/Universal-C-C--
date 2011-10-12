#include "cvm.h"
#include "sys.h"
#include "file.h"
#include "sysstruct.h"

// Copyright (C) 2000,2001 Bob Dayley

#ifdef __WIN32__
#include <windows.h>
#include <stdlib.h>
#include <float.h>
#endif // __WIN32__

#ifdef UNIXSYSTEM

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "icvmerr.h"


#endif // UNIXSYSTEM

// round memsz up to page boundary and return it

u32  RoundToPageSize(s32 memsz){
	s32 pz=i_getpagesize();
	s32 pages=memsz/pz;
	while((pz * pages) < memsz)
		++pages;
	memsz=pz * pages;
	return memsz;
};


extern "C"{

// read and write memory protect functions
// sets read or read/write protections on page blocks containing mem.
// USE WITH CAUTION

void i_memreadonly(void *mem,s32 memsz){
	static s32 pagesz;
	s8 *m=(s8 *)mem;
	s32 dm=(s32)mem;
	s32 newsz;
	if(pagesz==0)
		pagesz=i_getpagesize();
	dm&=(pagesz-1); // find alignment of address
	m-=dm; // back it up to start of relavant block
	memsz+=dm; // add to size
	newsz=memsz & ~pagesz;
	if(newsz != memsz) // spans 2 pages
		newsz+=pagesz;
#ifdef __WIN32__
	{
		DWORD old;
		int status;
		status=VirtualProtect(m,newsz,PAGE_READONLY,&old);
//fprintf(stderr,"readony access to $%x size %d =%d\n",m,newsz,status);
	}
	return;
#endif // __WIN32__

#ifdef UNIXSYSTEM
	mprotect(m,newsz,PROT_READ);
	return;
#endif // UNIXSYSTEM

	NOT_IMPLEMENTED;
}

void i_memnoaccess(void *mem,s32 memsz){
	static s32 pagesz;
	s8 *m=(s8 *)mem;
	s32 dm=(s32)mem;
	s32 newsz;
	if(pagesz==0)
		pagesz=i_getpagesize();
	dm&=(pagesz-1); // find alignment of address
	m-=dm; // back it up to start of relavant block
	memsz+=dm; // add to size
	newsz=memsz & ~pagesz;
	if(newsz != memsz) // spans 2 pages
		newsz+=pagesz;
#ifdef __WIN32__
	{
		DWORD old;
		int status;
		status=VirtualProtect(m,newsz,PAGE_NOACCESS,&old);
//fprintf(stderr,"readony access to $%x size %d =%d\n",m,newsz,status);
	}
	return;
#endif // __WIN32__

#ifdef UNIXSYSTEM
	mprotect(m,newsz,PROT_NONE);
	return;
#endif // UNIXSYSTEM

	NOT_IMPLEMENTED;
}





}; // "C"


#ifdef __WIN32__
VMMemMaster *mainmem;
#endif // ! __WIN32__
	



#ifndef __WIN32__

SYSTEMSTRUCT *systemstuff; 
#define SYSTEMSTUFF systemstuff

#endif // ! __WIN32__





#ifdef __WIN32__

extern "C"{

int i_thisisfork;

char *i_getdefaultuname(void){return SYSTEMSTUFF->defaultuname;};
char *i_getdefaultgname(void){return SYSTEMSTUFF->defaultgname;};
char *i_getemptystring(void){return SYSTEMSTUFF->emptystring;};


};

#endif // __WIN32__

extern "C" {

#ifdef USEGLOBALSTRUCT

GLOBALSTRUCT *globalstruct;

#ifdef __WIN32__
i_processflags *globalprocesssigs;
#endif //__WIN32__

static GLOBALSTRUCT *i_getglobalstruct(void){
	int newinit=0;
	GLOBALSTRUCT *g=0;
	char *name="icvmGLOBALS";
	int sz=RoundToPageSize(sizeof(GLOBALSTRUCT));
#ifdef __WIN32__
	HANDLE h;
	h=CreateFileMapping((HANDLE)0xffffffff,0,
		SEC_COMMIT | SEC_NOCACHE  | PAGE_READWRITE,
		0,sz,name);
	if(h==0)
		return 0;
	if(GetLastError()!=ERROR_ALREADY_EXISTS)
		newinit=1;
// at this point we have the mapping, now pull it into memory
	g=(GLOBALSTRUCT *)MapViewOfFile(h,FILE_MAP_ALL_ACCESS,
		0,0,sz);

#endif // __WIN32__

#ifdef UNIXSYSTEM
	key_t h=shmget(ftok(name,0),sz,0666);
	if(h<0){
		h=shmget(ftok(name,0),sz,0666 | IPC_CREAT);
		if(h<0)
			return 0;
		newinit=1;
	}
	
	g=(GLOBALSTRUCT *)shmat(h,0,0);
#endif // UNIXSYSTEM


	if(g){
		if(newinit){ // need to initialize it
			memset(g,0,sizeof(GLOBALSTRUCT));
			g->magic=GLOBALMAGICNUMBER; // MUST BE LAST
		}else{
			while(g->magic!=GLOBALMAGICNUMBER) // wait for init to complete
#ifdef __WIN32__
				Sleep(100);
#else // __WIN32__
				sleep(1);
#endif // __WIN32__
		}
	}
	return g;
	
};

s32 i_initglobalsystem(void){
	GLOBALSTRUCT *g=i_getglobalstruct();
	if(g==0)
		return -1;
#ifdef UNIXSYSTEM
	if(globalstruct)
		munmap(globalstruct,RoundToPageSize(sizeof(GLOBALSTRUCT)));	
#endif // UNIXSYSTEM

	globalstruct=g;
#ifdef __WIN32__
	globalprocesssigs=globalstruct->processsigs;
#endif // __WIN32__
	return 0;

};


#endif // USEGLOBALSTRUCT

#ifdef UNIXSYSTEM
void i_initchildfork(void){
#ifdef USEGLOBALSTRUCT
	i_initglobalsystem();
#endif // USEGLOBALSTRUCT
	SYSTEMSTUFF->InitAfterFork();
}
#endif // UNIXSYSTEM
	
s32 i_initsystem(void){
	static int systeminited;
	if(systeminited)
		return 0; // already done
	++systeminited;
	if(i_initmemsystem()==-1)
		return -1;
#ifdef USEGLOBALSTRUCT
	if(i_initglobalsystem()==-1)
		return -1;
#endif // USEGLOBALSTRUCT
	if(i_initprocess()==-1)
		return -1;
	if(i_initfilesystem()==-1)
		return -1;

#ifdef USEGLOBALSTRUCT
	globalstruct->SetReadOnly(); // set write-protected structures

#ifdef __WIN32__
	THISPROCESS->GetProcessSigs()->Reset();
	icvmattnptr=&(THISPROCESS->GetProcessSigs()->attnflag);
	*icvmattnptr=0;
//	fprintf(stderr,"ptr=$%x\n",icvmattnptr);
#endif // __WIN32__

#endif // USEGLOBALSTRUCT
	return 0;
};



// return -1 if failure, 0 if good
s32 i_initmemsystem(void){

#ifdef __WIN32__
	if(i_thisisfork){ // already running
		SYSTEMSTUFF->InitAfterFork();
		return 0;
	}
	mainmem=VMMemMaster::InitMasterMem();
	if(mainmem==0)
		return -1;
#endif // __WIN32__

	SYSTEMSTUFF=(SYSTEMSTRUCT *)i_sysalloc(sizeof(SYSTEMSTRUCT));
	if(SYSTEMSTUFF==0)
		return -1;
	SYSTEMSTUFF->Init();
	return 0;
}



#ifdef __WIN32__
s32 i_artificialfork(void){
	s32 status=mainmem->ParentFork();
	icvm_returnerr();
	return status;
};

s32 i_artificialforkchild(s32 in,s32 out){
	DWORD n;
	s32 status=VMMemMaster::ChildFork(in,out,&mainmem);
	icvm_returnerr();
	return status;
};
#endif // __WIN32__

// protected memory allocation
// NOTE - THIS MEMORY IS NEVER FREEED
// USED TO ALLOCATE WRITE-PROTECTABLE PERMANENT STRUCTURES FOR ICVM

void *i_sysalloc(u32 memsz){
#ifdef __WIN32__
	return mainmem->AllocSysPages(RoundToPageSize(memsz));
#else // __WIN32__
	void *m;
	int fd=open("/dev/zero",O_RDWR);
	if(fd>=0){
		m=mmap(0,RoundToPageSize(memsz),
			PROT_READ | PROT_WRITE,MAP_PRIVATE,fd,0);
		if(m==MAP_FAILED){
			m=0;
		}
		close(fd);
	}else{
		m=mmap(0,RoundToPageSize(memsz),
			PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANON,-1,0);
		if(m==MAP_FAILED){
			m=0;
		}

	}
	if(m==0)
		m=malloc(RoundToPageSize(memsz));
	return m;
#endif // __WIN32__
	NOT_IMPLEMENTED;

}

// write protect a block of memory
// flag definitions
//		0=read/write
//      1=read only

void i_sysprotect(void *mem,u32 memsz,s32 flag){
#ifdef __WIN32__
	mainmem->Protect(mem,RoundToPageSize(memsz),flag);

#else // __WIN32__

	switch(flag){
		case 0:
			mprotect(mem,RoundToPageSize(memsz),PROT_READ | PROT_WRITE);
			break;
		case 1:
			mprotect(mem,RoundToPageSize(memsz),PROT_READ);
			break;

	}
#endif // __WIN32__
}


// list tracked malloc to be compatible with BSD

#ifndef __WIN32__
static void **memlist=0;
#endif // __WIN32__

void *i_malloc(size_t sz){
#ifdef __WIN32__
	return mainmem->Allocate(sz);

#else // __WIN32__


	void **m;
	sz+=2*sizeof(void **);
	sz=((sz<<3)+7)>>3; // align to 8 byte boundaries
	m=(void **)malloc(sz);
	if(m){
		*m=(void *)memlist;
		memlist=m;
		m+=2;
	}
	return m;		
#endif // __WIN32__

}

void i_free(void *v){

#ifdef __WIN32__
	mainmem->Free(v);

#else // __WIN32__

	void **m=(void **)v;
	void **ml=(void **)&memlist;
	if(v==0)
		return;
	m-=2;
	while(*ml){
		if(*ml==(void *)m){ // found it - zap it off
			*ml=*m; // unlink from list
			free((void *)m); // free it
			return;
		}
		ml=(void **) *ml;
	}
#endif // __WIN32__

}



		
void *i_realloc(void *v,size_t sz){
#ifdef __WIN32__
	return mainmem->Reallocate(v,sz);
#else // __WIN32__
	void **m=(void **)v;
	void **mnew;
	void **ml=(void **)&memlist;
	if(sz==0){
		if(v==0)
			return 0;
		i_free(v);
		return 0;
	}
	if(v==0){
		return i_malloc(sz);
	}
	m-=2;
	while(*ml){
		if(*ml==(void *)m){ // found it - realloc here
			sz+=2*sizeof(void **); // new size
			sz=((sz<<3)+7)>>3; // align to 8 byte boundaries
			mnew=(void **)realloc(m,sz); // try to realloc it
			if(mnew==0){
				return 0; // failed - do not touch it from here
			}
			if(m!=(void *)mnew){ // need to replace in list			
				*ml=*mnew; // unlink old from list
				*mnew=memlist; // and place new block into list
				memlist=mnew;
			}
			mnew+=2;
			return mnew;
		}
		ml=(void **) *ml;
	}
	return 0; // if got this far, original pointer  was not malloc'ed
#endif // __WIN32__

}



// ********** TIME stuff **************


s32 i_gettimeofday(struct ICVM_timeval *tv,struct ICVM_timezone *tz){

#ifdef __WIN32__
	SYSTEMTIME st;
	FILETIME  ft;
	u64 t;
	GetLocalTime(&st);
	SystemTimeToFileTime(&st,&ft);
	t=FILETIMETONSEC(ft.dwLowDateTime,ft.dwHighDateTime);
	tv->tv_sec=NSECTOSEC(t);
	tv->tv_usec=NSECTOUSEC(t)%1000000LL;
	return 0;
#endif // __WIN32__

#ifdef UNIXSYSTEM
	struct timeval ts;
	s32 status;
	status=gettimeofday(&ts,(struct timezone *)0);
	if(status<0)
		i_settranslatederr();
	else{
		tv->tv_sec=ts.tv_sec;
		tv->tv_usec=ts.tv_usec;
	}	
	return status;
#endif // UNIXSYSTEM
	NOT_IMPLEMENTED;

}

s32 i_getpagesize(void){
	static s32 pagesz=0;
	if(pagesz==0){
#ifdef __WIN32__
	    SYSTEM_INFO s;
	    GetSystemInfo(&s);
    	pagesz = s.dwPageSize;
#endif // __WIN32__
#ifdef UNIXSYSTEM
		pagesz= getpagesize();
#endif // UNIXSYSTEM
		if(pagesz<=0)
			pagesz=4096;
	}
	return pagesz;
}



}; // "C"



