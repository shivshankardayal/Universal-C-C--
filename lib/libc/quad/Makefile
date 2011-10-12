INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
																				
																																																																																															
CC=igcc  ${INCLUDES} -O2  -c -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
adddi3.o \
anddi3.o \
ashldi3.o \
ashrdi3.o \
cmpdi2.o \
divdi3.o \
fixdfdi.o \
fixsfdi.o \
fixunsdfdi.o \
fixunssfdi.o \
floatdidf.o \
floatdisf.o \
floatunsdidf.o \
iordi3.o \
lshldi3.o \
lshrdi3.o \
moddi3.o \
muldi3.o \
negdi2.o \
notdi2.o \
subdi3.o \
ucmpdi2.o \
udivdi3.o \
umoddi3.o \
xordi3.o \
qdivrem.o 


all: ${OBJ} quad.a

quad.a: ${OBJ}
	touch quad.a
	rm quad.a
	${AR} quad.a ${OBJ}

clean:
	rm *.o *.a

