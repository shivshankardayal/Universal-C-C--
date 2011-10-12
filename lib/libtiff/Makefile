							
CC=igcc ${INCLUDES} -O2   -c 
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
tif_aux.o \
tif_close.o \
tif_codec.o \
tif_compress.o \
tif_dir.o \
tif_dirinfo.o \
tif_dumpmode.o \
tif_error.o \
tif_fax3.o \
tif_flush.o \
tif_getimage.o \
tif_jpeg.o \
tif_luv.o \
tif_lzw.o \
tif_open.o \
tif_packbits.o \
tif_pixarlog.o \
tif_predict.o \
tif_print.o \
tif_read.o \
tif_strip.o \
tif_swab.o \
tif_thunder.o \
tif_tile.o \
tif_unix.o \
tif_warning.o \
tif_write.o \
tif_zip.o \
tif_dirread.o \
tif_dirwrite.o \
tif_version.o \

all: ${OBJ} libtiff.a

libtiff.a: ${OBJ}
	touch libtiff.a
	rm libtiff.a
	${AR} libtiff.a ${OBJ}


clean:
	rm *.o *.a

install: libtiff.a
	cp tiff.h /usr/local/internetc/include
	cp tiffio.h /usr/local/internetc/include
	cp libtiff.a /usr/local/internetc/lib
 