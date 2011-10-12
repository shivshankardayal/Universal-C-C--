INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
_def_messages.o \
_def_monetary.o \
_def_numeric.o \
_def_time.o \
ctypeio.o \
localeconv.o \
nl_langinfo.o \
setlocale.o \


all: ${OBJ} locale.a

locale.a: ${OBJ}
	touch locale.a
	rm locale.a
	${AR} locale.a ${OBJ}

clean:
	rm *.o *.a

 