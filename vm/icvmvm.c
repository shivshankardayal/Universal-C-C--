/* 			Copyrights(C) 1999,2000 by Bob A.Dayley */


//#define ICVMPROFILE
//#define ICVMDEBUG
//#define ICVMDEBUGSEGV // internal debug
//#define ICVMDEBUGSEGVBRK	308300LL	// internal debug

// define this to have everthing disassembled as it is executed
// SEE cvmdis.c and bfdstuff.h to make sure you have the proper softlink for this
//#define ICVM_DISASSEMBLEON

#define NEEDHWREGS // turn on high speed hardware registers

#include "cvm.h"
#include "icvmjunk.h"
#include "icvmsys.h"

extern char **environ;

#include <unistd.h>

#ifdef ICVMPROFILE

#include <math.h>
#include <sys/time.h>
#include <unistd.h>
static double icvmprofileavg[65536];
static double icvmprofile[65536];
static double icvmprofilecnt;
static  double icvmprofilesessions=0;
#endif


#ifndef ICVM_STACKSIZE
#define ICVM_STACKSIZE 1048576
//#define ICVM_STACKSIZE 8388608  // 8 meg stack - hopefully paged
#endif

#ifndef BIGANDFAST
#undef ICVMPROFILE
#endif


#if defined(ICVM_FORCEDALIGNMENTCHECK) || defined(ICVM_NATURALALIGNMENTCHECK32)\
		|| defined(ICVM_NATURALALIGNMENTCHECK64)

p32 icvm_checkalign(p32 addr,int align){
	if(((s32)addr) & (align-1))
		fprintf(stderr,"Alignment error addr=$%x align=%d\n",addr,align);
	return addr;
}


#endif

#ifdef CHECKILLEGALREADWRITES

// just used to help me write the messy addressing macros in cvm.h

int illegalwrite(void){
	fprintf(stderr,"ILLEGAL WRITE MODE\n");
	exit(3);
	return 0;
}

int illegalread(void){
	fprintf(stderr,"ILLEGAL READ MODE\n");
	exit(3);
	return 0;
}

#endif // CHECKILLEGALREADWRITES


// this is the structure that is handed to the startup program

// need to add an environment string ????

typedef p32 StartupParms[3];

extern  ICVM_debug();


FILE *icvmlogfile; // log file for kprintf - 

#ifdef MONITORREADWRITES  // internal debugging - not supported

static int m_nst=0;

p32 m_rd(int sz,p32 addr){
	if(m_nst>1){
		--m_nst;
		return addr;
	}
	R_rmtp=sz;
	R_rmaddr=addr;
	switch(R_wmtp){
		case 1:
			R_rmu8=(*(u8 *)R_rmaddr);
			break;
		case 2:
			R_rmu16=(*(u16 *)R_rmaddr);
			break;
		case 4:
			R_rmu32=(*(u32 *)R_rmaddr);
			break;
		case 8:
			R_rmu64=(*(u64 *)R_rmaddr);
			break;
	}
	--m_nst;
	return addr;
};	

p32  m_wr(int sz,p32 addr){
	if(m_nst>1){
		--m_nst;
		return addr;
	}
	R_wmtp=sz;
	R_wmaddr=addr;
	--m_nst;
	return addr;
};	

void m_startwr(void){
	++m_nst;
};

void m_startrd(void){
	++m_nst;
};

void m_finishwr(void){
	switch(R_wmtp){
		case 1:
			R_wmu8=(*(u8 *)R_wmaddr);
			break;
		case 2:
			R_wmu16=(*(u16 *)R_wmaddr);
			break;
		case 4:
			R_wmu32=(*(u32 *)R_wmaddr);
			break;
		case 8:
			R_wmu64=(*(u64 *)R_wmaddr);
			break;
	}
};	



#endif // MONITORREADWRITES 


#ifdef ICVMDEBUGSEGV

#include <signal.h>

#define SEGVSZ	16384
static s32 segvpc[SEGVSZ];
static s32 segvop[SEGVSZ];
static s32 segvr0[SEGVSZ];
static s32 segvr1[SEGVSZ];
static s32 segvr2[SEGVSZ];
static s32 segvr3[SEGVSZ];
static s32 segvsp[SEGVSZ];
static s32 segvfp[SEGVSZ];
static int segvindex;
static s64 segvcnt;
static p32 segvbasepc;
static p32 segvbasesp;

#ifdef MONITORREADWRITES 

static union R_MONITOR segvrm[SEGVSZ];
static union R_MONITOR segvwm[SEGVSZ];
static int segvrtp[SEGVSZ];
static int segvwtp[SEGVSZ];
static p32 segvraddr[SEGVSZ];
static p32 segvwaddr[SEGVSZ];

