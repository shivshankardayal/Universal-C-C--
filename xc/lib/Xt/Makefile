																																		
																																																																				

#CC=echeck -I../../../include/X11\
	-I../../../include/X11/extensions  -O2  -c

CC=igcc -I../../../include/X11\
	-I../../../include/X11/extensions  -O2  -c

AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
ActionHook.o \
Alloc.o \
ArgList.o \
Callback.o \
ClickTime.o \
Composite.o \
Constraint.o \
Convert.o \
Converters.o \
Core.o \
Create.o \
Destroy.o \
Display.o \
Error.o \
Event.o \
EventUtil.o \
Functions.o \
GCManager.o \
Geometry.o \
GetActKey.o \
GetResList.o \
GetValues.o \
HookObj.o \
Hooks.o \
Initialize.o \
Intrinsic.o \
Keyboard.o \
Manage.o \
NextEvent.o \
Object.o \
PassivGrab.o \
Pointer.o \
Popup.o \
PopupCB.o \
RectObj.o \
ResConfig.o \
Resources.o \
Selection.o \
SetSens.o \
SetValues.o \
SetWMCW.o \
Shell.o \
TMaction.o \
TMgrab.o \
TMkey.o \
TMparse.o \
TMprint.o \
TMstate.o \
Threads.o \
VarCreate.o \
VarGet.o \
Varargs.o \
Vendor.o \
strings.o \
sharedlib.o \

all: ${OBJ} libXt.a

libXt.a: ${OBJ}
	touch libXt.a
	rm libXt.a
	${AR} libXt.a ${OBJ}

clean:
	rm *.o *.a

install: all
	cp libXt.a /usr/local/internetc/lib



 