#include "cvm.h"

#include "sys.h"
#include "proc.h"
#include "sysstruct.h"

// Copyright (C) 2000,2001 Bob Dayley


#ifndef __WIN32__

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



#endif // __WIN32__

void ICVM_getpid(void){R_R0=i_getpid();};
void ICVM_getppid(void){R_R0=i_getppid();};
void ICVM_getpgrp(void){R_R0=i_getpgrp();};

// icvm timespec

#define ICVM_tv_sec	0
#define ICVM_tv_nsec	4

void ICVM_nanosleep(void){
	p32 ireq;
	p32 irem;
	struct ICVM_timespec treq;
	struct ICVM_timespec trem;

	ireq=rSTKs32(0);
	irem=rSTKs32(1);
	treq.tv_sec=RDu32(ireq+ICVM_tv_sec,0);
	treq.tv_nsec=RDu32(ireq+ICVM_tv_nsec,0);
	R_R0=i_nanosleep(&treq,&trem);
	if(irem){
		WDs32(irem+ICVM_tv_sec,trem.tv_sec,0);
		WDs32(irem+ICVM_tv_nsec,trem.tv_nsec,0);
	}
	icvm_returnerr();
}	
	


void ICVM_sigsetjmp(void){
	p32 b=rSTKs32(0);
	int savemask=rSTKs32(1);
	i_savecontext((struct ICVM_jmp_buf *)Nptr(b),savemask);
	R_R0=0;
}



void ICVM_siglongjmp(void){
	p32 b=rSTKs32(0);
	s32 ret=rSTKs32(1); // return val
	i_restorecontext((struct ICVM_jmp_buf *)Nptr(b));
	if(ret==0)
		ret=1;
	R_R0=ret;
}

// return from signal, restore context that was  saved for signal

void ICVM_sigreturn(void){
	ICVM_sigset_t smask;
	R_SP+=8; // dump argument
	i_restorecontext((struct ICVM_jmp_buf *)R_SP);
	R_SP+=(sizeof(struct ICVM_jmp_buf)+7) & -8;
};

// need to set signal return address to somewhere in code
void ICVM_setsigreturn(void){
	SYSTEMSTUFF->sigreturnaddress=(p32)rSTKs32(0);
};


void ICVM_sigpending(void){
	p32 s=rSTKs32(0);
	ICVM_sigset_t sigs;
	if(s==0){
		i_seterr(ICVM_EFAULT);
		icvm_returnerr();
	}
	R_R0=i_sigpending(&sigs);
	WDs32(s,sigs,0);		
};


// save context to stack, including a return to sigreturn which will
// restore the context later when it hits an rts or rtsframe


static void dosignal(p32 newaddress,u32 sig,ICVM_sigset_t sigmask){
	R_SP-=(sizeof(struct ICVM_jmp_buf)+7)&-8;
	i_savecontext((struct ICVM_jmp_buf *)R_SP,1);
	sigmask |=(1<<sig);
	i_sigprocmask(ICVM_SIG_BLOCK,&sigmask,0);
	R_SP-=16;
	WDu32(R_SP,SYSTEMSTUFF->sigreturnaddress,0); // so we can return here
	WDu32(R_SP+8,sig,0); // argument for handler
	R_PC=newaddress; // jump to new address
}



