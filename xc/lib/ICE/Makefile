
																																																																				
CC=igcc   -I../../../include  -I../../../include/X11\
	-I../../../include/X11/extensions \
	-I../../../include/X11/ICE   -O2  -c  

AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
accept.o \
authutil.o \
connect.o \
error.o \
getauth.o \
iceauth.o \
listen.o \
listenwk.o \
locking.o \
misc.o \
ping.o \
process.o \
protosetup.o \
register.o \
replywait.o \
setauth.o \
shutdown.o \
watch.o 

all: ${OBJ} libICE.a

libICE.a: ${OBJ} dummy.o
	touch libICE.a
	rm libICE.a
	${AR} libICE.a dummy.o

clean:
	rm *.o  *.a

install: all
	cp libICE.a /usr/local/internetc/lib
 