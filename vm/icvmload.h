
// ********************** NOTE ************************
// ********** NEED TO ADD CHECKS ON FILE HEADER FOR FAERY etc.
// *** also need to make check this to see if lib or executable
// ** executables may be used by the intrepteter directly,
// ** libs may only be loaded -- might be a VM issue, but needs to be marked

// Copyright (C) 2000,2001 Bob Dayley


#ifndef ICVM_LOAD_H
#define ICVM_LOAD_H

//#define USEMMAP	1 // to use MMAP to map the files rather than loading entire files
				  // has the side effect in that if a command is already running
				 // the shell will refuse to run the file, citing text file is busy.
				 // however, you may force it by using: icvm command, rather
				 // then invoking it directly

#define USEZEROMMAP	1 // to use MMAP to allocate memory from /dev/zero 
					  // allows write protection to be used on executable
	
#include "fileio.h"
#include "cvm.h"
#include "sys.h"

#if defined(USEMMAP) || defined(USEZEROMMAP)

#ifndef __WIN32__

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#endif

#endif

// header information and offsets

#define ICVMELFSHHEADER "#!/bin/icvm\n"
#define ICVMELFSHHEADERSZ 16

#define EI_MAG0		16		/* File identification byte 0 index */
#define ELFMAG0		'F'		/* Magic number byte 0 */

#define EI_MAG1		17		/* File identification byte 1 index */
#define ELFMAG1		'A'		/* Magic number byte 1 */

#define EI_MAG2		18		/* File identification byte 2 index */
#define ELFMAG2		'E'		/* Magic number byte 2 */

#define EI_MAG3		19		/* File identification byte 3 index */
#define ELFMAG3		'R'		/* Magic number byte 3 */

#define EI_MAG4		20		/* File identification byte 4 index */
#define ELFMAG4		'Y'		/* Magic number byte 4 */

#define EI_MAG5		21		/* File identification byte 5 index */
#define ELFMAG5		'!'		/* Magic number byte 5 */

#define EI_MAG6		22		/* File identification byte 6 index */
#define ELFMAG6		0x7f		/* Magic number byte 6 */


#define EI_CLASS	23		/* File class */
#define ELFCLASSNONE	0		/* Invalid class */

#define ELFCLASS32	1		/* 32-bit objects */
#define ELFCLASS64	2		/* 64-bit objects */

#define EI_DATA		24		/* Data encoding */
#define ELFDATANONE	0		/* Invalid data encoding */
#define ELFDATA2LSB	1		/* 2's complement, little endian */
#define ELFDATA2MSB	2		/* 2's complement, big endian */

#define EI_VERSION	25		/* File version */
#define EI_TIME1	26		/* timestamp MSB */
#define EI_TIME2	27		/* timestamp  */
#define EI_TIME3	28		/* timestamp  */
#define EI_TIME4	29		/* timestamp LSB */

#define EI_PAD		30		/* Start of padding bytes */


/* Values for e_type, which identifies the object file type */

#define ET_NONE		0		/* No file type */
#define ET_REL		1		/* Relocatable file */
#define ET_EXEC		2		/* Executable file */
#define ET_DYN		3		/* Shared object file */
#define ET_CORE		4		/* Core file */
#define ET_LOPROC	0xFF00		/* Processor-specific */
#define ET_HIPROC	0xFFFF		/* Processor-specific */


//  ICVM ELF relocation types
//  Derived from relocation r_info

// type mask for r_info 

#define RELA_TYPE(x) ((x) & 255)

// index mask for r_info

#define RELA_INDEX(x) ((x)>>8)


#define R_ICVM_NONE	0
#define R_ICVM_32	1
#define R_ICVM_16	2
#define R_ICVM_8		3
#define R_ICVM_PC32	4
#define R_ICVM_PC16	5
#define R_ICVM_PC8	6
#define R_ICVM_GOT32	7
#define R_ICVM_GOT16	8
#define R_ICVM_GOT8	9
#define R_ICVM_GOT32O	10
#define R_ICVM_GOT16O	11
#define R_ICVM_GOT8O	12
#define R_ICVM_PLT32	13
#define R_ICVM_PLT16	14
#define R_ICVM_PLT8	15
#define R_ICVM_PLT32O	16
#define R_ICVM_PLT16O	17
#define R_ICVM_PLT8O	18
#define R_ICVM_COPY	19
#define R_ICVM_GLOB_DAT	20
#define R_ICVM_JMP_SLOT	21
#define R_ICVM_RELATIVE	22




class RelA{
public:

// ************** file format
	p32	r_offset;
	u32	r_info;
	s32	r_addend; // MAY BE OBSOLETE

// ************* end file format


	virtual void Read(ICVMGenericFile *r){
		r->Read(r_offset);
		r->Read(r_info);
		r->Read(r_addend);
	};


};

class Symbol{
public:

// ******* FILE FORMAT
	u32	st_name;
	p32	st_value;
	u32	st_size;
	u8	st_info;
	u8	st_other;
	u16	st_shndx;
// ****************

