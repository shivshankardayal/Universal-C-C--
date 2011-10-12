#include <stdio.h>

void OutputReg(char *s){
	printf("\"%s\",",s);
};

void NewLn(void){printf("\\\n\t");};

void OutputBaseRegisters(void){
	OutputReg("%r0");	
	OutputReg("%r1");	
	OutputReg("%r2");	
	OutputReg("%r3");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	NewLn();
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%q0");	
	OutputReg("%q1");	
	OutputReg("%fp");	
	OutputReg("%sp");	
	NewLn();
	OutputReg("%fp0");	
	OutputReg("%fp1");	
	OutputReg("%fp2");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	NewLn();
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	OutputReg("%???");	
	NewLn();
};

static int soffset=0;

typedef struct stackregalloc{
	int offset;
	char *name;
}stackregalloc;


static stackregalloc sr[128];
static stackregalloc sf[128];

void AddToSys(char *name,int size){
	int cnt=0;
	int alignmsk=size;
	if(alignmsk>8)
		alignmsk=8;
	alignmsk--;
	while(cnt<128){
		if(sf[cnt].name==0){
			sf[cnt].name=name;
			soffset-=size;
			while(soffset & alignmsk)
				--soffset;
			sf[cnt].offset=soffset;
			return;
		}
		++cnt;
	};	
};

void DumpSysDefines(void){
	int cnt=0;
	while(cnt<128){
		if(sf[cnt].name)
			printf("#define %s %d\n",sf[cnt].name,sf[cnt].offset);
		++cnt;
	}
	cnt=0;
	while(cnt<128){
		if(sr[cnt].name && strlen(sr[cnt].name))
			printf("#define %s %d\n",sr[cnt].name,sr[cnt].offset);
		++cnt;
	}
};


static int siregs;
void AllocSIReg(char *name){
	int cnt=32;
	if(strlen(name) && strncmp(name,"GP",2)){
		AddToSys(name,4);
		return;
	}
	while(cnt<=47){
		if(sr[cnt].name==0){
			soffset-=4;
			sr[cnt].name=name;
			sr[cnt].offset=soffset;
			++siregs;
			return;
		}
		++cnt;
	}	
	fprintf(stderr,"Out Of SIREGS\n");
	exit(3);
};

static int dfregs;
void AllocDFReg(char *name){
	int cnt=48;
	if(strlen(name) && strncmp(name,"GP",2)){
		AddToSys(name,8);
		return;
	}
	while(cnt<=55){
		if(sr[cnt].name==0){
			soffset-=8;
			while(soffset &7)
				--soffset;
			sr[cnt].name=name;
			sr[cnt].offset=soffset;
			++dfregs;
			return;
		}
		++cnt;
	}	
	fprintf(stderr,"Out Of DFREGS\n");
	exit(3);
};

static int diregs;
void AllocDIReg(char *name){
	int cnt=56;
	if(strlen(name) && strncmp(name,"GP",2)){
		AddToSys(name,32);
		return;
	}
	while(cnt<=63){
		if(sr[cnt].name==0){
			soffset-=32;
			while(soffset &7)
				--soffset;
			sr[cnt].name=name;
			sr[cnt].offset=soffset;
			++diregs;
			return;
		}
		++cnt;
	}	
	fprintf(stderr,"Out Of DIREGS\n");
	exit(3);
};

static int hiregs;

void AllocHIReg(char *name){
	int cnt=64;
	if(strlen(name) && strncmp(name,"GP",2)){
		AddToSys(name,2);
		return;
	}
	while(cnt<=71){
		if(sr[cnt].name==0){
			soffset-=2;
			sr[cnt].name=name;
			sr[cnt].offset=soffset;
			++hiregs;
			return;
		}
		++cnt;
	}	
	fprintf(stderr,"Out Of HIREGS\n");
	exit(3);
};

static int qiregs;
void AllocQIReg(char *name){
	int cnt=72;
	if(strlen(name) && strncmp(name,"GP",2)){
		AddToSys(name,1);
		return;
	}
	while(cnt<=79){
		if(sr[cnt].name==0){
			soffset-=1;
			sr[cnt].name=name;
			sr[cnt].offset=soffset;
			++qiregs;
			return;
		}
		++cnt;
	}	
	fprintf(stderr,"Out Of QIREGS\n");
	exit(3);
};


static int sfregs;
void AllocSFReg(char *name){
	int cnt=80;
	if(strlen(name) && strncmp(name,"GP",2)){
		AddToSys(name,4);
		return;
	}
	while(cnt<=87){
		if(sr[cnt].name==0){
			soffset-=4;
			sr[cnt].name=name;
			sr[cnt].offset=soffset;
			++sfregs;
			return;
		}
		++cnt;
	}	
	fprintf(stderr,"Out Of SFREGS\n");
	exit(3);
};


void AllocSysFrame(void){
	AllocSIReg("rSH_globalptr");
	AllocSIReg("rSH_r1");
	AllocSIReg("rSH_r2");
	AllocSIReg("rSH_r3");
	AllocSIReg("");
	AllocSIReg("");
	AllocDFReg("rSH_fp1");
	AllocDFReg("rSH_fp2");
	AllocSIReg("");
	AllocSIReg("");
	AllocSFReg("");
	AllocSFReg("");
	AllocSFReg("");
	AllocSFReg("");
	AllocHIReg("");
	AllocHIReg("");
	AllocHIReg("");
	AllocHIReg("");
	AllocDIReg("rSH_q1");
	AllocSIReg("rSH_reserved1");
	AllocSIReg("rSH_savepc");
	AllocSIReg("rSH_return2");
	AllocSIReg("rSH_return3");
};


