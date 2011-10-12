																																																																				
CC=igcc   -I../../../include  -I../../../include/X11\
	-I../../../include/X11/extensions   -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
XRecord.o \
XTest.o



all: ${OBJ} libXtst.a

libXtst.a: ${OBJ} dummy.o
	touch libXtst.a
	rm libXtst.a
	${AR} libXtst.a dummy.o

clean:
	rm *.o *.a

install: all
	cp libXtst.a /usr/local/internetc/lib
 