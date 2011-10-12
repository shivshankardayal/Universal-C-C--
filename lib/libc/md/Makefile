INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
md4c.o \
md4hl.o \
md5c.o \
md5hl.o 

all: ${OBJ} md.a

md.a: ${OBJ}
	touch md.a
	rm md.a
	${AR} md.a ${OBJ}


clean:
	rm *.o *.a


 