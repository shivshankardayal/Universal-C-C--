																																																																				
CC=igcc  -I../../../include\
	  -I../../../include/X11/Xaw\
	-I../../../include/X11/extensions  -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
AllWidgets.o \
AsciiSink.o \
AsciiSrc.o \
AsciiText.o \
Box.o \
Command.o \
Dialog.o \
Form.o \
Grip.o \
Label.o \
List.o \
MenuButton.o \
MultiSink.o \
MultiSrc.o \
Paned.o \
Panner.o \
Porthole.o \
Repeater.o \
Scrollbar.o \
Simple.o \
SimpleMenu.o \
Sme.o \
SmeBSB.o \
SmeLine.o \
StripChart.o \
Template.o \
Text.o \
TextAction.o \
TextPop.o \
TextSink.o \
TextSrc.o \
TextTr.o \
Toggle.o \
Tree.o \
Vendor.o \
Viewport.o \
XawI18n.o \
XawIm.o \
XawInit.o \
sharedlib.o \


all: ${OBJ} libXaw.a

libXaw.a: ${OBJ}
	touch libXaw.a
	rm libXaw.a
	${AR} libXaw.a ${OBJ}

clean:
	rm *.o *.a

install: all
	cp libXaw.a /usr/local/internetc/lib

 
