																																		
																																																																				
CC=igcc    -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
	atan2.o\
	sincos.o\
	tan.o

all: ${OBJ} 

common.a: ${OBJ}
	touch common.a
	rm common.a
	${AR} common.a ${OBJ}

clean:
	rm *.o *.a

 