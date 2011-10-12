#include "bfdstuff.h"
#include "cvm.h"

// ******************* NOTE --- IF YOU USE THIS YOU MUST PROVED SOFTLINKS
// **** IN THE CURRENT DIRECTORY TO THE binutils/bfd directory used
// to build the binutils for ICVM
// THE LINKS NEEDED:
// binutils	->   binutils	: to the base of the directory used for ICVM binutils


// Copyright (C) 2000,2001 Bob Dayley


static struct disassemble_info icvmdisinfo;

// read bytes from memory for the disassembler

static int DisasmReadMemory(bfd_vma memaddr, bfd_byte *myaddr,
		int length, struct disassemble_info *info){
	char *d=(char *)myaddr;
	char *s=(char *)memaddr;
	int cnt=length;
	while(cnt){
		*d++=*s++;
		--cnt;
	}
	return 0;

}


void DisasmMemoryError(int status,bfd_vma addr, struct disassemble_info *info){
	fprintf(stderr,"MEMORY ERROR CALLED\n");
}


int DisasmSymbolAtAddress(bfd_vma addr, struct disassemble_info *info){
	return 0;
}

// special - builtin tends to trash this
static int b_strlen(char *s){
	int cnt=0;
	while(*s){
		++cnt;
		++s;
	}
	return cnt;
};

static int bprintf (FILE *stream,
			 char *format, ...){
	long *p=(long *)(&format);
	char *s=(char *)stream;
	s+=b_strlen(s);
	sprintf(s,format,p[1],p[2],p[3],p[4]);
	return b_strlen(s);
}

static void DisasmPrintAddress(bfd_vma addr, struct disassemble_info *info){
	bprintf(icvmdisinfo.stream,"L%08.8X",addr);

}


static int needinit=1;

static void InitDisassembler(char *buf){
	icvmdisinfo.fprintf_func=(fprintf_ftype)bprintf;
	icvmdisinfo.stream=buf;
//	icvmdisinfo.flavour=bfd_target_unknown_flavour;
	icvmdisinfo.endian=BFD_ENDIAN_LITTLE;
#ifdef BIGENDIAN
	icvmdisinfo.current_endian=BFD_ENDIAN_BIG;
#else
	icvmdisinfo.current_endian=BFD_ENDIAN_LITTLE;
#endif
	icvmdisinfo.symbols=0;
	icvmdisinfo.num_symbols=0;
	icvmdisinfo.flags=0;
	icvmdisinfo.private_data=0;
	icvmdisinfo.read_memory_func=DisasmReadMemory;
	icvmdisinfo.memory_error_func=DisasmMemoryError;
	icvmdisinfo.print_address_func=DisasmPrintAddress;
	icvmdisinfo.symbol_at_address_func=DisasmSymbolAtAddress;
	needinit=0;
}




int ICVMDisassemble(void *addr,char *buf){
	static char outbuf[1024]; 
	int len;
	DUMPHWREGS;
	outbuf[0]=0;
	if(needinit)
		InitDisassembler(outbuf);
	len=print_insn_icvm((bfd_vma) addr,&icvmdisinfo);
	strcpy(buf,outbuf);
	LOADHWREGS;
	return len;
}




