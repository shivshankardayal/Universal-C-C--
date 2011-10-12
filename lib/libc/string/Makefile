INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
									
																													
CC=igcc  ${INCLUDES}  -O2 -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
bcopy.o \
bzero.o \
bm.o \
strcoll.o \
strmode.o \
strsep.o \
strxfrm.o \
__strerror.o \
__strsignal.o


#strftime.o \

all: ${OBJ} string.a

string.a: ${OBJ}
	touch string.a
	rm string.a
	${AR} string.a ${OBJ}


clean:
	rm *.o *.a

 