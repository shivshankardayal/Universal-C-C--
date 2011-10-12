INC1=-I../../../../include
INC2=-I../../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
NOOPTCC=igcc ${INCLUDES}    -c  -nostdinc
CC=igcc ${INCLUDES} -O2   -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} -D__DBINTERFACE_PRIVATE $*.c

OBJ =\
bt_close.o \
bt_conv.o \
bt_debug.o \
bt_delete.o \
bt_get.o \
bt_open.o \
bt_overflow.o \
bt_page.o \
bt_put.o \
bt_search.o \
bt_seq.o \
bt_split.o \
bt_utils.o \



all: ${OBJ} btree.a

btree.a: ${OBJ}
	touch btree.a
	rm btree.a
	${AR} btree.a ${OBJ}


clean:
	rm *.o *.a

# still spill-over register problem in GCC
bt_seq.o : bt_seq.c
	${NOOPTCC} bt_seq.c


 