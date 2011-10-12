							
																												
CC=igcc ${INCLUDES}  -O2  -c  
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
fparseln.o \
getmaxpartitions.o \
getrawpartition.o \
login.o \
login_tty.o \
logout.o \
logwtmp.o \
opendisk.o \
passwd.o \
pidlock.o \
pty.o \
ttyaction.o \
ttymsg.o \

all: ${OBJ} libutil.a

libutil.a: ${OBJ}
	touch libutil.a
	rm libutil.a
	${AR} libutil.a ${OBJ}


clean:
	rm *.o *.a

install: libutil.a
	cp libutil.a /usr/local/internetc/lib

 