#ifndef SYSSTRUCT_H
#define SYSSTRUCT_H

#include "sys.h"
#include "file.h"
#include "id.h"
#include "proc.h"
#include "thread.h"

// Copyright (C) 2000,2001 Bob Dayley


// round memsz up to page boundary and return it
u32  RoundToPageSize(s32 memsz);

// master system struct that centralizes many things used by ICVM

typedef struct _systemstuct{
	struct filesystemstuff filestuff;
	i_threadlist threadlist;

	p32 sigreturnaddress;

#ifdef UNIXSYSTEM
	ICVM_sigset_t cursigmask;
	ICVM_sigset_t cursignals;
	ICVM_sigset_t cursigpending;
	ICVM_sigset_t cursigignore;
#endif

	struct ICVM_sigaction sigactions[32];

#ifdef USEGLOBALSTRUCT

#ifdef __WIN32__

	i_processentry *thisprocess;	
#define THISPROCESS SYSTEMSTUFF->thisprocess
	s32 defaultppid;
	s32 defaultpgrp;
#define DEFAULTPPID SYSTEMSTUFF->defaultppid
#define DEFAULTPGRP SYSTEMSTUFF->defaultpgrp
#endif // USEGLOBALSTRUCT

	char defaultuname[16];
	char defaultgname[16];
	char emptystring[4];
	struct ICVM_jmp_buf forkcontext;

#endif // __WIN32__

#ifdef __cplusplus

	inline void Init(void){
		memset(this,0,sizeof(this));
#ifdef __WIN32__
		strcpy(defaultuname,_DEFAULT_UNAME);
		strcpy(defaultgname,_DEFAULT_GNAME);
		strcpy(emptystring,"");
#endif // __WIN32__

	};

	inline void InitAfterFork(void){
#ifdef UNIXSYSTEM
		cursignals=0;
		cursigpending=0;
#endif
	};

#endif // __cplusplus

} SYSTEMSTRUCT;


#ifdef USEGLOBALSTRUCT

// this is available to all ICVM processes and must be syncronized
typedef struct _globalstruct{

	char guard1[16384]; // must be very first

// this part should be all READONLY
#define GLOBALMAGICNUMBER 0x12345678
	volatile u32	magic; // must be GLOBALMAGICNUMBER to be valid
							// if not set then table is being initted
							// and the process should wait til done, at startup


#ifdef __WIN32__
	i_processtable processtable;
#endif  // __WIN32__


	// READ/WRITE STUFF FOLLOWS HERE

	s8 guard2[16384]; // must be FIRST in read/write stuff

#ifdef __WIN32__
	i_processflagstable processsigs; // where attn flags and signals are kept

#endif  // __WIN32__

	s8 guard3[16384]; // guard pages - must be LAST in read/write stuff
#ifdef __cplusplus

// turn on readonly access only

	void SetReadOnly(void){
	// write protect first part of this structure
	// we are already assumed to be page aligned at start
		s32 pagesz=i_getpagesize();
		s32 s1=(s32)this;
		s32 s2=(s32)(guard2);
		s8 *s=(s8 *) this;
		s2-=s1;
		s+=sizeof(guard1);
		s2-=sizeof(guard1);
		i_memreadonly((void *)s,s2);

// now set up guard pages
// prevents reading and writing around the pages
// should keep crashes that send spurious signals down to a minimum
		i_memnoaccess((void *)guard1,pagesz);
		i_memnoaccess((void *)(guard2 + (pagesz-1)),pagesz);
		i_memnoaccess((void *)(guard3 + (pagesz-1)),pagesz);

	};

#endif // __cplusplus

} GLOBALSTRUCT;


extern GLOBALSTRUCT *globalstruct;

#define PROCESSTABLE (&(globalstruct->processtable))

#endif // USEGLOBALSTRUCT


#ifdef __WIN32__

extern struct VMMemMaster *mainmem;
#define SYSTEMSTUFF mainmem->systemstuff

// *** heap/block memory management

