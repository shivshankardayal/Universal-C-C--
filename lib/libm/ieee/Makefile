																																																																				
NOOPTCC=igcc ${INCLUDES}   -c 
CC=igcc ${INCLUDES}   -O2  -c
AR=iar -qc 



.c.o:
	${CC}  $*.c

OBJ=\
cabs.o \
cbrt.o \
support.o \


all: ${OBJ} 


ieee.a: ${OBJ}
	touch ieee.a
	rm ieee.a
	${AR} ieee.a ${OBJ}

clean:
	rm *.o *.a

