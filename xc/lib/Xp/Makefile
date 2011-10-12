																																		
																																																																				
CC=igcc -I../../../include/X11\
	-I../../../include/X11/extensions  -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
XpAttr.o \
XpContext.o \
XpDoc.o \
XpExtUtil.o \
XpExtVer.o \
XpGetData.o \
XpImageRes.o \
XpInput.o \
XpJob.o \
XpLocale.o \
XpNotifyPdm.o \
XpPage.o \
XpPageDim.o \
XpPrinter.o \
XpPutData.o \
XpScreens.o \


all: ${OBJ} libXp.a

libXp.a: ${OBJ}
	touch libXp.a
	rm libXp.a
	${AR} libXp.a ${OBJ}

clean:
	rm *.o *.a

install: all
	cp libXp.a /usr/local/internetc/lib

 