typedef struct VMMemBlock{
	struct VMMemBlock *Next; 
	s32 Size; // if less than zero, this is allocated 

#ifdef __cplusplus

// attempt to merge all free blocks together
	void PerformMerges(void){ 
		VMMemBlock *This=this;
		while(This){
			while(This->Next && This->Size>0 && 
					This->Next->Size>=0 && 
						((s8 *)This)+This->Size==(s8 *)This->Next){
				This->Size+=This->Next->Size;
				This->Next=This->Next->Next;
			}
			This=This->Next;
		}
	};

// allocate memory block with sz bytes available
// sz includes struct VMMemBlock size and alignments
	
	VMMemBlock *AllocMemBlock(s32 sz){
		VMMemBlock *This=this;
		while(This){
			if(This->Size>0 && This->Size>=sz){
				if(This->Size>=(sz+sizeof(VMMemBlock))){
					This->Size=-This->Size;
					return This;
				}
	// split the block
				s8 *s=(s8 *)This;
				VMMemBlock *n=(VMMemBlock *)(s+sz);
				n->Size=This->Size-sz;
				n->Next=This->Next;				
				This->Next=n;
				This->Size=-sz;
				return This;
			}
			This=This->Next;
		}
		return 0;
	};

// add new memory block to end of list

	void AddMemBlock(VMMemBlock *newblock,s32 sz){
		VMMemBlock *This=this;
		while(This->Next)
			This=This->Next;
		This->Next=newblock;
		newblock->Next=0;
		newblock->Size=sz;
		PerformMerges();
	};		

// free an allocated memory block

	int FreeMemBlock(VMMemBlock *block){
		VMMemBlock *This=this;
		while(This){
			if(This==block){
				if(This->Size<0)
					This->Size=-This->Size;
				PerformMerges();
				return 1;
			}
			
			This=This->Next;
		}
		return 0;
	};

#endif // __cplusplus


}VMMemBlock;




// this header is found at the beginning of a master allocation 

struct VMMemMaster{
	s32 TotalSize;
	s32 CommitedSize;
	SYSTEMSTRUCT *systemstuff; 
	struct VMMemMaster *Next; // next in list
	VMMemBlock FirstBlock; // *** MUST BE LAST ****

#ifdef __cplusplus

// initialize this master allocator.
// return 0 if successful, -1 if failure
// assumes memory is NOT enabled yet.

	int Init(s32 totalsize){
		int pagesize=i_getpagesize();
		
		while(pagesize<sizeof(VMMemMaster))
			pagesize<<=1;

		void *m=VirtualAlloc((LPVOID)this,pagesize,MEM_COMMIT,PAGE_READWRITE);
		if(m==0)
			return -1;
		if(m!=(LPVOID)this)
			return -1;
		CommitedSize=pagesize;
		TotalSize=totalsize;
		Next=0;
		FirstBlock.Next=0;
		FirstBlock.Size=CommitedSize-sizeof(VMMemMaster);
		return 0;
	};

// allocate and initialize a master memory pointer

#ifndef MEM_TOP_DOWN // missing in cygnus
#define MEM_TOP_DOWN 0x100000
#endif

	static VMMemMaster *InitMasterMem(s32 sz){
		VMMemMaster *m;
		if(sz<MASTERMEMSIZE)
			sz=MASTERMEMSIZE;
// align to masterblock size
		if(sz % MASTERMEMSIZE){
			sz-=(sz % MASTERMEMSIZE);
			sz+=MASTERMEMSIZE;
		}
		m=(VMMemMaster *)
			VirtualAlloc(0,sz,MEM_RESERVE|MEM_TOP_DOWN,PAGE_READWRITE);
		if(m==0)
			return 0;
		if(m->Init(sz)==0)
			return m;
	};

	static VMMemMaster *InitMasterMem(void){
		return InitMasterMem(MASTERMEMSIZE); 
	};


// make sure a Next exists. If not, try to allocate a new MemMaster to at least
// satisfy memsize needs

