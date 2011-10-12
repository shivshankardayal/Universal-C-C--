INC1=-I../../../../include
INC2=-I../../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} -D__DBINTERFACE_PRIVATE $*.c

OBJ =\
rec_close.o \
rec_delete.o \
rec_get.o \
rec_open.o \
rec_put.o \
rec_search.o \
rec_seq.o \
rec_utils.o \


all: ${OBJ} recno.a

recno.a: ${OBJ}
	touch recno.a
	rm recno.a
	${AR} recno.a ${OBJ}


clean:
	rm *.o *.a


 