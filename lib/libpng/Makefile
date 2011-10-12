INCLUDES= -I../zlib
CC=igcc ${INCLUDES}  -O2  -c
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
png.o \
pngerror.o \
pnggccrd.o \
pngget.o \
pngmem.o \
pngpread.o \
pngread.o \
pngrio.o \
pngrtran.o \
pngrutil.o \
pngset.o \
pngtrans.o \
pngvcrd.o \
pngwio.o \
pngwrite.o \
pngwtran.o \
pngwutil.o \

all: ${OBJ} libpng.a

libpng.a: ${OBJ}
	touch libpng.a
	rm libpng.a
	${AR} libpng.a ${OBJ}


clean:
	rm *.o *.a

install: libpng.a
	cp png*.h /usr/local/internetc/include
	cp libpng.a /usr/local/internetc/lib
 