	virtual void Read(ICVMGenericFile *r){
		r->Read(st_name);
		r->Read(st_value);
		r->Read(st_size);
		r->Read(st_info);
		r->Read(st_other);
		r->Read(st_shndx);
	};
	
	inline char *GetName(char *strings){return &(strings[st_name]);};



};




// Dynamic section tags - d_tag

#define DT_NULL		0
#define DT_NEEDED	1
#define DT_PLTRELSZ	2
#define DT_PLTGOT	3
#define DT_HASH		4
#define DT_STRTAB	5
#define DT_SYMTAB	6
#define DT_RELA		7
#define DT_RELASZ	8
#define DT_RELAENT	9
#define DT_STRSZ	10
#define DT_SYMENT	11
#define DT_INIT		12
#define DT_FINI		13
#define DT_SONAME	14
#define DT_RPATH	15
#define DT_SYMBOLIC	16
#define DT_REL		17
#define DT_RELSZ	18
#define DT_RELENT	19
#define DT_PLTREL	20
#define DT_DEBUG	21
#define DT_TEXTREL	22
#define DT_JMPREL	23


// dynamic section structure

class Dyn{
public:

// ************** file format
	u32 d_tag;
	u32	d_val;

// ************* end file format


	virtual void Read(ICVMGenericFile *r){
		r->Read(d_tag);
		r->Read(d_val);
	};


};


// internal definitions
// for coding types
#define SH_NEEDINIT		-1
#define SH_UNDEFINED 	0
#define SH_TEXT			1
#define SH_DATA			2
#define SH_RDATA		3
#define SH_BSS			4


#define SH_STRTAB		5
#define SH_SHSTRTAB		6

#define SH_RELATEXT		7
#define SH_RELADATA		8
#define SH_RELARDATA	9

#define SH_DYNSYM		10
#define SH_DYNSTR		11

#define SH_CTOR			12
#define SH_DTOR			13


#define SH_RELACTOR		14
#define SH_RELADTOR		15

#define SH_NUMTYPES		16





#define SH_MAXSECTIONS 64

class ElfSection;

class ElfFileBase{
	ElfSection *shdrsByType[SH_NUMTYPES];
	ElfSection *shdrsBySHIndex[SH_MAXSECTIONS];
public:

	ElfFileBase(){
		int cnt;
		cnt=0;
		while(cnt<SH_NUMTYPES)
			shdrsByType[cnt++]=0;
		cnt=0;
		while(cnt<SH_MAXSECTIONS)
			shdrsBySHIndex[cnt++]=0;
	};

	inline void SetSectionByType(ElfSection *sect,int n){
		shdrsByType[n]=sect;
	};

	inline ElfSection *GetSectionByType(int tp){return shdrsByType[tp];};

	inline void SetSectionBySHIndex(ElfSection *sect,int n){
		if(n<0 || n>=SH_MAXSECTIONS){
			fprintf(stderr,"Illegal section number - SetSectionBySHIndex=%d\n",n);
			exit(3);
		}
		shdrsBySHIndex[n]=sect;
	};

	inline ElfSection *GetSectionBySHIndex(int tp){
		if(tp<0 || tp>=SH_MAXSECTIONS)
			return 0;
		return shdrsBySHIndex[tp];	
	};

	inline ElfSection *GetTextSection(void){return GetSectionByType(SH_TEXT);};		
	inline ElfSection *GetDataSection(void){return GetSectionByType(SH_DATA);};		
	inline ElfSection *GetRDataSection(void){return GetSectionByType(SH_RDATA);};		
	inline ElfSection *GetCtorSection(void){return GetSectionByType(SH_CTOR);};		
	inline ElfSection *GetDtorSection(void){return GetSectionByType(SH_DTOR);};		
	inline ElfSection *GetBSSSection(void){return GetSectionByType(SH_BSS);};		
	inline ElfSection *GetTextRelocSection(void){return GetSectionByType(SH_RELATEXT);};		
	inline ElfSection *GetDataRelocSection(void){return GetSectionByType(SH_RELADATA);};		
	inline ElfSection *GetRDataRelocSection(void){return GetSectionByType(SH_RELARDATA);};		
	inline ElfSection *GetCtorRelocSection(void){return GetSectionByType(SH_RELACTOR);};		
	inline ElfSection *GetDtorRelocSection(void){return GetSectionByType(SH_RELADTOR);};		
	inline ElfSection *GetDynamicSymbolSection(void){return GetSectionByType(SH_DYNSYM);};		
	virtual void *GetTextSectionData(void)=0;

	virtual p32 ConvertFromElfAddress(p32 addr)=0;

};



/* sh_type */
#define SHT_NULL	0
#define SHT_PROGBITS	1
#define SHT_SYMTAB	2
#define SHT_STRTAB	3
#define SHT_RELA	4
#define SHT_HASH	5
#define SHT_DYNAMIC	6
#define SHT_NOTE	7
#define SHT_NOBITS	8
#define SHT_REL		9
#define SHT_SHLIB	10
#define SHT_DYNSYM	11
#define SHT_NUM		12
#define SHT_LOPROC	0x70000000
#define SHT_HIPROC	0x7fffffff
#define SHT_LOUSER	0x80000000
#define SHT_HIUSER	0xffffffff