#endif // MONITORREADWRITES 


static void segvhandler(int handler);


static void segvadd(s32 pc,s32 op){
	++segvcnt;
	if(segvbasepc==0)
		segvbasepc=pc;
	if(segvbasesp==0)
		segvbasesp=R_SP;
	segvpc[segvindex]=pc;
	segvop[segvindex]=op;
	segvr0[segvindex]=R_R0;
	segvr1[segvindex]=R_R1;
	segvr2[segvindex]=R_R2;
	segvr3[segvindex]=R_R3;
	segvfp[segvindex]=R_FP;
	segvsp[segvindex]=R_SP;
#ifdef MONITORREADWRITES 
	segvrm[segvindex]=R_rm;
	segvwm[segvindex]=R_wm;
	segvrtp[segvindex]=R_rmtp;
	segvwtp[segvindex]=R_wmtp;
	segvraddr[segvindex]=R_rmaddr;
	segvwaddr[segvindex]=R_wmaddr;
#endif // MONITORREADWRITES 
	segvindex++;
	segvindex%=SEGVSZ;
#ifdef ICVMDEBUGSEGVBRK
	if(segvcnt>=ICVMDEBUGSEGVBRK)
		segvhandler(0); // break out and dump
#endif ICVMDEBUGSEGVBRK

};

static void segvhandler(int handler){
	int cnt=0;
	int i=segvindex;
	static int nest;
	s32 pc=R_PC;
	fprintf(stderr,"SEGV DUMP after %qd instructions\n",segvcnt);
	if(nest){
		fprintf(stderr,"DOUBLE SEGV\n");
		exit(3);
	}
	++nest;
	while(cnt<SEGVSZ){
	
#ifndef ICVM_DISASSEMBLEON
		fprintf(stderr,"%08.8x: %04.4x ",
			segvpc[i],segvop[i]);
#endif // ICVMDISASSEMBLEON
		fprintf(stderr,"R0:%08.8x R1:%08.8x R2:%08.8x R3:%08.8x\n",
			segvr0[i],segvr1[i],segvr2[i],segvr3[i]);
		fprintf(stderr,"SP:$%08.8x (*%08.8x) FP:$%08.8x (#%08.8x)\n",
			segvsp[i],segvbasesp-segvsp[i],
			segvfp[i],segvbasesp-segvfp[i]
			);
#ifdef MONITORREADWRITES 
		fprintf(stderr,"LastRead: $%08.8x ",segvraddr[i]);
		switch(segvrtp[i]){
			case 1:
				fprintf(stderr,"%02.2x\n",segvrm[i].mu8);
				break;
			case 2:
				fprintf(stderr,"%04.4x\n",segvrm[i].mu16);
				break;
			case 4:
				fprintf(stderr,"%08.8x\n",segvrm[i].mu32);
				break;
			case 8:
				fprintf(stderr,"%qx\n",segvrm[i].mu64);
				break;
			default:
				fprintf(stderr,"???\n");
		};
		fprintf(stderr,"LastWrite:$%08.8x ",segvwaddr[i]);
		switch(segvwtp[i]){
			case 1:
				fprintf(stderr,"%02.2x\n",segvwm[i].mu8);
				break;
			case 2:
				fprintf(stderr,"%04.4x\n",segvwm[i].mu16);
				break;
			case 4:
				fprintf(stderr,"%08.8x\n",segvwm[i].mu32);
				break;
			case 8:
				fprintf(stderr,"%qx\n",segvwm[i].mu64);
				break;
			default:
				fprintf(stderr,"???\n");
		};
		
#endif // MONITORREADWRITES
#ifdef ICVM_DISASSEMBLEON
		{
			static char buf[4096];
			fprintf(stderr,"\n%08.8x:(@%08.8x) ",segvpc[i],
				segvpc[i]-segvbasepc,buf);	
			ICVMDisassemble(Nptr(segvpc[i]),buf);
			fprintf(stderr,"%s - @@%qd\n",buf,(s64)(segvcnt-(s64)SEGVSZ)+(s64)cnt);	
		}
#endif // ICVMDISASSEMBLEON

		++i;
		i%=SEGVSZ;
		++cnt;
	}
	fprintf(stderr,"SEGV VIOLATION pc=$%x - cnt=%qd EXITING\n",pc,segvcnt);
	exit(3);
};

static void setsegvhandler(void){
	struct sigaction act;
	act.sa_handler=segvhandler;
	act.sa_flags=0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGSEGV,&act,0);
};

#endif // ICVMDEBUGSEGV

struct ICVMMachine *_R_LM;

