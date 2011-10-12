#ifndef PROC_H
#define PROC_H 1

// Copyright (C) 2000,2001 Bob Dayley


#ifdef __WIN32__
#include <windows.h>
#endif // __WIN32__

#ifdef UNIXSYSTEM
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#endif // UNIXSYSTEM

#include "icvmerr.h"

typedef u32 ICVM_sigset_t;

// icvm signals

#define	ICVM_SIGHUP	1	/* hangup */
#define	ICVM_SIGINT	2	/* interrupt */
#define	ICVM_SIGQUIT	3	/* quit */
#define	ICVM_SIGILL	4	/* illegal instruction (not reset when caught) */
#define	ICVM_SIGABRT	6	/* abort() */
#define	ICVM_SIGFPE	8	/* floating point exception */
#define	ICVM_SIGKILL	9	/* kill (cannot be caught or ignored) */
#define	ICVM_SIGSEGV	11	/* segmentation violation */
#define	ICVM_SIGPIPE	13	/* write on a pipe with no one to read it */
#define	ICVM_SIGALRM	14	/* alarm clock */
#define	ICVM_SIGTERM	15	/* software termination signal from kill */
#define	ICVM_SIGSTOP	17	/* sendable stop signal not from tty */
#define	ICVM_SIGTSTP	18	/* stop signal from tty */
#define	ICVM_SIGCONT	19	/* continue a stopped process */
#define	ICVM_SIGCHLD	20	/* to parent on child stop or exit */
#define	ICVM_SIGTTIN	21	/* to readers pgrp upon background tty read */
#define	ICVM_SIGTTOU	22	/* like TTIN for output if (tp->t_local&LTOSTOP) */
#define ICVM_SIGUSR1 30	/* user defined signal 1 */
#define ICVM_SIGUSR2 31	/* user defined signal 2 */


struct	ICVM_sigaction {
	p32 handler;	/* signal handler */
	ICVM_sigset_t mask;		/* signal mask to apply */
	s32	flags;		/* see signal options below */
};


// icvm signal definitions

#define ICVM_SA_NOCLDSTOP	0x0004	/* do not generate SIGCHLD on child stop */
#define ICVM_SA_RESTART		0x0002	/* do not restart system on signal return */


#define	ICVM_SIG_DFL		0
#define	ICVM_SIG_IGN		1

#define	ICVM_SIG_BLOCK		1	/* block specified signal set */
#define	ICVM_SIG_UNBLOCK	2	/* unblock specified signal set */
#define	ICVM_SIG_SETMASK	3	/* set specified signal set */



struct ICVM_jmp_buf{
	s32 r0;
	s32 r1;
	s32 r2;
	s32 r3;
	s64 q0;
	s64 q1;
	f64 fp0;
	f64 fp1;
	f64 fp2;
	p32 fp;
	p32 sp;
	p32 pc;
	p32 lastpc;
	p32 lastsp;
#ifdef __WIN32__
	ICVM_sigset_t cursigmask;
//	ICVM_sigset_t sigmask;
#endif // __WIN32__
#ifdef UNIXSYSTEM
	sigset_t cursigmask;
//	sigset_t sigmask;
#endif // UNIXSYSTEM
	s32 sigmaskwassaved;
};



#ifdef __cplusplus
extern "C"{
#endif // __cplusplus



void i_savecontext(struct ICVM_jmp_buf *b,int savemask);
void i_restorecontext(struct ICVM_jmp_buf *b);
s32 i_initprocess(void);
s32 i_initthisprocess(void);
s32 i_getpid(void);
s32 i_getppid(void);
s32 i_getpgrp(void);
s32 i_kill(s32 pid,s32 sig);
s32 i_nanosleep(struct ICVM_timespec *req,struct ICVM_timespec *rem);
s32 i_sigprocmask(s32 how,ICVM_sigset_t *get,ICVM_sigset_t *old);
s32 i_sigpending(ICVM_sigset_t *s);

#ifdef __WIN32__
s32 i_getcurrentsignals(void);
#endif // __WIN32__


#ifdef UNIXSYSTEM

s32 TranslateSigFromICVM(s32 insig);
s32 TranslateSigToICVM(s32 insig);
void TranslateSigSetFromICVM(ICVM_sigset_t insig,sigset_t *sigs);
ICVM_sigset_t TranslateSigSetToICVM(sigset_t *sigs);

#endif // UNIXSYSTEM


#ifdef __cplusplus
};
#endif // __cplusplus



#ifdef USEGLOBALSTRUCT

#define ICVM_MAXPROCESS	4096


#ifdef __WIN32__

// contains externally modifiable items

