#ifndef THREAD_H
#define THREAD_H

#include "sys.h"
#include "proc.h"

// Copyright (C) 2000,2001 Bob Dayley


// defines threadsafe vars such as errno etc.
typedef struct i_threadvars{
	s32 terrno;
	s32 therrno;
}i_threadvars;

typedef struct i_thread{
	struct i_thread *next; // next in list
	struct i_thread *snext; // scheduler list
	struct i_thread *mainthread; // main thread
	struct i_threadvars vars; // threadsafe variables
	void *stack; // allocated stack
	u32 stacksize; // size of allocated stack
	struct ICVM_jmp_buf context;

#ifdef __cplusplus
	inline void SaveContext(void){i_savecontext(&context,0);};
	inline void RestoreContext(void){i_restorecontext(&context);};
#endif // __cplusplus

}i_thread;

typedef struct i_threadlist{
	i_thread *threads; // list of threads running in this system
	i_thread *currentthread;
}i_threadlist;

#endif