	void Need(s32 memsize){
		memsize+=(MASTERMEMSIZE>>2);
		if(Next)
			return;
		Next=InitMasterMem(memsize);
	}

// build and send a copy of this mem block to a child process
// this will recursively copy all memblocks starting here, and if
// NEXT is set, copy that ... and so forth
// return -1 on error, 0 on success
	s32	CopyToChild(HANDLE pipein,HANDLE pipeout,HANDLE childprocess){
		int cnt,cnt1;
		s8 *s,*sb;
		s32 pagesz=i_getpagesize();
		struct VMMemMaster *vmm=this;
		DWORD n;
		s32 status;

		HANDLE hnewmem;
		PVOID  newmem;		
		HANDLE duphnewmem;
		PVOID m;


		hnewmem=CreateFileMapping((HANDLE)0xffffffff,0,
			SEC_RESERVE | PAGE_READWRITE,
			0,TotalSize,0);

		if(hnewmem==0){
//fprintf(stderr,"FAILURE0=$%x %d\n",GetLastError(),GetLastError());
			i_settranslatederr();
			return -1;
		}	

		newmem=MapViewOfFile(hnewmem,FILE_MAP_ALL_ACCESS,0,0,MASTERMEMSIZE);			
		if(newmem==0){
//fprintf(stderr,"FAILURE1=$%x %d\n",GetLastError(),GetLastError());
			i_settranslatederr();
			CloseHandle(hnewmem);
			return -1;
		}	
		m=VirtualAlloc(newmem,
			CommitedSize,MEM_COMMIT,PAGE_READWRITE);

		if(newmem!=(PVOID)m){
//fprintf(stderr,"FAILURE1=$%x %d\n",GetLastError(),GetLastError());
			i_settranslatederr();
err:
			UnmapViewOfFile(newmem);
			CloseHandle(hnewmem);
			return -1;
		}

// make copy for child
		memcpy((void *)newmem,(void *)this,CommitedSize);

		if(!DuplicateHandle(
			GetCurrentProcess(),
			hnewmem,
			childprocess,
			&duphnewmem,0,0,DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE
		)){
			i_settranslatederr();
			goto err;
		}

		
// now give clues to setting this up
		WriteFile(pipeout,&vmm,sizeof(vmm),&n,0);
		WriteFile(pipeout,&TotalSize,sizeof(TotalSize),&n,0);
		WriteFile(pipeout,&CommitedSize,sizeof(CommitedSize),&n,0);
		WriteFile(pipeout,&duphnewmem,sizeof(duphnewmem),&n,0); // handle of our file

// now obtain status from other side
		status=-1;
		ReadFile(pipein,&status,sizeof(status),&n,0);
		if(status==-1){
			fprintf(stderr,"Child Process Failure\n");
			i_seterr(ICVM_ECHILD);
			goto err;
		}


// now send write protection info
		s=(s8 *)this;
		cnt=0;
		status = IsBadWritePtr(s,pagesz);
		sb=s;
		while(cnt<CommitedSize){
			if(IsBadWritePtr(s,pagesz)!=status){
				cnt1=(s32)(s-sb);
				WriteFile(pipeout,&sb,sizeof(sb),&n,0);
				WriteFile(pipeout,&cnt1,sizeof(cnt1),&n,0);
				WriteFile(pipeout,&status,sizeof(status),&n,0);
				sb=s;
				status = IsBadWritePtr(s,pagesz);
			}
			s+=pagesz;
			cnt+=pagesz;
		}						

		cnt1=(s32)(s-sb);
		if(cnt1){
			WriteFile(pipeout,&sb,sizeof(sb),&n,0);
			WriteFile(pipeout,&cnt1,sizeof(cnt1),&n,0);
			WriteFile(pipeout,&status,sizeof(status),&n,0);
		}
// write protection done status to child
		s=0;		
		WriteFile(pipeout,&s,sizeof(s),&n,0);
		UnmapViewOfFile(newmem);
		CloseHandle(hnewmem);
		if(Next) // copy next one now
			return Next->CopyToChild(pipein,pipeout,childprocess);
		else
			WriteFile(pipeout,&Next,sizeof(Next),&n,0);
		return 0;
	};



// set up a forked process and copy mem to the  process 
// and set up for a fork
// returns 0 if failure, process id if successful

