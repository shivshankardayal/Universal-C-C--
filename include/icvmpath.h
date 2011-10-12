#ifndef ICVM_PATH_H
#define ICVM_PATH_H

/* the macro to define how to find the rootpath for most PATH directories */

extern char *__icvmpath(char *s);
extern char *__geticvmbasepath(void);

#define ICVMPATH(s) __icvmpath(s)

#endif




