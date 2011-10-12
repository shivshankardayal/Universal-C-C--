INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
cfgetispeed.o \
cfgetospeed.o \
cfmakeraw.o \
cfsetispeed.o \
cfsetospeed.o \
cfsetspeed.o \
tcdrain.o \
tcflow.o \
tcflush.o \
tcgetattr.o \
tcgetpgrp.o \
tcgetsid.o \
tcsendbreak.o \
tcsetattr.o \
tcsetpgrp.o \

all: ${OBJ} termios.a

termios.a: ${OBJ}
	touch termios.a
	rm termios.a
	${AR} termios.a ${OBJ}


clean:
	rm *.o *.a


 