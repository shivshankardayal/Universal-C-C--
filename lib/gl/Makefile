
#	quad/*.o\

AR=iar -qc 
RANLIB=iranlib 
LD=ild
CC=igcc -O2  -c 


all: libgl.a libglu.a libglut.a

libgl.a: glsys.o
	touch libgl.a 
	rm libgl.a
	${AR} libgl.a glsys.o
	${RANLIB} libgl.a

libglu.a: glusys.o
	touch libglu.a 
	rm libglu.a
	${AR} libglu.a glusys.o
	${RANLIB} libglu.a

libglut.a: glutsys.o
	touch libglut.a 
	rm libglut.a
	${AR} libglut.a glutsys.o
	${RANLIB} libglut.a


clean:
	rm *.a *.o

install: libgl.a libglu.a libglut.a
#	cp libgl.a libglu.a libglut.a  /usr/local/internetc/lib
	cp libgl.a libglut.a  /usr/local/internetc/lib