typedef struct i_processflags{
	ICVMATTNTYPE attnflag;
	ICVMATTNTYPE cursignals;
	ICVMATTNTYPE cursigignore;
	ICVMATTNTYPE cursigmask;
	ICVMATTNTYPE cursigpending;

#ifdef __cplusplus
	friend struct i_processentry;

// set signals and set attn.

private:

// return 1 if a handled signal is delivered, else return 0
	inline s32 SetSignals(s32 sigs){
		s32 sigpend;
		sigs |= (s32)cursigpending;
		sigs &= ~((s32)cursigignore);
		sigpend = (s32)cursigmask & sigs;
		sigs &= ~sigpend;
		if(sigpend){
			SyncAndValue(&cursignals,~sigpend);
			SyncOrValue(&cursigpending,sigpend);
		}
		if(sigs){
			SyncOrValue(&cursignals,sigs);
			SyncAndValue(&cursigpending,~sigs);
			SyncOrValue(&attnflag,ICVM_SIGNAL);
			return 1;
		}
		return 0;
	};

public:

	inline s32 BlockSignals(s32 mask){
		s32 old=(s32)cursigmask;
		s32 newpend;
		SyncOrValue(&cursigmask,mask);
		newpend=(s32)cursignals & mask;
		if(newpend){
			SyncOrValue(&cursigpending,newpend);
			SyncAndValue(&cursignals,~newpend);
		}
		return old;	
	};

	inline s32 UnBlockSignals(s32 mask){
		s32 old=(s32)cursigmask;
		s32 newsig;
		SyncAndValue(&cursigmask,~mask);
		newsig=(s32)cursigpending & ~mask;
		if(newsig){
			SyncOrValue(&cursignals,newsig);
			SyncAndValue(&cursigpending,~newsig);
			SyncOrValue(&attnflag,ICVM_SIGNAL);
		}
		return old;	
	};

	inline s32 SetSignalMask(s32 mask){
		s32 old=(s32)cursigmask;
		BlockSignals(mask);
		UnBlockSignals(~mask);
		return old;	

	};

	inline s32 GetSignalMask(void){return (s32)cursigmask;};

	inline s32 GetSigPending(void){return (s32)cursigpending;};

	inline s32 GetSignals(void){
		s32 sigs=(s32)cursignals;
		if(sigs)
			SyncAndValue(&cursignals,~sigs);
		return sigs;	
	};


	inline void Reset(void){
		attnflag=0;
		cursignals=0;
		cursigignore=0;
		cursigmask=0;
		cursigpending=0;
	};


#endif //__cplusplus

} i_processflags;	

typedef i_processflags i_processflagstable[ICVM_MAXPROCESS];

// local event structure to speed signaling etc.

typedef struct i_localhandleque{
#ifdef __cplusplus
private:
#endif //__cplusplus

	s32 pid; // pid if process that this refers to
	HANDLE wakeupevent; // local handle access to the wakeup event

#ifdef __cplusplus

public:

// send a wakeup call to the PID. Que the handle if this is first time
// return 0 if successful, -1 if not 

	s32 WakeUpProcess(s32 ipid){
		if(pid!=pid || wakeupevent==0){
			char buf[512];
			HANDLE h;
			if(wakeupevent)
				CloseHandle(wakeupevent); // process is obsolete
			wakeupevent=0;
			sprintf(buf,"icvmEVENT%d",ipid);
			h=OpenEvent(EVENT_MODIFY_STATE,0,buf);
			if(!h){
				i_settranslatederr();
				return -1;
			}
			pid=ipid;
			wakeupevent=h;
		}
		if(!SetEvent(wakeupevent)){ // set the wakeup
			i_settranslatederr();
			return -1;
		}
		return 0;
	};
#endif //__cplusplus
}i_localhandleque;

typedef struct i_localhandleque i_localque[ICVM_MAXPROCESS];

extern i_localque localque;

extern i_processflags *globalprocesssigs;

