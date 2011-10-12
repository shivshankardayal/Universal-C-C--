																																																																				
CC=igcc   	-I../../../include  -I../../../include/X11\
	-I../../../include/X11/fonts \
	-I../../../include/X11/extensions   -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
FSClServ.o \
FSCloseFt.o \
FSConnServ.o \
FSErrDis.o \
FSErrHndlr.o \
FSFlush.o \
FSFontInfo.o \
FSFtNames.o \
FSGetCats.o \
FSListCats.o \
FSListExt.o \
FSMisc.o \
FSNextEv.o \
FSOpenFont.o \
FSOpenServ.o \
FSQGlyphs.o \
FSQXExt.o \
FSQXInfo.o \
FSQuExt.o \
FSServName.o \
FSSetCats.o \
FSSync.o \
FSSynchro.o \
FSlibInt.o 



all: ${OBJ} libFS.a

libFS.a: ${OBJ}
	touch libFS.a
	rm libFS.a
	${AR} libFS.a ${OBJ}

clean:
	rm *.o *.a

install: all
	cp libFS.a /usr/local/internetc/lib
 