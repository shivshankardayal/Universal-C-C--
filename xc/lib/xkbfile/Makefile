																																																																				
CC=igcc   -I../../../include  -I../../../include/X11\
	-I../../../include/X11/extensions   -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
cout.o \
maprules.o \
srvmisc.o \
xkbatom.o \
xkbbells.o \
xkbconfig.o \
xkbdraw.o \
xkberrs.o \
xkbmisc.o \
xkbout.o \
xkbtext.o \
xkmout.o \
xkmread.o \


all: ${OBJ} libxkbfile.a

libxkbfile.a: ${OBJ} dummy.o
	touch libxkbfile.a
	rm libxkbfile.a
	${AR} libxkbfile.a dummy.o

clean:
	rm *.o *.a

install: all
	cp libxkbfile.a /usr/local/internetc/lib
 