/* sh_flags */
#define SHF_WRITE		0x1
#define SHF_ALLOC		0x2
#define SHF_EXECINSTR	0x4
#define SHF_BIGENDIAN	0x8
#define SHF_MASKPROC	0xf0000000

/* special section indexes */
#define SHN_UNDEF	0
#define SHN_LORESERVE	0xff00
#define SHN_LOPROC	0xff00
#define SHN_HIPROC	0xff1f
#define SHN_ABS		0xfff1
#define SHN_COMMON	0xfff2
#define SHN_HIRESERVE	0xffff
 
#define ELF_SHDR_SIZE	40 // 40 bytes we are expecting



class ElfSection{
protected:
	ElfFileBase *ebase; // global access 
	ElfSection *next;
	void *data;

	char name[16];
	int internaltype;
	int shindex;
public:



	void Init(ElfFileBase *e){
		next=0;
		data=0;

		strcpy(name,"NONAME");
		ebase=e;
	};


	ElfSection(ElfFileBase *e,ICVMGenericFile *r,char *tname,int codetype,
			int sindex){
//		r->Read((void *)name,sizeof(name));
		internaltype=codetype;
		Init(e);
		shindex=sindex;
		strcpy(name,tname);
		e->SetSectionByType(this,codetype);
		e->SetSectionBySHIndex(this,sindex);
		r->Read(sh_type);
		r->Read(sh_flags);
		r->Read(sh_addr);
		r->Read(sh_offset);
		r->Read(sh_size);
		r->Read(sh_link);
		r->Read(sh_info);
		r->Read(sh_addralign);
		r->Read(sh_entsize);
	};

	virtual void Print(void){};

	inline int GetCodeType(void){return internaltype;};
	
// **** this stuff is the physical file format
//	u32	sh_name;
	u32	sh_type;
	u32	sh_flags;
	p32	sh_addr;
	u32	sh_offset;
	u32	sh_size;
	u32	sh_link;
	u32	sh_info;
	u32	sh_addralign;
	u32	sh_entsize;
//***** end physical format

	inline ElfSection *Next(void){return next;};
	inline void *Data(void){return data;};

	inline int Type(void){return sh_type;};

	virtual RelA *Relocations(void){return 0;};
	virtual int NumRelocations(void){return 0;};

	inline p32 BaseAddress(void){return sh_addr;};
	inline u32 GetDataSize(void){return sh_size;};


	inline char *Name(void){return name;};

	inline void Link(ElfSection **list){
		ElfSection **tail=list;
		while(*tail)
			tail=&((*tail)->next);
		*tail=this;
	};
		



	// find section  address as classified by input elf address, if possible
	// if not, hand off to Next, or return 0

	virtual p32 GetSectionAddressFromElf(p32 addr){
		if(next)
			return next->GetSectionAddressFromElf(addr);
		return 0;
	};
 

	virtual p32 ConvertFromElfAddress(p32 addr){
		return ebase->ConvertFromElfAddress(addr);
	};



	virtual void FinishInit(void){};
	virtual void InitData(ICVMGenericFile *r,char *basemem,u32 baseaddr){};
	virtual void  GetMemoryNeeds(size_t &memsz,size_t &filesz,u32 baseaddr,u32 basefileoffs){};
};


class RelocationSection : public ElfSection{
	RelA *relocations;
	int numrelocations;
public:
	RelocationSection(ElfFileBase *e,ICVMGenericFile *r,char *tname,
			int codetype,int sindex):
		ElfSection(e,r,tname,codetype,sindex){
			numrelocations=sh_size/sh_entsize;					
			RelA *rel=new RelA[numrelocations];
			int cnt;
//fprintf(stderr,"RELA=%s\n",name);
			if(rel){
				cnt=0;
				while(cnt<numrelocations){
					r->SetAbsPos(sh_offset+(cnt * sh_entsize));
					rel[cnt].Read(r);
					++cnt;
				}
			}
			relocations=rel;
		
	};

	virtual RelA *Relocations(void){return relocations;};
	virtual int NumRelocations(void){return numrelocations;};


};


class StringSection : public ElfSection{
	char *strings;
public:
	StringSection(ElfFileBase *e,ICVMGenericFile *r,char *tname,
			int codetype,int sindex):
		ElfSection(e,r,tname,codetype,sindex){
			strings=new char[sh_size];
				r->SetAbsPos(sh_offset);
				r->Read((void *)strings,sh_size);

#ifdef NOT_NOW		
	printf("STRINGS=%s sz=%d string1=%s\n",name,sh_size,strings);
		int cnt=0;
		char *s=strings;
		while(cnt<sh_size){
			printf("\t\tSTRING[%d]=%s\n",cnt,s);
			while(*s && cnt<sh_size){
				++s;
				++cnt;
			}
			++s;
			++cnt;
		}
#endif
							

	};