void FinishAlloc(void){
	while(siregs<8)
		AllocSIReg("");
	while(qiregs<4)
		AllocQIReg("");
	while(hiregs<4)
		AllocHIReg("");
	while(dfregs<4)
		AllocDFReg("");
	while(sfregs<4)
		AllocSFReg("");
	while(diregs<2)
		AllocDIReg("");
	while(siregs<16)
		AllocSIReg("");
	while(dfregs<8)
		AllocDFReg("");
	while(sfregs<8)
		AllocSFReg("");
	while(diregs<8)
		AllocDIReg("");
	while(hiregs<8)
		AllocHIReg("");
	while(qiregs<8)
		AllocQIReg("");
}


void OutputStackRegs(void){
	int cnt=32;
	int lcnt=0;
	char buf[256];
	while(cnt<128){
		if(sr[cnt].name==0)
			OutputReg("(???)");
		else{
			sprintf(buf,"(%d,%%fp)",sr[cnt].offset);
			OutputReg(buf);	
		}
		++lcnt;
		if(lcnt==4){
			NewLn();
			lcnt=0;
		}
		++cnt;
	}
};
	

int main(void){
	printf("#define REGISTER_NAMES  {");
	NewLn();
	OutputBaseRegisters();
	AllocSysFrame();
	FinishAlloc();
	OutputStackRegs();
	printf("}\n\n");
	while(soffset & 7)
		--soffset;
	printf("#define STARTING_FRAME_OFFSET  (%d)\n\n",soffset);
	printf("/* **** PRIVATE FRAME MAPPINGS **** */\n");
	DumpSysDefines();

};


#define REGISTER_NAMES \
{"%r0", "%r1", "%r2", "%r3", "%??", "%??", "%??", "%??",	\
 "%??a0", "%??a1", "%??a2", "%??a3", "%q0", "%q1", "%fp", "%sp",	\
 "%fp0", "%fp1", "%fp2", "%fp3", "%???", "%???", "%???", "%???",\
 "%??", "%??", "%??", "%??", "???", "???", "???", "???",	\
/* 32 bit SI registers 32 - 47 */\
 "(-4,%fp)", "(-8,%fp)", "(-12,%fp)", "(-16,%fp)",\
 "(-20,%fp)", "(-24,%fp)", "(-28,%fp)", "(-32,%fp)",	\
 "(-36,%fp)", "(-40,%fp)", "(-44,%fp)", "(-48,%fp)",\
 "(-52,%fp)", "(-56,%fp)", "(-60,%fp)", "(-64,%fp)",	\
/* 64 bit DF registers  48 - 55 */\
 "(-72,%fp)", "(-80,%fp)", "(-88,%fp)", "(-96,%fp)",	\
 "(-104,%fp)", "(-112,%fp)", "(-120,%fp)", "(-128,%fp)",	\
/* 256 bit DI registers 56-63 */\
 "(-136,%fp)", "(-168,%fp)", "(-200,%fp)", "(-232,%fp)",	\
 "(-264,%fp)", "(-296,%fp)", "(-328,%fp)", "(-360,%fp)",	\
/* 16 bit HI registers 64 - 71 */\
 "(-392,%fp)", "(-394,%fp)", "(-396,%fp)", "(-398,%fp)",	\
 "(-400,%fp)", "(-402,%fp)", "(-404,%fp)", "(-406,%fp)",	\
/* 8 bit QI registers 72 - 79 */\
 "(-408,%fp)", "(-409,%fp)", "(-410,%fp)", "(-411,%fp)",	\
 "(-412,%fp)", "(-413,%fp)", "(-414,%fp)", "(-415,%fp)",	\
/* 32 bit SF registers 80 - 87 */\
 "(-416,%fp)", "(-420,%fp)", "(-424,%fp)", "(-428,%fp)",	\
 "(-432,%fp)", "(-436,%fp)", "(-440,%fp)", "(-444,%fp)",	\
\
 "(???,%fp)", "(???,%fp)", "(???,%fp)", "(???,%fp)",	\
 "(???,%fp)", "(???,%fp)", "(???,%fp)", "(???,%fp)",	\
\
 "(???,%fp)", "(???,%fp)", "(???,%fp)", "(???,%fp)",	\
 "(???,%fp)", "(???,%fp)", "(???,%fp)", "(???,%fp)",	\
 "(???,%fp)", "(???,%fp)", "(???,%fp)", "(???,%fp)",	\
 "(???,%fp)", "(???,%fp)", "(???,%fp)", "(???,%fp)",	\
 "(???,%fp)", "(???,%fp)", "(???,%fp)", "(???,%fp)",	\
 "(???,%fp)", "(???,%fp)", "(???,%fp)", "(???,%fp)",	\
 "(???,%fp)", "(???,%fp)", "(???,%fp)", "(???,%fp)",	\
 "(???,%fp)", "(???,%fp)", "(???,%fp)", "(???,%fp)",	\
\
}

#define STARTING_FRAME_OFFSET  (-512)
