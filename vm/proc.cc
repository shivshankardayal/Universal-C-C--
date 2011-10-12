#include "cvm.h"

#include "sys.h"
#include "proc.h"
#include "sysstruct.h"

// Copyright (C) 2000,2001 Bob Dayley

#ifdef UNIXSYSTEM

#ifdef __svr4__
#define __EXTENSIONS__
#endif

#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include "icvmsys.h"
#include "icvmerr.h"
#include <errno.h>
#include <sys/resource.h>
#include <sys/wait.h>



#endif // UNIXSYSTEM

#ifdef __WIN32__

i_localque localque;


#endif // __WIN32__

#ifdef __cplusplus
extern "C"{
#endif

#ifdef   USEGLOBALSTRUCT

s32 i_initthisprocess(void){
#ifdef __WIN32__
	i_processentry *p;
	p=PROCESSTABLE->AllocateProcess(DEFAULTPPID,DEFAULTPGRP);
	if(p==0)
		return -1;
	THISPROCESS=p;
#endif // __WIN32__
	return 0;
};

#endif // USEGLOBALSTRUCT


s32 i_initprocess(void){
#ifdef   USEGLOBALSTRUCT
#ifdef 	 __WIN32__
	if(i_initthisprocess()<0)
		return -1;
#endif //	 __WIN32__
#endif // USEGLOBALSTRUCT
	return 0;
}




// ************************************
s32 i_getpid(void){

#ifdef __WIN32__
	return THISPROCESS->GetPID();
#endif // __WIN32__

#ifndef __WIN32__
	return getpid();
#endif // !__WIN32__
	NOT_IMPLEMENTED;
	return 0;
};
s32 i_getppid(void){
#ifdef __WIN32__
	return THISPROCESS->GetPPID();
#endif // __WIN32__
#ifndef __WIN32__
	return getppid();
#endif // !__WIN32__
	NOT_IMPLEMENTED;
	return 0;
};
s32 i_getpgrp(void){
#ifdef __WIN32__
	return THISPROCESS->GetPGRP();
#endif // __WIN32__
#ifndef __WIN32__
	return getpgrp();
#endif // !__WIN32__
	NOT_IMPLEMENTED;
	return 0;
};


#ifdef __WIN32__																

// get current signals for Windoze
// returns signals that are not masked
s32 i_getcurrentsignals(void){
	return THISPROCESS->GetProcessSigs()->GetSignals();
};

#endif // __WIN32__




s32 i_kill(s32 pid,s32 sig){

#ifdef 	 UNIXSYSTEM
	s32 status=kill(pid,TranslateSigFromICVM(sig));
	i_settranslatederr();
	return status;
#endif // UNIXSYSTEM


#ifdef 	 __WIN32__
	return PROCESSTABLE->kill(pid,sig);
#endif // __WIN32__

	NOT_IMPLEMENTED;
	return 0;
	
};

s32 i_sigprocmask(s32 how,ICVM_sigset_t *get,ICVM_sigset_t *old){

#ifdef __WIN32__
	ICVM_sigset_t told;
	if(get==0)
		told=THISPROCESS->GetProcessSigs()->GetSignalMask();
	else{	
		switch(how){
			case ICVM_SIG_BLOCK:
				told=THISPROCESS->GetProcessSigs()->BlockSignals(*get);
				break;

			case ICVM_SIG_UNBLOCK:
				told=THISPROCESS->GetProcessSigs()->UnBlockSignals(*get);
				break;

			case ICVM_SIG_SETMASK:
				told=THISPROCESS->GetProcessSigs()->SetSignalMask(*get);
				break;
			
			default:
				i_seterr(ICVM_EINVAL);
				return -1;
		}
	}
	if(old)
		*old=told;
	return 0;		

#endif __WIN32__

#ifdef UNIXSYSTEM
	sigset_t tget,told;
	s32 status;
	if(get)
		TranslateSigSetFromICVM(*get,&tget);
	else	
		TranslateSigSetFromICVM(0,&tget);
	switch(how){
		case ICVM_SIG_BLOCK:
			how=SIG_BLOCK;
			break;

		case ICVM_SIG_UNBLOCK:
			how=SIG_UNBLOCK;
			break;

		case ICVM_SIG_SETMASK:
			how=SIG_SETMASK;
			break;
			
		default:
			i_seterr(ICVM_EINVAL);
			return -1;
		}
		status=sigprocmask(how,&tget,&told);
		if(old)
			*old=TranslateSigSetToICVM(&told);
		i_settranslatederr();
		return status;
#endif UNIXSYSTEM

	NOT_IMPLEMENTED
	return -1;
};


s32 i_sigpending(ICVM_sigset_t *s){
#ifdef __WIN32__
	*s=THISPROCESS->GetProcessSigs()->GetSigPending();
	return 0;
#endif // __WIN32__

#ifdef UNIXSYSTEM
	sigset_t syst;
	s32 status=sigpending(&syst);
	*s=TranslateSigSetToICVM(&syst);
	i_settranslatederr();
	return status;
#endif // UNIXSYSTEM
	NOT_IMPLEMENTED;
};


s32 i_nanosleep(struct ICVM_timespec *req,struct ICVM_timespec *rem){

#ifdef UNIXSYSTEM
	s32 status;
	struct timespec sreq,srem;
	sreq.tv_sec=req->tv_sec;
	sreq.tv_nsec=req->tv_nsec;
	status=nanosleep(&sreq,&srem);
	if(rem){
		rem->tv_sec=srem.tv_sec;
		rem->tv_nsec=srem.tv_nsec;
	}
	i_settranslatederr();
	return status;
#endif // UNIXSYSTEM

#ifdef __WIN32__
	s32 status=THISPROCESS->SleepMS(req->tv_sec*1000+(req->tv_nsec/1000000));
	if(rem){
		rem->tv_sec=status/1000;
		rem->tv_nsec=(status % 1000) * 1000000;
	}
	if(status){
		i_seterr(ICVM_EINTR);
		return -1;
	}
	return 0;
#endif // WIN32
	NOT_IMPLEMENTED;
};


void i_savecontext(struct ICVM_jmp_buf *b,int savemask){
#ifdef __WIN32__
	ICVM_sigset_t mask;
#endif //__WIN32__
#ifdef UNIXSYSTEM
	sigset_t mask;
#endif //__UNIXSYSTEM__
	b->r0=R_R0;
	b->r1=R_R1;
	b->r2=R_R2;
	b->r3=R_R3;
// just straight byte copies just in case need alignment
	memcpy(&b->q0,&R_Q0,sizeof(R_Q0));
	memcpy(&b->q1,&R_Q1,sizeof(R_Q1));
	memcpy(&b->fp0,&R_FP0,sizeof(R_FP0));
	memcpy(&b->fp1,&R_FP1,sizeof(R_FP1));
	memcpy(&b->fp2,&R_FP2,sizeof(R_FP2));
	b->fp=R_FP;
	b->sp=R_SP;
	b->pc=R_PC;
	b->lastpc=R_LASTPC;
	b->lastsp=R_LASTSP;
#ifdef __WIN32__
	i_sigprocmask(ICVM_SIG_SETMASK,0,&mask);
#endif // __WIN32__
#ifdef UNIXSYSTEM
	sigprocmask(SIG_SETMASK,0,&mask);
#endif // UNIXSYSTEM
	b->cursigmask=mask;
	b->sigmaskwassaved=savemask;
};

																									
void i_restorecontext(struct ICVM_jmp_buf *b){
#ifdef __WIN32__
	ICVM_sigset_t mask;
#endif //__WIN32__
#ifdef UNIXSYSTEM
	sigset_t mask;
#endif //__UNIXSYSTEM__
	R_FP=b->fp; // THIS MUST BE VERY FIRST
	R_R0=b->r0;
	R_R1=b->r1;
	R_R2=b->r2;
	R_R3=b->r3;
	memcpy(&R_Q0,&b->q0,sizeof(R_Q0));
	memcpy(&R_Q1,&b->q1,sizeof(R_Q1));
	memcpy(&R_FP0,&b->fp0,sizeof(R_FP0));
	memcpy(&R_FP1,&b->fp1,sizeof(R_FP1));
	memcpy(&R_FP2,&b->fp2,sizeof(R_FP2));
	R_SP=b->sp;
	R_PC=b->pc;
	R_LASTPC=b->lastpc;
	R_LASTSP=b->lastsp;
	if(b->sigmaskwassaved){
		mask=b->cursigmask;
#ifdef __WIN32__
		i_sigprocmask(ICVM_SIG_SETMASK,&mask,0);
#endif // __WIN32__
#ifdef UNIXSYSTEM
		sigprocmask(SIG_SETMASK,&mask,0);
#endif // UNIXSYSTEM_
	}
};





#ifdef UNIXSYSTEM				

// translate signal from ICVM set to local machine - return -1 if not recogized
s32 TranslateSigFromICVM(s32 insig){
	int newsig=-1;
	switch(insig){
		case	ICVM_SIGHUP:newsig=SIGHUP;break;
		case	ICVM_SIGINT:newsig=SIGINT;break;
		case	ICVM_SIGQUIT:newsig=SIGQUIT;break;
		case	ICVM_SIGILL:newsig=SIGILL;break;
		case	ICVM_SIGABRT:newsig=SIGABRT;break;
		case	ICVM_SIGFPE:newsig=SIGFPE;break;
		case	ICVM_SIGKILL:newsig=SIGKILL;break;
		case	ICVM_SIGSEGV:newsig=SIGSEGV;break;
		case	ICVM_SIGPIPE:newsig=SIGPIPE;break;
		case	ICVM_SIGALRM:newsig=SIGALRM;break;
		case	ICVM_SIGTERM:newsig=SIGTERM;break;
		case	ICVM_SIGSTOP:newsig=SIGSTOP;break;
		case	ICVM_SIGTSTP:newsig=SIGTSTP;break;
		case	ICVM_SIGCONT:newsig=SIGCONT;break;
		case	ICVM_SIGCHLD:newsig=SIGCHLD;break;
		case	ICVM_SIGTTIN:newsig=SIGTTIN;break;
		case	ICVM_SIGTTOU:newsig=SIGTTOU;break;
		case 	ICVM_SIGUSR1:newsig=SIGUSR1;break;
		case 	ICVM_SIGUSR2:newsig=SIGUSR2;break;

	}
	return newsig;
};


// translate signal to ICVM set from local machine - return -1 if not recogized
s32 TranslateSigToICVM(s32 insig){
	int newsig=-1;
	switch(insig){
		case	SIGHUP:newsig=ICVM_SIGHUP;break;
		case	SIGINT:newsig=ICVM_SIGINT;break;
		case	SIGQUIT:newsig=ICVM_SIGQUIT;break;
		case	SIGILL:newsig=ICVM_SIGILL;break;
		case	SIGABRT:newsig=ICVM_SIGABRT;break;
		case	SIGFPE:newsig=ICVM_SIGFPE;break;
		case	SIGKILL:newsig=ICVM_SIGKILL;break;
		case	SIGSEGV:newsig=ICVM_SIGSEGV;break;
		case	SIGPIPE:newsig=ICVM_SIGPIPE;break;
		case	SIGALRM:newsig=ICVM_SIGALRM;break;
		case	SIGTERM:newsig=ICVM_SIGTERM;break;
		case	SIGSTOP:newsig=ICVM_SIGSTOP;break;
		case	SIGTSTP:newsig=ICVM_SIGTSTP;break;
		case	SIGCONT:newsig=ICVM_SIGCONT;break;
		case	SIGCHLD:newsig=ICVM_SIGCHLD;break;
		case	SIGTTIN:newsig=ICVM_SIGTTIN;break;
		case	SIGTTOU:newsig=ICVM_SIGTTOU;break;
		case 	SIGUSR1:newsig=ICVM_SIGUSR1;break;
		case 	SIGUSR2:newsig=ICVM_SIGUSR2;break;

	}
	return newsig;
};
				
// translate signal set  from ICVM set to local machine - return -1 if not recogized
void TranslateSigSetFromICVM(ICVM_sigset_t insig,sigset_t *sigs){
	sigemptyset(sigs); // remove all signals
	if(insig & (1<<ICVM_SIGHUP))sigaddset(sigs,SIGHUP);
	if(insig & (1<<ICVM_SIGINT))sigaddset(sigs,SIGINT);
	if(insig & (1<<ICVM_SIGQUIT))sigaddset(sigs,SIGQUIT);
	if(insig & (1<<ICVM_SIGILL))sigaddset(sigs,SIGILL);
	if(insig & (1<<ICVM_SIGABRT))sigaddset(sigs,SIGABRT);
	if(insig & (1<<ICVM_SIGFPE))sigaddset(sigs,SIGFPE);
	if(insig & (1<<ICVM_SIGKILL))sigaddset(sigs,SIGKILL);
	if(insig & (1<<ICVM_SIGSEGV))sigaddset(sigs,SIGSEGV);
	if(insig & (1<<ICVM_SIGPIPE))sigaddset(sigs,SIGPIPE);
	if(insig & (1<<ICVM_SIGALRM))sigaddset(sigs,SIGALRM);
	if(insig & (1<<ICVM_SIGTERM))sigaddset(sigs,SIGTERM);
	if(insig & (1<<ICVM_SIGSTOP))sigaddset(sigs,SIGSTOP);
	if(insig & (1<<ICVM_SIGTSTP))sigaddset(sigs,SIGTSTP);
	if(insig & (1<<ICVM_SIGCONT))sigaddset(sigs,SIGCONT);
	if(insig & (1<<ICVM_SIGCHLD))sigaddset(sigs,SIGCHLD);
	if(insig & (1<<ICVM_SIGTTIN))sigaddset(sigs,SIGTTIN);
	if(insig & (1<<ICVM_SIGTTOU))sigaddset(sigs,SIGTTOU);
	if(insig & (1<<ICVM_SIGUSR1))sigaddset(sigs,SIGUSR1);
	if(insig & (1<<ICVM_SIGUSR2))sigaddset(sigs,SIGUSR2);

};

// translate signal set to ICVM set from local machine
ICVM_sigset_t TranslateSigSetToICVM(sigset_t *sigs){
	ICVM_sigset_t newsig=0;
	if(sigismember(sigs,SIGHUP))newsig|=(1<<ICVM_SIGHUP);
	if(sigismember(sigs,SIGINT))newsig|=(1<<ICVM_SIGINT);
	if(sigismember(sigs,SIGQUIT))newsig|=(1<<ICVM_SIGQUIT);
	if(sigismember(sigs,SIGILL))newsig|=(1<<ICVM_SIGILL);
	if(sigismember(sigs,SIGABRT))newsig|=(1<<ICVM_SIGABRT);
	if(sigismember(sigs,SIGFPE))newsig|=(1<<ICVM_SIGFPE);
	if(sigismember(sigs,SIGKILL))newsig|=(1<<ICVM_SIGKILL);
	if(sigismember(sigs,SIGSEGV))newsig|=(1<<ICVM_SIGSEGV);
	if(sigismember(sigs,SIGPIPE))newsig|=(1<<ICVM_SIGPIPE);
	if(sigismember(sigs,SIGALRM))newsig|=(1<<ICVM_SIGALRM);
	if(sigismember(sigs,SIGTERM))newsig|=(1<<ICVM_SIGTERM);
	if(sigismember(sigs,SIGSTOP))newsig|=(1<<ICVM_SIGSTOP);
	if(sigismember(sigs,SIGTSTP))newsig|=(1<<ICVM_SIGTSTP);
	if(sigismember(sigs,SIGCONT))newsig|=(1<<ICVM_SIGCONT);
	if(sigismember(sigs,SIGCHLD))newsig|=(1<<ICVM_SIGCHLD);
	if(sigismember(sigs,SIGTTIN))newsig|=(1<<ICVM_SIGTTIN);
	if(sigismember(sigs,SIGTTOU))newsig|=(1<<ICVM_SIGTTOU);
	if(sigismember(sigs,SIGUSR1))newsig|=(1<<ICVM_SIGUSR1);
	if(sigismember(sigs,SIGUSR2))newsig|=(1<<ICVM_SIGUSR2);
	return newsig;
};
				
#endif // UNIXSYSTEM


#ifdef __cplusplus
};
#endif