typedef struct i_processentry{
#ifdef __cplusplus
	private:
#endif //__cplusplus

	volatile s32   pid; // this process id
	volatile s32   index; // this is the array index of the table we sit
	volatile s32   ppid;  // parent process ID
	volatile s32   pgrp;  // process groupe

	volatile PVOID allocated;
	volatile DWORD wpid; // windows process ID
	volatile PVOID  signals; // signals for this process
	volatile PVOID attnflag; // attnflag for main loop
	volatile HANDLE wakeupevent; // wakeup event generator
								// NOTE - this handle is only valid for
								// the process that owns it.
								// YOU MUST DUPLICATE IT for other processes

#ifdef __cplusplus
public:
	inline s32 GetPID(void){return pid;};
	inline s32 GetPPID(void){return ppid;};
	inline s32 GetPGRP(void){return pgrp;};

	inline void Free(void){allocated=0;};
	inline s32 IsFree(void){return allocated==0;};
	inline s32 IsValid(void){return (s32)allocated==-1;};
	inline void Validate(void){
		allocated=(PVOID)-1;
	};
	inline void Init(s32 ipid,s32 ippid,s32 ipgrp,s32 iindex){
		char buf[512];
		ppid=ippid;
		pgrp=ipgrp;
		pid=ipid;
		index=iindex;
		wpid=GetCurrentProcessId();
		attnflag=0;
		signals=0;
		sprintf(buf,"icvmEVENT%d",ipid);
		wakeupevent=CreateEvent(0,1,0,buf);
		Validate(); // must happen last
	};

	inline s32 Allocate(void){
		return TrySyncSetValue(&allocated,1,0);
	};


	inline i_processflags *GetProcessSigs(void){
		return globalprocesssigs+index;
	};

// wait for something to happen by waiting on the local wakekup event
// resets the event when awakened, but does not check or modify anything else
// THIS FUNCTION IS ONLY VALID IN A LOCAL CONTEXT

	inline void WaitForSomethingToHappen(DWORD ms){
		RELEASECPU; // let other threads run
		WaitForSingleObject(wakeupevent,ms); // go to sleep, maybe
		ResetEvent(wakeupevent); // reset the wakeup
		HOLDCPU; // inhibit other threads
	};

	inline void WaitForSomethingToHappen(void){
		WaitForSomethingToHappen(INFINITE);
	};

// set a series of signals
	inline s32 SetSignals(s32 sigs){
// set up the signal
		if(sigs){
			if(GetProcessSigs()->SetSignals(sigs)==0)
				return 1; // probably ignored or blocked
		}
		if(i_getpid()==GetPID()){ // manual trigger this from here
			SetEvent(wakeupevent); // wake up this process
			return 0;
		}
		return localque[index].WakeUpProcess(GetPID());
	};


// sleep for a number of MS. Wakeup on any event and return time in MS that lapsed
// return -1 on error
	inline s32 SleepMS(s32 ms){
		s32 sleeptime;
		s32 tdiff;
		if(ms==0){
			Sleep(0);
			return 0;
		}
		sleeptime=GetTickCount();
		WaitForSomethingToHappen(ms);
		tdiff=GetTickCount()-sleeptime;
		tdiff=ms-tdiff;
		if(tdiff<0)
			tdiff=0;
		return tdiff;
	};
		

#endif //__cplusplus


}i_processentry;

typedef struct i_processtable{
	i_processentry entries[ICVM_MAXPROCESS];	
	s32  nextprocessid;

#ifdef __cplusplus

	// get a new process id
	
	inline s32 NewProcessID(void){

		s32 tid=0;
		int cnt;
		while(tid<2){
			tid=iInterlockedExchangeAdd((PLONG)&nextprocessid,1) & 0xfffffff;
			if(tid){
				cnt=0;		
				while(cnt<ICVM_MAXPROCESS){
					if(!entries[cnt].IsFree() && 
							entries[cnt].GetPID()==tid){
						tid=0;
						break;
					}
					++cnt;
				}
			}
		}
		return tid;
	};

	// allocate and initialize  process entry 
	// - return process ptr if found, or 0 if none available
	inline  i_processentry *AllocateProcess(s32 ippid,s32 ipgrp){
		int cnt=0; 
		while(cnt<ICVM_MAXPROCESS){
			if(entries[cnt].IsFree() && entries[cnt].Allocate()){
				entries[cnt].Init(NewProcessID(),ippid,ipgrp,cnt);
				return &entries[cnt];
			}
			++cnt;
		}
		return 0;
	};


	// return a pointer to process entry based on id
	// return 0 if error
	inline i_processentry *GetProcessPtr(s32 id){
		int cnt=0; 
		while(cnt<ICVM_MAXPROCESS){
			if(entries[cnt].IsValid() && entries[cnt].GetPID()==id)
				return &entries[cnt];
			++cnt;
		}
		return 0;
	};

	inline i_processflags *GetProcessSigs(s32 id){
		i_processentry *e=GetProcessPtr(id);
		if(e)
			return e->GetProcessSigs();
		return 0;
	};


// unix KILL emulation
	inline s32 kill(s32 pid,s32 sig){
		int cnt=ICVM_MAXPROCESS-1; 
		i_processentry *p,*pg;
		s32 grp=i_getpgrp();
		if(pid>0){
			p=GetProcessPtr(pid);
			if(p==0){
				i_seterr(ICVM_ESRCH);
				return -1;
			}
			return p->SetSignals(1<<sig);
		}	
		if(pid!=-1 && pid<0){
			p=GetProcessPtr(-pid);
			if(p==0){
				i_seterr(ICVM_ESRCH);
				return -1;
			}
			grp=p->GetPGRP();
		}
		while(cnt>=0){
			p=&entries[cnt];
			if(p->IsValid()){
				if(pid<=0){
					if(pid==-1 || (pid<-1 && p->GetPGRP()==grp))
						p->SetSignals(1<<sig);
				}
			}
			--cnt;
		}
		return 0;
	};					


#endif //__cplusplus

}i_processtable;

#endif // __WIN32__

#endif // USEGLOBALSTRUCT



#endif // !PROC_H




