INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
xdryp.o \
yp_all.o \
yp_first.o \
yp_maplist.o \
yp_master.o \
yp_match.o \
yp_order.o \
yperr_string.o \
yplib.o \
ypprot_err.o \

all: ${OBJ} yp.a

yp.a: ${OBJ}
	touch yp.a
	rm yp.a
	${AR} yp.a ${OBJ}


clean:
	rm *.o *.a


 