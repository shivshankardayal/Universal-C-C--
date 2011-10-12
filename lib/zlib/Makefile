							
CC=igcc ${INCLUDES}  -O2  -c 
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
adler32.o \
compress.o \
crc32.o \
deflate.o \
gzio.o \
infblock.o \
infcodes.o \
inffast.o \
inflate.o \
inftrees.o \
infutil.o \
trees.o \
uncompr.o \
zutil.o \

all: ${OBJ} libz.a

libz.a: ${OBJ}
	touch libz.a
	rm libz.a
	${AR} libz.a ${OBJ}


clean:
	rm *.o *.a

install: libz.a
	cp z*.h /usr/local/internetc/include
	cp libz.a /usr/local/internetc/lib
 