void HandleSignals(void){
	struct ICVM_sigaction *sa;
	int cnt=0;
	s32 signals;
	ICVM_CLEARATTN(ICVM_SIGNAL);

#ifdef __WIN32__
	signals=i_getcurrentsignals(); // retrieve current signals
#endif // __WIN32__

#ifdef UNIXSYSTEM
	signals=SYSTEMSTUFF->cursignals;
	SYSTEMSTUFF->cursignals &=~signals;
#endif // UNIXSYSTEM

	while(cnt<32){
		if(signals & (1<<cnt)){
			sa=&(SYSTEMSTUFF->sigactions[cnt]);
// default handler
			if(sa->handler==ICVM_SIG_DFL){
				switch(cnt){

// SIGCHLD needs windows handler
					case	ICVM_SIGCHLD:
					case	ICVM_SIGCONT:
//						SYSTEMSTUFF->cursignals&=~(1<<cnt);
						break;

					case	ICVM_SIGPIPE:
					case	ICVM_SIGTTIN:
					case	ICVM_SIGTTOU:	
					case	ICVM_SIGALRM:
					case	ICVM_SIGHUP:
					case	ICVM_SIGINT:
					case	ICVM_SIGQUIT:
					case	ICVM_SIGILL:
					case	ICVM_SIGSEGV:
					case	ICVM_SIGABRT:
					case	ICVM_SIGFPE:
					case	ICVM_SIGKILL:
					case	ICVM_SIGTERM:
					case	ICVM_SIGSTOP:
					case	ICVM_SIGTSTP:
					case	ICVM_SIGUSR1:
					case	ICVM_SIGUSR2:
					default:
						R_R0=0;
						ICVM_SETBREAK(ICVM_EXIT);
						return;
				}
			}else if(sa->handler!=ICVM_SIG_IGN) 
				dosignal(sa->handler,cnt,sa->mask); 
		}
		++cnt;
	}
}
// handle attn signals for ICVM
void HandleAttn(void){
#ifdef NOT_NOW // disable for now
		
		if(ICVMATTN & ICVM_DEBUG){ // debug needs attention
			ICVM_debug();
		}

#endif

		if(ICVMATTN & ICVM_SIGNAL){ // signal is set
			HandleSignals();
		}

};


#ifdef UNIXSYSTEM																


static void sighandler(int signum){
	int sig=TranslateSigToICVM(signum);
	s32 sigmask; 
	if(sig==-1){
		return; // *********** BAD SIGNAL SHOULD NOT HAPPEN
	}
	SYSTEMSTUFF->cursignals |= sigmask; 
	ICVM_SETATTN(ICVM_SIGNAL); // notify cpu that signal is waiting 

};

#endif // UNIXSYSTEM


// icvm sigaction offsets

#define ICVM_sa_handler		0
#define ICVM_sa_mask		4
#define ICVM_sa_flags		8



void ICVM_sigaction(void){
	int icvmsignum=rSTKs32(0);
	p32 act=rSTKs32(1);
	p32 oldact=rSTKs32(2);

#ifdef UNIXSYSTEM
	struct sigaction syssigaction,oa;
	int nativesignum=TranslateSigFromICVM(icvmsignum);
#endif // UNIXSYSTEM

	int flags=0;
	struct ICVM_sigaction *sa;
// see if this is a legal signal first
	if( 
#ifdef UNIXSYSTEM
			nativesignum<0 || 
#endif // UNIXSYSTEM
			icvmsignum==ICVM_SIGKILL || icvmsignum==ICVM_SIGSTOP){
		R_ERRNO1=ICVM_EINVAL;
		R_R0=-1;
		return; // no good
	};
	R_R0=0;// good return
	sa=&(SYSTEMSTUFF->sigactions[icvmsignum]);
	if(oldact){ // return old action
		WDs32(oldact+ICVM_sa_handler,sa->handler,0);
		WDs32(oldact+ICVM_sa_flags,sa->flags,0);
		WDs32(oldact+ICVM_sa_mask,sa->mask,0);
	}
	if(!act)
		return;

	sa->flags=RDs32(act+ICVM_sa_flags,0);
	sa->mask=RDs32(act+ICVM_sa_mask,0) | (1<<icvmsignum);
	sa->handler=RDs32(act+ICVM_sa_handler,0);
#ifdef __WIN32__
	R_R0=0;
	return;
#endif // __WIN32__

#ifdef UNIXSYSTEM	
	if(sa->flags & ICVM_SA_NOCLDSTOP)
		flags |= SA_NOCLDSTOP;
	if(sa->flags & ICVM_SA_RESTART)
		flags |= SA_RESTART;
	sigfillset(&syssigaction.sa_mask); // only ever process one signal 
	syssigaction.sa_flags=flags;
	if(sa->handler==ICVM_SIG_DFL){
		syssigaction.sa_handler=SIG_DFL;
		SYSTEMSTUFF->cursigignore &= ~(1<<icvmsignum); 
		
	}else if(sa->handler==ICVM_SIG_IGN){
		SYSTEMSTUFF->cursigignore |= (1<<icvmsignum); 
		syssigaction.sa_handler=SIG_IGN;
	}else{
		syssigaction.sa_handler=sighandler;
		SYSTEMSTUFF->cursigignore &= ~(1<<icvmsignum); 
	}
	R_R0=sigaction(nativesignum,&syssigaction,&oa);
	icvm_SetErr();
	return;
#endif //UNIXSYSTEM	

	NOT_IMPLEMENTED;
};





