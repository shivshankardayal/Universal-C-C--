INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
							
																												
CC=igcc ${INCLUDES}  -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
asctime.o \
ftime.o \
difftime.o \
ialloc.o \
localtime.o \
scheck.o \
strftime.o \
strptime.o \

#zdump.o \
#zic.o 

all: ${OBJ} time.a

time.a: ${OBJ}
	touch time.a
	rm time.a
	${AR} time.a ${OBJ}


clean:
	rm *.o *.a

nsparser.o: nsparser.y
	yacc -p_nsyy -bnsparser -d nsparser.y
	mv nsparser.tab.c nsparser.c
	mv nsparser.tab.h nsparser.h
	${CC} $*.c nsparser.c


nslexer.o: nslexer.l nsparser.h
	lex -P_nsyy -otmp.lex  nslexer.l 
	cat tmp.lex | sed -e '/YY_BUF_SIZE/s/16384/1024/' >nslexer.c
	rm tmp.lex
	${CC} $*.c nslexer.c

 