																																																																				
CC=igcc  -I../../../include/X11\
	-I../../../include/X11/PEX5\
	-I../../../include/X11/extensions\
	  -O2  -c  
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ=\
pl_convert.o \
pl_escape.o \
pl_font.o \
pl_free.o \
pl_lut.o \
pl_nameset.o \
pl_oc_attr.o \
pl_oc_dec.o \
pl_oc_enc.o \
pl_oc_prim.o \
pl_oc_str.o \
pl_oc_util.o \
pl_pc.o \
pl_pick.o \
pl_rdr.o \
pl_sc.o \
pl_startup.o \
pl_struct.o \
pl_wks.o \
pl_util.o \

all: ${OBJ} libPEX5.a

libPEX5.a: ${OBJ}
	touch libPEX5.a
	rm libPEX5.a
	${AR} libPEX5.a ${OBJ}

clean:
	rm *.o *.a

install: all
	cp libPEX5.a /usr/local/internetc/lib

 