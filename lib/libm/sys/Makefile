INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																																																					
CC=igcc ${INCLUDES}  -O2  -c -nostdinc
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ =\
 sys.o\


all: ${OBJ} 

clean:
	rm *.o *.a

 