int ICVMMain(int argc,char **argv,p32 pc){
	int cnt;
	int ecnt;
	p32 startup;
	p32 str;
	int icvmfd=dup(2);
	int status;
	p32 stack;
	p32 newargv;
	p32 newenv;
	struct ICVMMachine *oldmachine; // need to save this
	struct ICVMMachine localmachine;

	SET_LM(&localmachine); // set up our initial machine

	icvmlogfile=fdopen(icvmfd,"w");	
	if(icvmlogfile)
  		setvbuf(icvmlogfile,0,_IONBF,0);

#ifdef __WIN32__
	if(!i_thisisfork)
#endif

// **** all this stuff should be skipped on an artificial fork
	{	
		stack=Iptr(i_malloc(ICVM_STACKSIZE & -8));
		startup=Iptr(i_malloc(sizeof(StartupParms)));
		if(stack==0 || startup==0)
			return -1;
		_R_SP=stack+(ICVM_STACKSIZE & -8);
		_R_SP-=16;
		while(_R_SP  & 7)
			--_R_SP; //  align it
		_R_FP=_R_SP; // initialize the frame pointer
		_R_SP+=STARTING_FRAME_OFFSET; // for variables that might be accessed by frame pointer
		newargv=Iptr(i_malloc(sizeof(s32) * (argc+1)));
		if(newargv==0)
			return -1;
		cnt=0;
		str=newargv;
		while(cnt<argc){
			WDs32(str,Iptr(i_strdup(argv[cnt])),0);
			++cnt;
			str+=4;
		}
		WDs32(str,0,0);

	
		ecnt=0;

#ifndef __WIN32__ // not working right now ????

		while(environ[ecnt])
			++ecnt;

#endif

		newenv=Iptr(i_malloc(sizeof(s32) * (ecnt+2)));
		str=newenv;
		cnt=0;

		while(cnt<ecnt){
			WDs32(str,Iptr(i_strdup(environ[cnt])),0);
			++cnt;
			str+=4;
		}
		WDs32(str,0,0);

		WDs32(Iptr(startup),argc,0);
		WDs32(Iptr(startup + 4),newargv,0);
		WDs32(Iptr(startup + 8),newenv,0);


		_R_SP-=8; // put pointer to this struct on top of stack
		WDs32(_R_SP,startup,0);
		_R_SP-=8; // simulate a subroutine call
		_R_PC=pc;
	}

#ifdef ICVMDEBUGSEGV
	setsegvhandler();
#endif // ICVMDEBUGSEGV

	ICVMRunLoop();
#ifdef __WIN32__
	if(!i_thisisfork)
#endif

	{
		i_free(Nptr(newargv));
		i_free(Nptr(newenv));
		i_free(Nptr(stack));
	}
// ********** end of artificial fork skip

#ifdef ICVMPROFILE
{
	time_t tm;
	FILE *f=fopen("/tmp/icvmprofile","r");
	int cnt=0;
	int allzeros=1;
	double d;
	double dcnt;
	char buf[512];
	time(&tm); // set up random number generator to be truly random
	srand(tm);
	if(f){
		fgets(buf,511,f);
		icvmprofilesessions=atof(buf);
		while(cnt<65536){
			fgets(buf,511,f);
			icvmprofileavg[cnt]=atof(buf);
			if(icvmprofileavg[cnt])
				allzeros=0;
			++cnt;
		}
		fclose(f);
		if(allzeros)
			fprintf(stderr,"Warning - profile file returns all zeros\n");
	}
	f=fopen("/tmp/icvmprofile","w");
	cnt=0;
	dcnt=(double)icvmprofilecnt;
	if(f){
		icvmprofilesessions+=(dcnt);
		fprintf(f,"%1.1f\n",icvmprofilesessions);
		cnt=0;
		while(cnt<65536){
			d=(double)icvmprofile[cnt];
			d=d+icvmprofileavg[cnt];
			fprintf(f,"%1.1f\n",d);
			++cnt;
		}
		fclose(f);
	}
}
#endif
	exit(_R_R0);
}




volatile ICVMATTNTYPE icvmattn=0; // attention flags
volatile ICVMATTNTYPE *icvmattnptr=&icvmattn; // attention flags

#ifdef ICVM_DISASSEMBLEON
#include "cvmdis.c"
static char disbuf[512];
#endif


i_jmp_buf *icvmattnjmp=0;
i_jmp_buf *returnjmpbuf;

extern SysCallFunction *BaseSysCalls[1024];
extern SysCallFunction *XSysCalls[1024];
extern SysCallFunction *FASTSysCalls[1024];
extern SysCallFunction *OpenGLSysCalls[1024];


SysCallFunction **syscallsLowBlocks[4096]={
	BaseSysCalls,
	0,
	0,
	OpenGLSysCalls,
	XSysCalls,
	0,
	0,
	0,
	FASTSysCalls,
};

