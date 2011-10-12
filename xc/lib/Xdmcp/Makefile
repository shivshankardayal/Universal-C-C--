																																		
																																																																				
CC=igcc  -O2  \
	-I../../../include/X11\
	-I../../../include/X11/PEX5\
	-I../../../include/X11/extensions\
	   -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
A8Eq.o \
AA16.o \
AA32.o \
AA8.o \
Alloc.o \
AofA8.o \
CA8.o \
CmpKey.o \
DA16.o \
DA32.o \
DA8.o \
DAofA8.o \
DecKey.o \
Fill.o \
Flush.o \
GenKey.o \
IncKey.o \
RA16.o \
RA32.o \
RA8.o \
RAofA8.o \
RC16.o \
RC32.o \
RC8.o \
RHead.o \
RR.o \
RaA16.o \
RaA32.o \
RaA8.o \
RaAoA8.o \
Unwrap.o \
WA16.o \
WA32.o \
WA8.o \
WAofA8.o \
WC16.o \
WC32.o \
WC8.o \
Whead.o \
Wrap.o \


all: ${OBJ} libXdmcp.a

libXdmcp.a: ${OBJ} dummy.o
	touch libXdmcp.a
	rm libXdmcp.a
	${AR} libXdmcp.a dummy.o

clean:
	rm *.o *.a

install: all
	cp libXdmcp.a /usr/local/internetc/lib

 