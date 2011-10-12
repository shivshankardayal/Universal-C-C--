INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
regcomp.o \
regerror.o \
regexec.o \
regfree.o \

all: ${OBJ} regex.a

regex.a: ${OBJ}
	touch regex.a
	rm regex.a
	${AR} regex.a ${OBJ}


clean:
	rm *.o *.a


 