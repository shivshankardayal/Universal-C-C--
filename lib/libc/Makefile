LIBS=\
	stdlib/*.o\
	stdio/*.o\
	string/*.o\
	gen/*.o\
	net/*.o\
	rpc/*.o\
	locale/*.o\
	time/*.o\
	md/*.o\
	regex/*.o\
	sys/*.o\
	yp/*.o\
	nls/*.o\
	hash/*.o\
	termios/*.o\
	db/*/*.o\
	wcsmbs/*.o\


#	quad/*.o\

AR=iar -qc 
RANLIB=iranlib 
LD=ild


all: libc.a libc.so

checkall:
	cd stdlib;make
	cd stdio;make
	cd string;make
	cd quad;make
	cd gen;make
	cd net;make
	cd rpc;make
	cd locale;make
	cd time;make
	cd md;make
	cd regex;make
	cd sys;make
	cd yp;make
	cd nls;make
	cd hash;make
	cd termios;make
	cd db;make
	cd wcsmbs;make

libc.a: checkall
	touch libc.a libc.so
	rm libc.a libc.so
	${AR} libc.a ${LIBS}
	${RANLIB} libc.a

libc.so: libc.a
	${LD} --shared --whole-archive  -x -X -L. -lc -o libc.la
	mv libc.la libc.so 

clean:
	cd stdlib;make -i  clean
	cd stdio;make -i  clean
	cd string;make -i  clean
	cd quad;make -i  clean
	cd gen;make -i  clean
	cd net;make -i  clean
	cd rpc;make -i  clean
	cd locale;make -i  clean
	cd time;make -i  clean
	cd md;make -i  clean
	cd regex;make -i  clean
	cd sys;make -i  clean
	cd yp;make -i  clean
	cd nls;make -i  clean
	cd hash;make -i  clean
	cd termios;make -i  clean
	cd db;make -i  clean
	cd wcsmbs;make -i clean
	rm *.a *.so

install: libc.a libc.so
#	touch /usr/local/internetc/lib/libc.a
#	rm /usr/local/internetc/lib/libc.a
#	cp libc.so /usr/local/internetc/lib
	cp libc.a  /usr/local/internetc/lib

