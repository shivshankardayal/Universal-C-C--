																																		
CC=igcc ${INCLUDES}   -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
	acosh.o\
	asinh.o\
	atanh.o\
	cosh.o\
	erf.o\
	expm1.o\
	gamma.o\
	j0.o\
	j1.o\
	jn.o\
	lgamma.o\
	log1p.o\
	sinh.o\
	tanh.o\

#	log__L.o\
#	pow.o\
#	log.o\
#	log10.o\
#	floor.o\
#	fmod.o\
#	asincos.o\
#	atan.o\
#	exp.o\
#	exp__E.o\
	



all: ${OBJ} 

common.a: ${OBJ}
	touch common.a
	rm common.a
	${AR} common.a ${OBJ}

clean:
	rm *.o *.a

 