	virtual char *Strings(int n){
		if(n<0 || n>=sh_size)
			return "";
		return &(strings[n]);
	};

	virtual int StringSize(void){return sh_size;};

	~StringSection(){
		if(strings)
			delete strings;
	}
};




class SymbolSection : public ElfSection{
	Symbol *symbols;
	int numsymbols;
	char *strings;
	int stringsize;
public:
	SymbolSection(ElfFileBase *e,ICVMGenericFile *r,char *tname,
			int codetype,int sindex):
		ElfSection(e,r,tname,codetype,sindex){
			strings=0;
			numsymbols=sh_size/sh_entsize;					
			Symbol *rel=new Symbol[numsymbols];
			int cnt;
//fprintf(stderr,"SYMBOL=%s\n",name);
			if(rel){
				cnt=0;
				while(cnt<numsymbols){
					r->SetAbsPos(sh_offset+(cnt * sh_entsize));
					rel[cnt].Read(r);
					++cnt;
				}
			}
			symbols=rel;
		
	};

	inline Symbol *Symbols(void){return symbols;};

	inline Symbol *Symbols(int n){
		if(n<0 || n>=NumSymbols())
			return 0;
		return &(symbols[n]);
	};

	inline int NumSymbols(void){return numsymbols;};
	virtual int StringCodeType(void){return SH_STRTAB;};

	inline char *GetString(int n)
		{
			if(n<0)
				return 0;
			if(strings==0){
				StringSection *sts=
					(StringSection *)ebase->GetSectionByType(StringCodeType());
				if(sts==0){
					fprintf(stderr,"Fatal: StringTable absent for section %s!\n",Name());
					exit(3);
				}
				strings=sts->Strings(0);
				stringsize=sts->StringSize();
			}
			if(n>=stringsize)
				return 0;
			return &(strings[n]);
		};

	inline p32 GetSymbolAddress(Symbol *s){
		return ConvertFromElfAddress(s->st_value);
	};

	inline char *GetSymbolName(Symbol *s){
		return s->GetName(GetString(0));
	};

	virtual void Print(void){
		int cnt=0;
		Symbol *s;
		for(;;){
			s=Symbols(cnt);
			if(s==0)
				break;
			Print(s);
			++cnt;
		}
	};

	inline void Print(Symbol *s){
		printf("SYMBOL name=%s shndx=%d value=$%x info=$%x address=$%x\n",
					s->GetName(GetString(0)),s->st_shndx,
					s->st_value,s->st_info,
					GetSymbolAddress(s)
					);
	};

};

class DynamicSymbolSection : public SymbolSection{
public:
	DynamicSymbolSection(ElfFileBase *e,ICVMGenericFile *r,
			char *tname,int codetype,int sindex):
		SymbolSection(e,r,tname,codetype,sindex){};
	virtual int StringCodeType(void){return SH_DYNSTR;};
};




class CodeSection : public ElfSection{
public:
	CodeSection(ElfFileBase *e,ICVMGenericFile *r,char *tname,
			int codetype,int sindex):
		ElfSection(e,r,tname,codetype,sindex){
#ifdef NOT_NOW
//fprintf(stderr,"PROGBITS=%s size=%d offset=%d flags=$%x type=%d\n",name,sh_size,sh_offset,sh_flags,sh_type);
			long *l=new long[(sh_size+3)>>2];
			if(l){
				if(sh_type==SHT_PROGBITS){
					r->SetAbsPos(sh_offset);
					r->Read(l,sh_size);
				}else // bss section coming in
					memset((void *)l,0,(sh_size+3)>>2);
			}
			data=(void *)l;
					
#endif
	};

	virtual void InitData(ICVMGenericFile *r,char *basemem,u32 baseaddr){
//fprintf(stderr,"PROGBITS=%s size=%d offset=%d flags=$%x type=%d\n",name,sh_size,sh_offset,sh_flags,sh_type);
		data=(void *)(basemem+sh_addr-baseaddr);
		if(sh_type!=SHT_PROGBITS)
			memset((void *)data,0,sh_size);
	};


	virtual void GetMemoryNeeds(size_t &memsz,size_t &filesz,u32 baseaddr,u32 baseoffs){
		size_t fileneed,memneed;
		if(sh_type==SHT_PROGBITS){
			if((sh_offset-baseoffs) != (sh_addr-baseaddr)){
				fprintf(stderr,"FATAL: Non-contiguous executable\n");
				exit(3);
			}
			if(sh_offset<baseoffs  || sh_addr<baseaddr){
				fprintf(stderr,"FATAL: Malformed executable\n");
				exit(3);
			}
			fileneed=sh_offset-baseoffs+sh_size;
			if(fileneed>filesz)
				filesz=fileneed;
		}
		memneed=sh_addr-baseaddr+sh_size;
		if(memneed>memsz)
			memsz=memneed;
	};
													

	// find section  address as classified by input elf address, if possible
	// if not, hand off to Next, or return 0

