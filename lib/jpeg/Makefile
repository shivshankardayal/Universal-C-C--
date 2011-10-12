							
CC=igcc ${INCLUDES}  -O2  -c
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
jcapimin.o \
jcapistd.o \
jccoefct.o \
jccolor.o \
jcdctmgr.o \
jchuff.o \
jcinit.o \
jcmainct.o \
jcmarker.o \
jcmaster.o \
jcomapi.o \
jcparam.o \
jcphuff.o \
jcprepct.o \
jcsample.o \
jctrans.o \
jdapimin.o \
jdapistd.o \
jdatadst.o \
jdatasrc.o \
jdcoefct.o \
jdcolor.o \
jddctmgr.o \
jdhuff.o \
jdinput.o \
jdmainct.o \
jdmarker.o \
jdmaster.o \
jdmerge.o \
jdphuff.o \
jdpostct.o \
jdsample.o \
jerror.o \
jfdctflt.o \
jfdctfst.o \
jfdctint.o \
jidctflt.o \
jidctfst.o \
jidctint.o \
jidctred.o \
jmemansi.o \
jmemmgr.o \
jmemname.o \
jmemnobs.o \
jpegtran.o \
jquant1.o \
jquant2.o \
jutils.o \
rdbmp.o \
rdcolmap.o \
rdgif.o \
rdppm.o \
rdrle.o \
rdswitch.o \
rdtarga.o \
transupp.o \
wrbmp.o \
wrgif.o \
wrppm.o \
wrrle.o \
wrtarga.o \


all: ${OBJ} libjpeg.a

libjpeg.a: ${OBJ}
	touch libjpeg.a
	rm libjpeg.a
	${AR} libjpeg.a ${OBJ}


clean:
	rm *.o *.a

install: libjpeg.a
	cp jpeglib.h /usr/local/internetc/include
	cp jerror.h /usr/local/internetc/include
	cp jconfig.h /usr/local/internetc/include
	cp jmorecfg.h /usr/local/internetc/include
	cp libjpeg.a /usr/local/internetc/lib
 