ICVMLOCALDIR=/usr/local
ICVMDIR=${ICVMLOCALDIR}/internetc

all: makedirs_done makebinutils_done makegcc_done
	./makelocallinks
	./fixdirs
	echo "Build is done"

makedirs_done:
	./makedirs
	touch makedirs_done

confbinutils_done: makedirs_done
	cd binutils;\
	./configure --target=icvm-unknown-elf  --prefix=${ICVMDIR} --program-prefix=i
	touch confbinutils_done


makebinutils_done: confbinutils_done
	cd binutils;\
	make;\
	make install
	./makelocallinks
	touch makebinutils_done


confgcc_done: makebinutils_done
	cd gcc;\
	./configure --with-headers=../../include  --target=icvm-unknown-elf --prefix=${ICVMDIR} --program-prefix=i
	touch confgcc_done

makegcc_done: confgcc_done
	cd gcc;\
	make;\
	make install
	touch makegcc_done

clean:
	cd binutils;make clean
	cd gcc;make clean
	rm *_done