	virtual p32 GetSectionAddressFromElf(p32 addr){
		if(addr>=sh_addr && addr< (sh_addr+sh_size)){
//printf("ADDRESS RESOLVED - section=%d\n",shindex);

			return (addr-sh_addr)+(p32) Data();
		}
		if(next)
			return next->GetSectionAddressFromElf(addr);
		return 0;

	};




// adjust base address of this section from the offset section of ARel

	inline void AdjustRelAOffsets(RelA *arel,int anx){
		RelA *rel=arel;
		int nx=anx;
		p32 newaddr=(p32)Data();
		p32 addr;
		p32 l;
		int cnt=0;
		if(rel==0 || nx==0)
			return;
		while(cnt<nx){
			addr=rel->r_offset-sh_addr;
			if(addr<0 || addr>=sh_size){
				fprintf(stderr,"Fatal: AdjustRelOffsets Sanity Check Failure\n");
				exit(3);
			}
			addr+=newaddr;
			rel->r_offset=addr;
			++cnt;
			++rel;
		}	
		PerformLocalLink(arel,anx);
	};

	virtual void FinishInit(void){
		ElfSection *e;
		switch(GetCodeType()){ // find the relocations for this item

			case SH_TEXT:
				e=ebase->GetTextRelocSection();
				if(e)
					AdjustRelAOffsets(e->Relocations(),e->NumRelocations()); // correct incoming addresses
				break;


			case SH_DATA:
				e=ebase->GetDataRelocSection();
				if(e)
					AdjustRelAOffsets(e->Relocations(),e->NumRelocations()); // correct incoming addresses
				break;

			case SH_RDATA:
				e=ebase->GetRDataRelocSection();
				if(e)
					AdjustRelAOffsets(e->Relocations(),e->NumRelocations()); // correct incoming addresses
				break;

			case SH_CTOR:
				e=ebase->GetCtorRelocSection();
				if(e)
					AdjustRelAOffsets(e->Relocations(),e->NumRelocations()); // correct incoming addresses
				if(Data()) // set number of init functions to call
					WDu32(Iptr(Data()),(sh_size/4)-1,0);
				else
					fprintf(stderr,"FATAL: Could not init constructors\n");
				break;


			case SH_DTOR:
				e=ebase->GetDtorRelocSection();
				if(e)
					AdjustRelAOffsets(e->Relocations(),e->NumRelocations()); // correct incoming addresses
				if(Data()) // set number of init functions to call
					WDu32(Iptr(Data()),(sh_size/4)-1,0);
				else
					fprintf(stderr,"FATAL: Could not init destructors\n");

				break;

		}
	};

	inline void PerformLocalLink(RelA *rel,int nx){
		DynamicSymbolSection *dss=(DynamicSymbolSection *)ebase->GetDynamicSymbolSection();
		Symbol *sy;
		p32 newaddr=(p32)Data();
		p32 addr;
		p32 l;
		int cnt=0;
		int n;
		while(cnt<nx){
			addr=rel->r_offset;

//fprintf(stderr,"Type $%x RELA_TYPE=%d offset=$%x addend=$%x ",rel->r_info,RELA_TYPE(rel->r_info),rel->r_offset,rel->r_addend);
//fprintf(stderr,"\t\taddress=$%x GetU32(addr0=$%x addjust=$%x\n",addr,GetU32(addr),GetU32(addr)-rel->r_addend);
// for type $16 ----
			switch(n=RELA_TYPE(rel->r_info)){


				case R_ICVM_PC32:
				case R_ICVM_32:
nodynamic:				
					if(dss==0){
						fprintf(stderr,"Do Dynamic Symbol Section in LocalLink\n");
						exit(3);
					}
					sy=dss->Symbols(RELA_INDEX(rel->r_info));
					if(sy==0){
nosym:
						fprintf(stderr,"Invalid Symbol in LocalLink\n");	
						exit(3);
					};
dss->Print(sy);
					l=dss->GetSymbolAddress(sy);
					if(l==0){
badaddr:
						fprintf(stderr,"Bad Address in LocalLink - %s\n",dss->GetSymbolName(sy));	
printf("Bad Address in LocalLink - %s\n",dss->GetSymbolName(sy));	
break;
						exit(3);
					}
					l+=RCu32(addr,0);
					if(n==R_ICVM_PC32){
						l-=addr;
						l-=rel->r_addend;
					}
					WCu32(addr,l,0);
					break;					
				case R_ICVM_RELATIVE:
					l=ConvertFromElfAddress(RCu32(addr,0));
					if(l==0){
						fprintf(stderr,"Bad Address in LocalLink\n");	
printf("Bad Address in LocalLink\n");	
break;
						exit(3);
					}
					WCu32(addr,l,0);	
					break;


				default:
					fprintf(stderr,"Illegal RELA value=%d\n",RELA_TYPE(rel->r_info));
					exit(3);
			}

			++cnt;
			++rel;
		}	
	};


};


