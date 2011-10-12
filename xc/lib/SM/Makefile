

																																																																				
CC=igcc  -I../../../include  -I../../../include/X11\
	-I../../../include/X11/SM \
	-I../../../include/X11/extensions   -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
sm_auth.o \
sm_client.o \
sm_error.o \
sm_genid.o \
sm_manager.o \
sm_misc.o \
sm_process.o \



all: ${OBJ} libSM.a

libSM.a: ${OBJ} dummy.o
	touch libSM.a
	rm libSM.a
	${AR} libSM.a dummy.o

clean:
	rm *.o *.a

install: all
	cp libSM.a /usr/local/internetc/lib
 