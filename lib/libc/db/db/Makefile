INC1=-I../../../../include
INC2=-I../../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} -D__DBINTERFACE_PRIVATE $*.c

OBJ =\
db.o \

all: ${OBJ} db.a

db.a: ${OBJ}
	touch db.a
	rm db.a
	${AR} db.a ${OBJ}


clean:
	rm *.o *.a


 