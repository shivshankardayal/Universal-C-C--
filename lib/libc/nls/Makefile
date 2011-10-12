INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
_catclose.o \
_catgets.o \
_catopen.o \
catclose.o \
catgets.o \
catopen.o \

all: ${OBJ} nls.a

nls.a: ${OBJ}
	touch nls.a
	rm nls.a
	${AR} nls.a ${OBJ}


clean:
	rm *.o *.a


 