class CtorSection : public CodeSection{
public:
	CtorSection(ElfFileBase *e,ICVMGenericFile *r,char *tname,
			int codetype,int sindex): 
			CodeSection(e,r,tname,codetype,sindex){
#ifdef NOT_NOW
		s32 *p=(s32 *)Data();
		if(p) // set number of init functions to call
			*p=(sh_size/4)-1;
#endif
	};

};




#define EI_NIDENT	32

class ElfFile : public ElfFileBase{
	ElfSection *shdrs;
	char *strings; // global string table
	void *basemem; // memory base for executable
public:

	ElfFile(void) : ElfFileBase(){
		shdrs=0;
		strings=0;
	};



	~ElfFile(void){
		DeleteSHDRS();
	};


	virtual void DeleteSHDRS(void){
		ElfSection *p,*n;
		p=shdrs;
		while(p){
			n=p->Next();
			delete p;
			p=n;
		}
		shdrs=0;
	};

	virtual void Print(void){
		ElfSection *p;
		p=shdrs;
		while(p){
			p->Print();
			p=p->Next();
		}
	};




// **** this stuff is the physical file format

	unsigned char	e_ident[EI_NIDENT];
	unsigned char	e_signature[256];
	u16	e_type;
	u16	e_machine;
	u32	e_version;
	p32	e_entry;  /* Entry point */
	u32	e_phoff;
	u32	e_shoff;
	u32	e_flags;
	u16	e_ehsize;
	u16	e_phentsize;
	u16	e_phnum;
	u16	e_shentsize;
	u16	e_shnum;
	u16	e_shstrndx;

//***** end physical format

// general code catagorization

	inline int GetCodeType(char *name){
#ifdef BIGENDIAN
		if(strcmp(name,".btext")==0)
			return SH_TEXT;
		if(strcmp(name,".bdata")==0)
			return SH_DATA;
		if(strcmp(name,".brdata")==0)
			return SH_RDATA;
		if(strcmp(name,".bctors")==0)
			return SH_CTOR;
		if(strcmp(name,".bdtors")==0)
			return SH_DTOR;
		if(strcmp(name,".rela.btext")==0)
			return SH_RELATEXT;
		if(strcmp(name,".rela.bdata")==0)
			return SH_RELADATA;
		if(strcmp(name,".rela.brdata")==0)
			return SH_RELARDATA;
		if(strcmp(name,".rela.bctors")==0)
			return SH_RELACTOR;
		if(strcmp(name,".rela.bdtors")==0)
			return SH_RELADTOR;
		if(strcmp(name,".bbss")==0)
			return SH_BSS;
#else
		if(strcmp(name,".ltext")==0)
			return SH_TEXT;
		if(strcmp(name,".ldata")==0)
			return SH_DATA;
		if(strcmp(name,".lrdata")==0)
			return SH_RDATA;
		if(strcmp(name,".lctors")==0)
			return SH_CTOR;
		if(strcmp(name,".ldtors")==0)
			return SH_DTOR;
		if(strcmp(name,".rela.ltext")==0)
			return SH_RELATEXT;
		if(strcmp(name,".rela.ldata")==0)
			return SH_RELADATA;
		if(strcmp(name,".rela.lrdata")==0)
			return SH_RELARDATA;
		if(strcmp(name,".rela.lctors")==0)
			return SH_RELACTOR;
		if(strcmp(name,".rela.ldtors")==0)
			return SH_RELADTOR;
		if(strcmp(name,".lbss")==0)
			return SH_BSS;
#endif

		if(strcmp(name,".dynsym")==0)
			return SH_DYNSYM;
		if(strcmp(name,".dynstr")==0)
			return SH_DYNSTR;
		if(strcmp(name,".strtab")==0)
			return SH_STRTAB;
		if(strcmp(name,".shstrtab")==0)
			return SH_SHSTRTAB;
//printf("%s CodeType encountered but not acked\n",name);
		return SH_UNDEFINED;
	};


