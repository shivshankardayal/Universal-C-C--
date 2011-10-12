// Instantiation file for the -*- C++ -*- Standard Library allocator templates
// This file is part of the GNU ANSI C++ Library.

#ifdef INTERNETC
#define __DECLARE_GLOBALS_HERE
#endif

#include <alloc.h>

#ifndef __USE_MALLOC
template class __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0>;
#endif

template class  __malloc_alloc_template<0>;
