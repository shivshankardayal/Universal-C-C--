INC1=-I../../include
INC2=-I../../include/sys
INCLUDES= ${INC1} ${INC2}
														
																																																					
CC=igcc ${INCLUDES}  -O   -c -nostdinc 
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ =\
 crt0.o


all: ${OBJ} 

clean:
	rm *.o *.a

install: ${OBJ}
	cp ${OBJ} /usr/local/internetc/lib



 