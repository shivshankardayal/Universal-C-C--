																																																																				
CC=igcc   -I../../../include  -I../../../include/X11\
	-I../../../include/X11/extensions   -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
AuDispose.o \
AuFileName.o \
AuGetAddr.o \
AuGetBest.o \
AuLock.o \
AuRead.o \
AuUnlock.o \
AuWrite.o \
Autest.o 




#k5encode.o \ missing header


##aixlcLoad.o \ aix specific

all: ${OBJ} libXau.a

libXau.a: ${OBJ} dummy.o
	touch libXau.a
	rm libXau.a
	${AR} libXau.a dummy.o

clean:
	rm *.o *.a

install: all
	cp libXau.a /usr/local/internetc/lib
 