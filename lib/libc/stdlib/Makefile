
CC=igcc  ${INCLUDES} -O2  -c
AR=iar -qc 

.c.o:
	${CC} $*.c

OBJ =\
abort.o \
abs.o \
atof.o \
atexit.o \
atoi.o \
atol.o \
bsearch.o \
calloc.o \
div.o \
exit.o \
getenv.o \
getopt.o \
heapsort.o \
labs.o \
ldiv.o \
multibyte.o \
putenv.o \
qsort.o \
radixsort.o \
rand.o \
random.o \
setenv.o \
strtod.o \
strtoul.o \
strtol.o \
strtoq.o \
_strtoq.o \
system.o \
strtouq.o \
_strtouq.o \
_rand48.o \
drand48.o \
erand48.o \
jrand48.o \
lcong48.o \
lrand48.o \
mrand48.o \
nrand48.o \
seed48.o \
srand48.o \


		
all: ${OBJ} stdlib.a

stdlib.a: ${OBJ}
	touch stdlib.a
	rm stdlib.a
	${AR} stdlib.a ${OBJ}

clean:
	rm *.o *.a

 