	s32  ParentFork(void){
// allocate and commit memory
		s8 *s;
		char cmd[8196];
		SECURITY_ATTRIBUTES se;
		DWORD oldsz;
		PROCESS_INFORMATION pi;
		HANDLE p;
		STARTUPINFO si;
		HANDLE temppipein,temppipeout,childpipein,childpipeout;
		HANDLE pipein,pipeout;
		DWORD n;
		s32 status;

		strcpy(cmd,GetCommandLine()); // parse out command line
		s=cmd;
		while(*s && *s!=' ' && *s!='\t' && *s!='\n' && *s!='\r')
			++s;
		*s=0;
		strcat(cmd," -@@@- "); // to tell callee this is a fork
		se.nLength=sizeof(se);
		se.lpSecurityDescriptor=0;
		se.bInheritHandle=TRUE;

// create communication pipes for child to initialize it.
// open up this processes side as temps, initially,
// so we can dup this side and NOT pass them to the child

		if(!CreatePipe(&childpipein,&temppipeout,&se,0)){
//fprintf(stderr,"PIPEFAIL1\n");
			i_settranslatederr();
			return -1;
		}			

		if(!CreatePipe(&temppipein,&childpipeout,&se,0)){
//fprintf(stderr,"PIPEFAIL2\n");
			i_settranslatederr();
err4:			
			CloseHandle(temppipeout);
			goto err3;
		}			

// now dup off this sides public handles, to private and close the
// temporary private handles
		if(!DuplicateHandle(
			GetCurrentProcess(),
			temppipein,
			GetCurrentProcess(),
			&pipein,0,0,DUPLICATE_SAME_ACCESS
		)){
			i_settranslatederr();
err5:			
			CloseHandle(childpipeout);
			CloseHandle(temppipein);
			goto err4;
		}
		if(!DuplicateHandle(
			GetCurrentProcess(),
			temppipeout,
			GetCurrentProcess(),
			&pipeout,0,0,DUPLICATE_SAME_ACCESS
		)){
			i_settranslatederr();
			CloseHandle(pipein);
			goto err5;
		}
		CloseHandle(temppipein); // these do NOT get passed to child
		CloseHandle(temppipeout);

		sprintf(cmd+strlen(cmd),"%d %d",childpipein,childpipeout);


// save context and prepare a new memory copy

		i_savecontext(&systemstuff->forkcontext,1); // save current context
		systemstuff->forkcontext.r0=0; // forked process returns zero

#ifdef USEGLOBALSTRUCT
		DEFAULTPPID=i_getpid(); // fill in parent/group process stuff
		DEFAULTPGRP=i_getpgrp();
#endif	





		GetStartupInfo(&si);
		memset(&pi,0,sizeof(pi));


		if(!CreateProcess(0,cmd,&se,&se,1,
			0,0,0,&si,&pi)){
			i_settranslatederr();
err:
//			UnmapViewOfFile(newmem);
errm5:
//			CloseHandle(hnewmem);
errm4:
			CloseHandle(pipein);
			CloseHandle(childpipeout);
err2:
			CloseHandle(pipeout);
err3:
			CloseHandle(childpipein);
			return -1;
		};


		status=CopyToChild(pipein,pipeout,pi.hProcess);
		if(status>-1)
			ReadFile(pipein,&status,sizeof(status),&n,0);
		CloseHandle(pipein);
		CloseHandle(childpipeout);
		CloseHandle(childpipein);
		CloseHandle(pipeout);
		if(status==-1)
			return -1;
		return status;
	};


// build  a copy of a  block from parent process
// this will recursively copy all memblocks starting here, and if
// NEXT is set, copy that ... and so forth
// return -1 on error, 0 on success


	static s32 CopyFromParent(s32 in,s32 out,struct VMMemMaster **newvmm){
		DWORD n;
		void *m;
		s32 totalsize;
		s32 commitedsize;
		s32 status;
		s32 pagesz=i_getpagesize();
		s8 *s;
		s32 sz;
		HANDLE hnewmem;
		LPVOID newmem;
		struct VMMemMaster *vmm;

// now start reading in data
		ReadFile((HANDLE)in,&vmm,sizeof(vmm),&n,0);
		*newvmm=vmm;
		if(vmm==0)
			return 0; // done - return success
		ReadFile((HANDLE)in,&totalsize,sizeof(totalsize),&n,0);
		ReadFile((HANDLE)in,&commitedsize,sizeof(commitedsize),&n,0);
		ReadFile((HANDLE)in,&hnewmem,sizeof(hnewmem),&n,0);


// now attempt VM mapping

		m=VirtualAlloc((LPVOID)vmm,
			totalsize,MEM_RESERVE,PAGE_READWRITE);

		if(m!=(LPVOID)vmm){
//fprintf(stderr,"CHILD FAILURE1: vmm=$%x totalsize=$%x\n",vmm,totalsize);
memerr:
			i_seterr(ICVM_ENOMEM);
err:
			status=-1;
			WriteFile((HANDLE)out,&status,sizeof(status),&n,0);
			return -1;
		};

		m=VirtualAlloc((LPVOID)vmm,
			commitedsize,MEM_COMMIT,PAGE_READWRITE);

		if(m!=(LPVOID)vmm)
			goto memerr;

// now copy block of memory from parent
		newmem=MapViewOfFile(hnewmem,FILE_MAP_ALL_ACCESS,0,0,MASTERMEMSIZE);			
		if(newmem==0){
//fprintf(stderr,"CHILD FAILURE2: MEMORY ERR\n");
			goto memerr;
		}		
		memcpy((void *)vmm,newmem,commitedsize);
		UnmapViewOfFile(newmem);
		CloseHandle(hnewmem);

		status=0; // write an AOK back to parent
		WriteFile((HANDLE)out,&status,sizeof(status),&n,0);


// now set write protections
		for(;;){
			ReadFile((HANDLE)in,&s,sizeof(s),&n,0);
			if(s==0)
				break;
			ReadFile((HANDLE)in,&sz,sizeof(sz),&n,0);
			ReadFile((HANDLE)in,&status,sizeof(status),&n,0);
			if(status) // need to set readonly
				VirtualProtect(s,sz,PAGE_READONLY,&n);
		}
// recursively copy blocks from parent
		return CopyFromParent(in,out,&(vmm->Next));

	};


// child fork
// set up child fork. return -1 if failure, 0 if success and also
// the pointer at newvmm points to the VMMemMaster of the child process

