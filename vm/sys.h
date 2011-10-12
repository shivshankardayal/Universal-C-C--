#ifndef SYS_H
#define SYS_H

#define USEGLOBALSTRUCT

// Copyright (C) 2000,2001 Bob Dayley


#ifndef __WIN32__
#define UNIXSYSTEM
#endif // __WIN32__


// ******** time definitions

struct ICVM_timeval{
	s32 tv_sec;
	s32 tv_usec;
};

struct ICVM_timespec{
	s32 tv_sec;
	s32 tv_nsec;
};


struct ICVM_timezone{
	s32 tz_minuteswest;
	s32 tz_dsttime;
};

#ifdef __WIN32__

//#define NANOTIMEOFFSET (0x295e9648864000LL)
#define NANOTIMEOFFSET   (0x19db1ded53e8000LL)

#define FILETIMETONSEC(low,high) (((\
	(((u64) low) & 0xffffffffLL) + (((u64)(high))<<32)\
)-NANOTIMEOFFSET) *100)

#define NSECTOUSEC(x) ((x)/1000LL)
#define NSECTOMSEC(x) ((x)/1000000LL)
#define NSECTOSEC(x) ((x)/1000000000LL)


#endif // __WIN32__

#ifdef __cplusplus
extern "C"{
#endif


#ifdef __WIN32__
extern int i_thisisfork;

#define MASTERMEMSIZE	(1048576)	// sizeo of allocated mem block in 
									// our own allocator
#endif // __WIN32__

#ifdef UNIXSYSTEM
void i_initchildfork(void);
#endif // UNIXSYSTEM

s32 i_initsystem(void);
s32 i_initmemsystem(void);
void *i_malloc(size_t sz);
void i_free(void *v);
void *i_realloc(void *v,size_t sz);
s32 i_gettimeofday(struct ICVM_timeval *tv,struct ICVM_timezone *tz);
s32 i_getpagesize(void);
void *i_sysalloc(u32 memsz);
void i_sysprotect(void *mem,u32 memsz,s32 flag);

// mem protection routines

void i_memreadonly(void *mem,s32 memsz);
void i_memnoaccess(void *mem,s32 memsz);


#ifdef __WIN32__

s32 i_artificialforkchild(s32 in,s32 out);
char *i_getemptystring(void);
#define EMPTYSTRING i_getemptystring()

#else // __WIN32__

#define EMPTYSTRING ""

#endif // __WIN32__


#ifdef __cplusplus
};
#endif





#endif