void ICVM_kill(void){
	R_R0=i_kill(rSTKs32(0),rSTKu32(1));
	icvm_returnerr();
}


void ICVM_sigprocmask(void){
    int how=rSTKs32(0);
	p32 get=rSTKs32(1);
	p32 old=rSTKs32(2);

	ICVM_sigset_t syst; 
	ICVM_sigset_t *systp; 
	ICVM_sigset_t osyst; 
	if(get){
		systp=&syst;
		syst=RDs32(get,0);
	}else
		how=ICVM_SIG_SETMASK;
	R_R0=i_sigprocmask(how,systp,&osyst);
	if(old)
		WDs32(old,osyst,0);
	icvm_returnerr();
};


#ifndef __WIN32__



																




void ICVM_sigsuspend(void){
	ICVM_sigset_t *t=(ICVM_sigset_t *)rSTKs32(0);
	ICVM_sigset_t sig=RDs32(t,0);
	sigset_t syst;
	TranslateSigSetFromICVM(sig,&syst);
	R_R0=sigsuspend(&syst);
	icvm_SetErr();
};

#endif // !__WIN32__




#ifndef __WIN32__




// wait stuff

#define ICVM_WNOHANG		1	/* don't hang in wait */
#define ICVM_WUNTRACED	2	/* tell about stopped, untraced children */

static s32 TranslateWaitStatusToICVM(int stat){
	s32 istat=0;
	if(WIFEXITED(stat)) 
		istat|=(WEXITSTATUS(stat) & 255)<<8;
	else if(WIFSTOPPED(stat)){
		istat|=255;
		istat|= (TranslateSigToICVM(WSTOPSIG(stat)) & 255)<<8;
	}else if(WIFSIGNALED(stat))
		istat|= TranslateSigToICVM(WTERMSIG(stat));
	return istat;
};
	


// *********** NOTE - THIS EASILY COULD BE CONVERTED TO A WAITPID call 
// for systems with no wait 4
// as rusage is NOT being supported right now anyway.

void ICVM_wait4(void){
	int pid=rSTKs32(0);
	s32 *istat=(s32 *)rSTKs32(1);
	s32 iopt=rSTKs32(2);
	s32 *rusage=(s32 *)rSTKs32(3);
	int opt=0;
	int stat;
	if(iopt & ICVM_WNOHANG)
		opt|=WNOHANG;
	if(iopt & ICVM_WUNTRACED)
		opt|= WUNTRACED;
	R_R0=wait4(pid,&stat,opt,0); // do not support rusage for now
//	R_R0=waitpid(pid,&stat,opt); // do not support rusage for now
	if(rusage)
		memset((void *)rusage,0,256);
	if(istat){
		stat=TranslateWaitStatusToICVM(stat);
		WDs32(istat,stat,0);
	}
	icvm_SetErr();
}

	

void ICVM_execve(void){
	char *cmd=(char *)rSTKs32(0);
	p32 iargv=rSTKs32(1);
	p32 ienvp=rSTKs32(2);
	char **argv;
	char **envp;
	int acnt=0;
	int ecnt=0;
	int cnt;
	int asz,esz;
	while(iargv && RDs32(iargv + (acnt*4),0))
		++acnt;
	while(ienvp && RDs32(ienvp + (ecnt*4),0))
		++ecnt;
	asz=(acnt+1) * sizeof(char *);
	esz=(ecnt+1) * sizeof(char *);
	argv=(char **)alloca(asz);
	envp=(char **)alloca(esz);
	if(argv==0 || envp==0){
		fprintf(stderr,"FATAL - alloca failure\n");
		exit(3);
	}
	cnt=0;
	while(cnt<acnt){
		argv[cnt]=(s8 *)RDs32(iargv,0);
		++cnt;
		iargv+=4;
	}
	argv[cnt]=0;
	cnt=0;
	while(cnt<ecnt){
		envp[cnt]=(s8 *)RDs32(ienvp,0);
		++cnt;
		ienvp+=4;
	}
	envp[cnt]=0;
	R_R0=execve(cmd,argv,envp);	
	icvm_SetErr();

};


#endif // !__WIN32__

