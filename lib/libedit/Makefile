							
CC=igcc ${INCLUDES}  -O2  -c 
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
editline.o \

all: ${OBJ} libedit.a

libedit.a: ${OBJ}
	touch libedit.a
	rm libedit.a
	${AR} libedit.a ${OBJ}


clean:
	rm *.o *.a

install: libedit.a
	cp libedit.a /usr/local/internetc/lib
 