SysCallFunction ***syscallsHiBlocks[1024]={
	syscallsLowBlocks,
};


void SysCall(u32 func){
	SysCallFunction ***fhi;
	SysCallFunction **flo;
	SysCallFunction *f;
//fprintf(icvmlogfile,"SYSCALL %d\n",func);
	fhi=syscallsHiBlocks[(func>>22) & 1023];
	if(fhi){
		flo=fhi[(func>>10) & 4095];
		if(flo){
			f=flo[func & 1023];
			if(f){
				(*f)();			
//fprintf(icvmlogfile,"ENDSYSCALL %d\n",func);
				return;
			}
		}
	}
	fprintf(stderr,"FATAL: Bad System Call $%x %d\n",func,func);
	exit(-3);	
};





// this small mess is the runtime loop, with optimzation macros thrown in.
// the optimized code is on icvmopt.c, to avoid making this into a large mess


void ICVMRunLoop(void){
// save processor state
	i_jmp_buf newbuf;
	newbuf.oreturn=returnjmpbuf;
	newbuf.oattnjmp=icvmattnjmp;
	returnjmpbuf=&newbuf; // return frame for recursive calls
	if(icvmattnjmp==0)
		icvmattnjmp=returnjmpbuf;
	if(sigsetjmp(newbuf.buf,0)){
		icvmattnjmp=returnjmpbuf->oattnjmp;
		returnjmpbuf=returnjmpbuf->oreturn;
		ICVM_CLEARATTN(ICVM_ZERORETURN); // this might be a recursive return
		return; // exit 
	}
	LOADHWREGS; // load up native registers if used


	for(;;){


#ifndef BIGANDFAST
		u32 opcode=RCu16(R_PC,0);

#ifdef ICVMDEBUGSEGV
		segvadd(R_PC,opcode);
#endif // ICVMDEBUGSEGV


#ifdef ICVMDEBUG
		DUMPHWREGS;
//fprintf(stdout,"$%x: $%x fork=%d\n",_R_PC,opcode,i_thisisfork);
//fprintf(stderr,"$%x: $%x R0=$%x R1=$%x R2=$%x R3=$%x SP=$%x FP=$%x\n",
//_R_PC,opcode,_R_R0,_R_R1,_R_R2,_R_R3,_R_SP,_R_FP);
//fprintf(stderr,"\b\b\b\b%x",opcode);
		LOADHWREGS; // load up native registers if used
#endif // ICVMDEBUG

		{
			R_LASTPC=R_PC;
			R_LASTSP=R_SP;
			R_PC+=2;
			R_sinfo=&(sinfo[sopcodetable[opcode]]);
			R_soffset=opcode-R_sinfo->offset;
			(*(R_sinfo->handler))();
		}

#else // BIGANDFAST
#ifdef ICVMDEBUG
		{
			u16 oc=RCu16(R_PC,0);
		DUMPHWREGS;
fprintf(stderr,"$%x: $%x\n",_R_PC,oc);
		LOADHWREGS; // load up native registers if used
			(*(shortfunctions[oc]))();
		}

#else // ICVMDEBUG

#ifdef ICVMPROFILE 
			{
				static struct timeval cur,last;
				u16 oc=RCu16(R_PC,0);
				gettimeofday(&cur,0);
				cur.tv_usec &=0xffffe000;
				if(cur.tv_usec!=last.tv_usec ||
						cur.tv_sec!=last.tv_sec){
					icvmprofile[oc]++;
					++icvmprofilecnt;
					last.tv_usec=cur.tv_usec;
					last.tv_sec=cur.tv_sec;
				}
				(*(shortfunctions[oc]))();
			}
#endif // ICVMPROFILE

#define CHECKFORINTERRUPT	if(ICVMATTN){DUMPHWREGS;break;}

#ifdef ICVMDEBUGSEGV

#define EXECUTE1INSTRUCTION	\
	segvadd(R_PC,RCu16(R_PC,0));(*(shortfunctions[RCu16(R_PC,0)]))();
#else
#define EXECUTE1INSTRUCTION		(*(shortfunctions[RCu16(R_PC,0)]))();
#endif

#define DO16\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION\
			EXECUTE1INSTRUCTION

#define DO64	DO16;DO16;CHECKFORINTERRUPT;DO16;DO16;CHECKFORINTERRUPT;

				DO64;	// execute 256 times before looping	
				DO64;	
				DO64;	
				DO64;	

#endif // ICVMDEBUG


#endif // !BIGANDFAST


		if(ICVMATTN){ // attention called, examine it
			DUMPHWREGS;
			HandleAttn();
			LOADHWREGS;
		}
	}

};







