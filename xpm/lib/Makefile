																																																																				
CC=igcc    -I.  -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
Attrib.o \
CrBufFrI.o \
CrBufFrP.o \
CrDatFrI.o \
CrDatFrP.o \
CrIFrBuf.o \
CrIFrDat.o \
CrIFrP.o \
CrPFrBuf.o \
CrPFrDat.o \
CrPFrI.o \
Image.o \
Info.o \
RdFToBuf.o \
RdFToDat.o \
RdFToI.o \
RdFToP.o \
WrFFrBuf.o \
WrFFrDat.o \
WrFFrI.o \
WrFFrP.o \
create.o \
data.o \
hashtab.o \
misc.o \
parse.o \
rgb.o \
simx.o \
scan.o \

all: ${OBJ} libXpm.a

libXpm.a: ${OBJ}
	touch libXpm.a
	rm libXpm.a
	${AR} libXpm.a ${OBJ}

clean:
	rm *.o *.a

install: all
	cp xpm.h /usr/local/internetc/include/X11
	touch /usr/local/internetc/include/xpm.h
	rm /usr/local/internetc/include/xpm.h
	ln -s /usr/local/internetc/include/X11/xpm.h /usr/local/internetc/include/xpm.h
	cp libXpm.a /usr/local/internetc/lib
 
