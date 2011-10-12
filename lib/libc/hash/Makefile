INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
sha1.o \
sha1hl.o \


all: ${OBJ} hash.a

hash.a: ${OBJ}
	touch hash.a
	rm hash.a
	${AR} hash.a ${OBJ}


clean:
	rm *.o *.a


 