	static s32 ChildFork(s32 in,s32 out,struct VMMemMaster **newvmm){
		s32 status;
		struct VMMemMaster *vmm;
		DWORD n;

// get context from parent

		if(CopyFromParent(in,out,newvmm)==-1)
			return -1;
		vmm=*newvmm;
// restore processor context
		status=i_initsystem();
		i_restorecontext(&vmm->systemstuff->forkcontext);
		if(status<0){ // start system now
			WriteFile((HANDLE)out,&status,sizeof(status),&n,0);
			return -1;
		}
		status=i_getpid();
		WriteFile((HANDLE)out,&status,sizeof(status),&n,0);
		if(status<0)
			status=-1;
		else
			status=0;		
		CloseHandle((HANDLE)in);
		CloseHandle((HANDLE)out);
		return status;
	};

		


// allocate raw pages to fit memsz
	void *AllocPages(s32 memsz){
		s8 *s;
		s32 memavailable=TotalSize-CommitedSize;
		memsz=RoundToPageSize(memsz);
		if(memsz>memavailable)
			return 0;
		s=(s8 *)this;
		s+=CommitedSize;
		LPVOID m=VirtualAlloc((LPVOID)s,memsz,MEM_COMMIT,PAGE_READWRITE);
		if(m==0)
			return 0;
		if(m!=((LPVOID)s)){
//			fprintf(stderr,"Memory Error!!!!!!!\n");
			VirtualFree(m,memsz,0);
			return 0;
		}
		CommitedSize+=memsz;
		return m;
	};

	void *AllocSysPages(s32 memsz){
		void *m;
		m=AllocPages(memsz);
		if(m)
			return m;
		Need(memsz);
		if(Next)
			return Next->AllocPages(memsz);

	};

	void AddPages(s32 memsz){
		void *m=AllocPages(memsz);
		if(m)
			FirstBlock.AddMemBlock((VMMemBlock *)m,memsz); 
	};

	void *Allocate(s32 sz){
		VMMemBlock *v;
		if(sz<0)
			return 0;
		sz+=sizeof(VMMemBlock);
		sz+=15; // 16 byte alignment
		sz&=-16;
		v=FirstBlock.AllocMemBlock(sz);
		if(v==0){
			AddPages(sz);			
			v=FirstBlock.AllocMemBlock(sz);
		}
		if(v==0){
			Need(sz);
			if(Next)
				return Next->Allocate(sz);
			return 0;
		}
		v++;
		return (void *)v;
	};

	void Free(void *m){
		VMMemBlock *v=(VMMemBlock *)m;
		--v;
		if(!FirstBlock.FreeMemBlock(v)){
			if(Next)
				Next->Free(m);
		}
	};

	void *Reallocate(void *m, s32 sz){
		VMMemBlock *v=(VMMemBlock *)m;
		void *newm;
		s32 oldsz;
		--v;
		if(m==0){
			return Allocate(sz);
		}
		if(sz==0){
			Free(m);
			return 0;
		}
		newm=Allocate(sz);
		oldsz=(-v->Size)-sizeof(VMMemBlock);
		if(newm){
			if(sz>oldsz)
				sz=oldsz;
			memcpy(newm,m,sz);
			Free(m);
		}
		return newm;
	};

// set protections - 
// flag definitions
//		0=read/write
//      1=read only
	void *Protect(void * mem,s32 memsz,s32 flag){
		memsz=RoundToPageSize(memsz);
		DWORD oldsz;
		switch(flag){
			case 0:
				VirtualProtect(mem,memsz,PAGE_READWRITE,&oldsz);
				break;
			case 1:											
				VirtualProtect(mem,memsz,PAGE_READONLY,&oldsz);
				break;
		}
	};

#endif // __cplusplus

};


#endif // __WIN32__

#ifndef __WIN32__

extern SYSTEMSTRUCT *systemstuff; 
#define SYSTEMSTUFF systemstuff

#endif // ! __WIN32__

#define FILESYSTEMSTUFF SYSTEMSTUFF->filestuff


#endif

