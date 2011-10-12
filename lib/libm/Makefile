LIBS=\
	sys/*.o\
	ieee/*.o\
	common_source/*.o


AR=iar -qc 
RANLIB=iranlib 



all: libm.a

compileall:
	cd common_source;make
	cd ieee;make
	cd sys;make

libm.a: compileall
	touch libm.a
	rm libm.a
	${AR} libm.a ${LIBS}
	${RANLIB} libm.a

clean:
	cd common_source;make -i clean
	cd ieee;make -i clean
	cd sys;make -i clean
	rm *.a

install: libm.a
	cp libm.a /usr/local/internetc/lib

