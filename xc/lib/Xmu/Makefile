																																																																				
CC=igcc   -I../../../include  -I../../../include/X11\
	-I../../../include/X11/Xmu \
	-I../../../include/X11/extensions   -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
AllCmap.o \
Atoms.o \
ClientWin.o \
CloseHook.o \
CmapAlloc.o \
CrCmap.o \
CrPixFBit.o \
CursorName.o \
CvtCache.o \
CvtStdSel.o \
DefErrMsg.o \
DelCmap.o \
DisplayQue.o \
Distinct.o \
DrRndRect.o \
DrawLogo.o \
EditresCom.o \
ExtAgent.o \
FToCback.o \
GetHost.o \
GrayPixmap.o \
Initer.o \
LocBitmap.o \
Lookup.o \
LookupCmap.o \
Lower.o \
RdBitF.o \
ScrOfWin.o \
ShapeWidg.o \
StdCmap.o \
StrToBS.o \
StrToBmap.o \
StrToCurs.o \
StrToGrav.o \
StrToJust.o \
StrToLong.o \
StrToOrnt.o \
StrToShap.o \
StrToWidg.o \
UpdMapHint.o \
VisCmap.o \
WidgetNode.o \
Xct.o \
sharedlib.o \


all: ${OBJ} libXmu.a

libXmu.a: ${OBJ}
	touch libXmu.a
	rm libXmu.a
	${AR} libXmu.a ${OBJ}

clean:
	rm *.o *.a

install: all
	cp libXmu.a /usr/local/internetc/lib
 