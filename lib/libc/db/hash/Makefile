INC1=-I../../../../include
INC2=-I../../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} -D__DBINTERFACE_PRIVATE $*.c

OBJ =\
hash.o \
hash_bigkey.o \
hash_buf.o \
hash_func.o \
hash_log2.o \
hash_page.o \
hsearch.o \
ndbm.o \

all: ${OBJ} hash.a

hash.a: ${OBJ}
	touch hash.a
	rm hash.a
	${AR} hash.a ${OBJ}


clean:
	rm *.o *.a


 