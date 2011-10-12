LD=ild


all: linksdone include buildlibs

buildlibs: 
	cd csu.icvm;make
	cd libc;make
	cd libm;make
	cd libstdc++;make
	cd libutil;make
	cd libedit;make
	cd libterm;make
	cd zlib;make
	cd libpng;make
	cd jpeg;make
	cd libtiff;make
	cd xc;make
	cd xpm;make
	cd gl;make
	cd mesa;make

clean: 
	cd csu.icvm;make -i clean
	cd libc;make -i clean
	cd libm;make -i clean
	cd libstdc++;make -i clean
	cd libutil;make -i clean
	cd libedit;make -i clean
	cd libterm;make -i clean
	cd zlib;make -i clean
	cd libpng;make -i clean
	cd jpeg;make -i clean
	cd libtiff;make -i clean
	cd xc;make -i clean
	cd xpm;make -i clean
	cd gl;make -i clean
	cd mesa;make -i clean

install: 
	cd csu.icvm;make install
	cd libc;make install
	cd libm;make install
	cd libstdc++;make install
	cd libutil;make install
	cd libedit;make install
	cd libterm;make install
	cd zlib;make install
	cd libpng;make install
	cd jpeg;make install
	cd libtiff;make install
	cd xc;make install
	cd xpm;make install
	cd gl;make install
	cd mesa;make install

# NOTE these links are local to my own screwed up CVS tree.
# the directories they refer to should already be present in your build
# as also the file linksdone
linksdone:
	touch linksdone
	ln -s ../xpm .
	ln -s ../xc .
	
include:
	ln -s ../include include

