		
CC=igcc   -O2 -c  
CPP=igcc  -fexceptions  -O2 -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

.cc.o:
	${CPP} $*.cc

OBJ =\
 sys.o\
 sbrk.o \
 eh.o\
 new.o\
 new1.o\
 new2.o\
 pvirtual.o\
 exception.o\
 tinfo.o\
 tinfo2.o\
 ctors.o\


all: ${OBJ} sys.a

sys.a: ${OBJ}
	touch sys.a
	rm sys.a
	${AR} sys.a ${OBJ}

clean:
	rm *.o *.a

 