	void Read(ICVMGenericFile *r,char *filename){
		int cnt;
		int codetype;
		ElfSection *sh;
		char name[16];
		r->Read(e_ident,sizeof(e_ident));
		r->Read(e_signature,sizeof(e_signature));
		r->Read(e_type);
		r->Read(e_machine);
		r->Read(e_version);
		r->Read(e_entry);
		r->Read(e_phoff);
		r->Read(e_shoff);
		r->Read(e_flags);
		r->Read(e_ehsize);
		r->Read(e_phentsize);
		r->Read(e_phnum);
		r->Read(e_shentsize);
		r->Read(e_shnum);
		r->Read(e_shstrndx);
		if(r->Error())
			return;
// now read in shdr's
//printf("Reading section %d headers at %d size\n",e_shnum,e_shentsize); 
		cnt=0;
		while(!r->Error() && cnt<e_shnum){
			r->SetAbsPos(e_shoff + (e_shentsize *cnt));
			r->Read((void *)name,sizeof(name));
			codetype=GetCodeType(name);

			switch(codetype){

#ifdef BIGENDIAN
				case SH_TEXT:
				case SH_DATA:
				case SH_RDATA:
				case SH_BSS:
					sh=new CodeSection(this,r,name,codetype,cnt);
					if(!sh)
						r->Error(1);
					break;

				case SH_CTOR:
				case SH_DTOR:
					sh=new CtorSection(this,r,name,codetype,cnt);
					if(!sh)
						r->Error(1);
					break;


				case SH_RELATEXT:
				case SH_RELADATA:
				case SH_RELARDATA:
				case SH_RELACTOR:
				case SH_RELADTOR:
					sh=new RelocationSection(this,r,name,codetype,cnt);
					if(!sh)
						r->Error(1);
					break;


#else
				case SH_TEXT:
				case SH_DATA:
				case SH_RDATA:
				case SH_BSS:
					sh=new CodeSection(this,r,name,codetype,cnt);
					if(!sh)
						r->Error(1);
					break;


				case SH_CTOR:
				case SH_DTOR:
					sh=new CtorSection(this,r,name,codetype,cnt);
					if(!sh)
						r->Error(1);
					break;


				case SH_RELATEXT:
				case SH_RELADATA:
				case SH_RELARDATA:
				case SH_RELACTOR:
				case SH_RELADTOR:
					sh=new RelocationSection(this,r,name,codetype,cnt);
					if(!sh)
						r->Error(1);
					break;



#endif

				case SH_DYNSYM:
					sh=new DynamicSymbolSection(this,r,name,codetype,cnt);
					if(!sh)
						r->Error(1);
					break;


#ifdef NOT_NOW
				case SH_STRTAB:
				case SH_SHSTRTAB:
#endif

				case SH_DYNSTR:
					sh=new StringSection(this,r,name,codetype,cnt);
					if(!sh)
						r->Error(1);
					break;



				default:
					sh=0;
			}
			if(sh)
				sh->Link(&shdrs);
			++cnt;
		}

		sh=shdrs; // calculate memory needs
		size_t memsize=0;
		size_t filesize=0;
		ElfSection *cs=GetTextSection();
		ElfSection *ds=GetDataSection();
		if(cs==0){
			fprintf(stderr,"FATAL: No Text Section\n");
			exit(3);
		}
		if(ds==0){
			fprintf(stderr,"FATAL: No Data Section\n");
			exit(3);
		}

		filesize=0;
		memsize=0;
		while(sh){
			sh->GetMemoryNeeds(memsize,filesize,cs->sh_addr,cs->sh_offset);
			sh=sh->Next();
		}

#ifdef USEMMAP
		int fd=open(filename,O_RDWR);
		if(fd<0){
			fprintf(stderr,"FATAL:Could Not open file %s for mapping\n",filename);
			exit(3);
		}
		basemem=mmap(0,memsize,
			PROT_READ | PROT_WRITE,MAP_PRIVATE,fd,cs->sh_offset);
		if(basemem==MAP_FAILED){
			fprintf(stderr,"FATAL: Could not map file\n");
			exit(3);
		}
		close(fd);
#else
#ifdef USEZEROMMAP

#ifdef NOT_NOW
#ifdef UNIXSYSTEM
// try to alloc memory from mmap so we can apply protections
		int fd=open("/dev/zero",O_RDWR);
		if(fd>=0){
			basemem=mmap(0,memsize,
				PROT_READ | PROT_WRITE,MAP_PRIVATE,fd,0);
			if(basemem==MAP_FAILED)
				basemem=0;
		close(fd);
		}else{
			basemem=mmap(0,memsize,
				PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANON,-1,0);
			if(m==MAP_FAILED)
				basemem=0;
		}
		if(basemem==0)
#endif // UNIXSYSTEM

#endif
		basemem=malloc(memsize);
#endif // NOT_NOW

		basemem=i_sysalloc(memsize);

		if(basemem==0){
			fprintf(stderr,"FATAL: Out of memory\n");
			exit(3);
		}
		
	// read in data for entire file
		r->SetAbsPos(cs->sh_offset);
		r->Read(basemem,filesize);
#endif
		sh=shdrs; // pull in data where needed
		while(sh){
			sh->InitData(r,(char *)basemem,cs->sh_addr);
			sh=sh->Next();
		}


		sh=shdrs; // now finish initializations

		while(sh){
			sh->FinishInit();
			sh=sh->Next();
		}

#if  defined(USEMMAP) || defined(USEZEROMMAP)

#ifdef NOT_NOW
#ifndef __WIN32__
		// read protect first part of memory
		// NOTE - MAX ALIGNMENT here is 8092
		mprotect(basemem,
			ds->sh_addr-cs->sh_addr,PROT_READ);

#endif
#endif // NOT_NOW

		i_sysprotect(basemem,
			ds->sh_addr-cs->sh_addr,1);


#endif

	};	

	inline u32 GetEntryPoint(void){
		u32 e=0;
		ElfSection *s=GetTextSection();
		if(s)
			e=e_entry-s->BaseAddress();
		return e;
	};


