																																																																				
CC=igcc   -I../../../include/X11\
	-I../../../include/X11/extensions  -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
XAllowDv.o \
XChgDCtl.o \
XChgFCtl.o \
XChgKMap.o \
XChgKbd.o \
XChgPnt.o \
XChgProp.o \
XCloseDev.o \
XDevBell.o \
XExtInt.o \
XExtToWire.o \
XGMotion.o \
XGetBMap.o \
XGetDCtl.o \
XGetFCtl.o \
XGetKMap.o \
XGetMMap.o \
XGetProp.o \
XGetVers.o \
XGrDvBut.o \
XGrDvKey.o \
XGrabDev.o \
XGtFocus.o \
XGtSelect.o \
XListDev.o \
XOpenDev.o \
XQueryDv.o \
XSelect.o \
XSetBMap.o \
XSetDVal.o \
XSetMMap.o \
XSetMode.o \
XSndExEv.o \
XStFocus.o \
XUngrDev.o \
XUngrDvB.o \
XUngrDvK.o \


#XFreeLst.o \ BUG - UNDEFINED TYPE - THINK THIS MIGHT BE OBSOLETE ANYWAY OR SERVER ORIENTED


all: ${OBJ} libXi.a

libXi.a: ${OBJ} dummy.o
	touch libXi.a
	rm libXi.a
	${AR} libXi.a dummy.o

clean:
	rm *.o *.a

install: all
	cp libXi.a /usr/local/internetc/lib

 
