
#include "gbl-ctors.h"

void
__do_global_dtors ()
{
  int *pcnt=(int *)__DTOR_LIST__;
  int cnt=0;	 
  func_ptr *p = __DTOR_LIST__ + 1;
  while (cnt<*pcnt){
	  if(*p)
      	(*(p)) ();
	  ++cnt;
	  ++p;
    }
}

/* Run all the global constructors on entry to the program.  */



void __do_global_ctors ()
{

  int *pcnt=(int *)__CTOR_LIST__;
  int cnt=0;	 
  func_ptr *p = __CTOR_LIST__ + 1;
  while (cnt<*pcnt){
	 if(*p)
      	(*(p)) ();
	  ++cnt;
	  ++p;
    }
  atexit(__do_global_dtors);
}

