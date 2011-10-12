INC1=-I../../../../include
INC2=-I../../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
mpool.o \

all: ${OBJ} mpool.a

mpool.a: ${OBJ}
	touch mpool.a
	rm mpool.a
	${AR} mpool.a ${OBJ}


clean:
	rm *.o *.a


 