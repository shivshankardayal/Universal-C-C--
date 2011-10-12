CC=igcc ${INCLUDES}  -O2  -c 
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
termcap.o \
tgoto.o \
tputs.o \

all: ${OBJ} libterm.a

libterm.a: ${OBJ}
	touch libterm.a
	rm libterm.a
	${AR} libterm.a ${OBJ}


clean:
	rm *.o *.a

install: libterm.a
	cp libterm.a /usr/local/internetc/lib



 