	virtual p32 ConvertFromElfAddress(p32 addr){
		return shdrs->GetSectionAddressFromElf(addr);
	};

	virtual void *GetTextSectionData(void){
		ElfSection *e=GetTextSection();
		if(e)
			return e->Data();
		return 0;
	};


};





#ifdef NOT_NOW

#define EM_M32   1
#define EM_SPARC 2
#define EM_386   3
#define EM_68K   4
#define EM_88K   5
#define EM_486   6   /* Perhaps disused */
#define EM_860   7

#define EM_MIPS		8	/* MIPS R3000 (officially, big-endian only) */

#define EM_MIPS_RS4_BE 10	/* MIPS R4000 big-endian */

#define EM_PARISC      15	/* HPPA */

#define EM_SPARC32PLUS 18	/* Sun's "v8plus" */

#define EM_PPC	       20	/* PowerPC */

#define EM_SPARCV9     43	/* SPARC v9 64-bit */

/*
 * This is an interim value that we will use until the committee comes
 * up with a final number.
 */
#define EM_ALPHA	0x9026

#endif // NOT_NOW





/* These constants define the various ELF target machines */
#define EM_NONE  0






/* This is the info that is needed to parse the dynamic section of the file */
#define DT_NULL		0
#define DT_NEEDED	1
#define DT_PLTRELSZ	2
#define DT_PLTGOT	3
#define DT_HASH		4
#define DT_STRTAB	5
#define DT_SYMTAB	6
#define DT_RELA		7
#define DT_RELASZ	8
#define DT_RELAENT	9
#define DT_STRSZ	10
#define DT_SYMENT	11
#define DT_INIT		12
#define DT_FINI		13
#define DT_SONAME	14
#define DT_RPATH 	15
#define DT_SYMBOLIC	16
#define DT_REL	        17
#define DT_RELSZ	18
#define DT_RELENT	19
#define DT_PLTREL	20
#define DT_DEBUG	21
#define DT_TEXTREL	22
#define DT_JMPREL	23
#define DT_LOPROC	0x70000000
#define DT_HIPROC	0x7fffffff

/* This info is needed when parsing the symbol table */
#define STB_LOCAL  0
#define STB_GLOBAL 1
#define STB_WEAK   2

#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4

#define ELF32_ST_BIND(x) ((x) >> 4)
#define ELF32_ST_TYPE(x) (((unsigned int) x) & 0xf)

/* Symbolic values for the entries in the auxiliary table
   put on the initial stack */
#define AT_NULL   0	/* end of vector */
#define AT_IGNORE 1	/* entry should be ignored */
#define AT_EXECFD 2	/* file descriptor of program */
#define AT_PHDR   3	/* program headers for program */
#define AT_PHENT  4	/* size of program header entry */
#define AT_PHNUM  5	/* number of program headers */
#define AT_PAGESZ 6	/* system page size */
#define AT_BASE   7	/* base address of interpreter */
#define AT_FLAGS  8	/* flags */
#define AT_ENTRY  9	/* entry point of program */
#define AT_NOTELF 10	/* program is not ELF */
#define AT_UID    11	/* real uid */
#define AT_EUID   12	/* effective uid */
#define AT_GID    13	/* real gid */
#define AT_EGID   14	/* effective gid */
#define AT_PLATFORM 15  /* string identifying CPU for optimizations */
#define AT_HWCAP  16    /* arch dependent hints at CPU capabilities */

typedef struct dynamic{
  s32 d_tag;
  union{
    s32	d_val;
    p32	d_ptr;
  } d_un;
} Elf32_Dyn;



/* The following are used with relocations */
#define ELF32_R_SYM(x) ((x) >> 8)
#define ELF32_R_TYPE(x) ((x) & 0xff)



typedef struct elf32_rel {
  p32	r_offset;
  u32	r_info;
} Elf32_Rel;




typedef struct elf32_sym{
  u32	st_name;
  p32	st_value;
  u32	st_size;
  unsigned char	st_info;
  unsigned char	st_other;
  u16	st_shndx;
} Elf32_Sym;









#define	ELFCLASSNONE	0		/* EI_CLASS */
#define	ELFCLASS32	1
#define	ELFCLASS64	2
#define	ELFCLASSNUM	3

#define ELFDATANONE	0		/* e_ident[EI_DATA] */
#define ELFDATA2LSB	1
#define ELFDATA2MSB	2

#define EV_NONE		0		/* e_version, EI_VERSION */
#define EV_CURRENT	1
#define EV_NUM		2

/* Notes used in ET_CORE */
#define NT_PRSTATUS	1
#define NT_PRFPREG	2
#define NT_PRPSINFO	3
#define NT_TASKSTRUCT	4

/* Note header in a PT_NOTE section */
typedef struct elf32_note {
  u32	n_namesz;	/* Name size */
  u32	n_descsz;	/* Content size */
  u32	n_type;		/* Content type */
} Elf32_Nhdr;



extern Elf32_Dyn _DYNAMIC [];
#define elf_note	elf32_note


#endif /* ICVM_ELF_H */


