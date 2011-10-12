																																																																				
CC=igcc     -I../../../include\
     -I../../../include/X11\
	-I../../../include/X11/extensions -I../X11  -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
abort.o \
await.o \
clientdata.o \
colorlist.o \
conven.o \
dummy.o \
elements.o \
errors.o \
events.o \
free.o \
ieee.o \
init.o \
lut.o \
photoflo.o \
photomap.o \
photospace.o \
roi.o \
technique.o \
utils.o 


all: ${OBJ} libXIE.a

libXIE.a: ${OBJ} dummy.o
	touch libXIE.a
	rm libXIE.a
	${AR} libXIE.a dummy.o

clean:
	rm *.o *.a

install: all
	cp libXIE.a /usr/local/internetc/lib
 