																																																																				
CC=igcc     -I../../../include/X11\
	-I../../../include/X11/extensions -I../X11  -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
DPMS.o \
MITMisc.o \
XAppgroup.o \
XEVI.o \
XLbx.o \
XMultibuf.o \
XPanoramiX.o \
XSecurity.o \
XShape.o \
XShm.o \
XSync.o \
XTestExt1.o \
Xcup.o \
Xdbe.o \
extutil.o \
globals.o \


##aixlcLoad.o \ aix specific

all: ${OBJ} libXext.a

libXext.a: ${OBJ} dummy.o
	touch libXext.a
	rm libXext.a
	${AR} libXext.a dummy.o

clean:
	rm *.o *.a

install: all
	cp libXext.a /usr/local/internetc/lib
 