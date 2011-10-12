
ICVMSRC = icvmvm.c icvmsys.c icvmxsys.c icvmfile.c icvmerr.c icvmnet.c \
	icvmfast.c icvmid.c icvmstring.c icvmproc.c icvmopt.c icvmsup.c

# NOTE - ordering IS important here.
ICVMOBJS = icvmsup.o icvmvm.o  \
	icvmsup1.o\
	icvmsup2.o\
	icvmsup3.o\
	icvmsup4.o\
	icvmsup5.o\
	icvmsup6.o\
	icvmsup7.o\
	icvmsup8.o\
	icvmsup9.o\
	icvmsup10.o\
	icvmsup11.o\
	icvmsup12.o\
	icvmsup13.o\
	icvmfile.o icvmerr.o icvmnet.o \
	file.o \
	sys.o \
	proc.o \
	icvm.o icvmid.o icvmproc.o \
	icvmfast.o\
	icvmstring.o icvmsys.o icvmxsys.o\
	icvmopengl.o

ADDLLIBS=-L/usr/X11R6/lib -lGL -lGLU -lglut -lICE   -lX11 -lm 


all:  icvm 


#################################################
# NOTE - My builds are all done with GCC-2.95.2
# IF you have problems, update your compiler!!!
#################################################


# choose your compiler

# Generic builds - these build the slow VM's

#CC = cc -O2  ${ALIGN}
#CC = gcc -O2  ${ALIGN}

# generic pentium build - small, slow version
CC = gcc  -O2 ${ALIGN} -ffixed-%ebx -DHW_LM  -fomit-frame-pointer -fno-exceptions

# generic pentium build - big fast version
CC = gcc  -O2 ${ALIGN} -ffixed-%ebx -DHW_LM -DBIGANDFAST  -fomit-frame-pointer -fno-exceptions

# OPTIMIZED WINDOWS
#CC = i586-mingw32-gcc  -O2 -ffixed-%ebx -DHW_LM\
#	-fomit-frame-pointer\
#	-mcpu=k6\
#	-DBIGANDFAST\
#	-Iinclude.ms\
#	 -I/usr/local/crossx/i586-mingw32/sys-include \
# 	 -L/usr/local/crossx/i586-mingw32/lib\
# 	-fno-exceptions 
#ADDLLIBS=  -Llib.ms -lglut32 -lopengl32

# NON-OPTIMIZED WINDOWS
#CC = i586-mingw32-gcc  -O2 -ffixed-%ebx -DHW_LM\
#	-fomit-frame-pointer\
#	-Iinclude.ms\
#	 -I/usr/local/crossx/i586-mingw32/sys-include \
# 	 -L/usr/local/crossx/i586-mingw32/lib\
# 	-fno-exceptions 
#ADDLLIBS=  -Llib.ms -lglut32 -lopengl32


# ----   intel Linux/Unix GCC native builds - build the FAST VM's -------

# 386, 486, or old-gcc compiler native build
# use this if your compiler does not support -mcpu=k6 or -mpentium
#CC = gcc -O2 -fomit-frame-pointer -ffixed-%ebx -DHW_LM -fno-exceptions  -DBIGANDFAST

# pentium pro, pentium II, pentium III - native build
#CC = gcc -O2 -fomit-frame-pointer -ffixed-%ebx -DHW_LM -fno-exceptions -mpentiumpro -DBIGANDFAST

# pentium - native build 
#CC = gcc -O2 -fomit-frame-pointer -ffixed-%ebx -DHW_LM -fno-exceptions -mpentium -DBIGANDFAST
#CC = gcc -O2 -fomit-frame-pointer -ffixed-%ebx -DHW_LM -fno-exceptions -mpentium

# AMD K6  - native build
# @@@@@@@@@@@@@@@@@@@@@@@@
#CC = gcc   -O2 -DUSE3DNOW -ffixed-%ebx -DHW_LM -fomit-frame-pointer -fno-exceptions -mcpu=k6 -DBIGANDFAST
#CC = gcc   -O2 -DUSEMMX  -ffixed-%ebx -DHW_LM -fomit-frame-pointer -fno-exceptions -mcpu=k6 -DBIGANDFAST
#CC = gcc   -O2  -ffixed-%ebx -DHW_LM -fomit-frame-pointer -fno-exceptions -mcpu=k6 -DBIGANDFAST
#CC = gcc   -O2  -ffixed-%ebx -DHW_LM -DNOPTIMIZE -fomit-frame-pointer -fno-exceptions -mcpu=k6 -DBIGANDFAST
#CC = gcc -O2 -DUSEMMX -DUSE3DNOW -ffixed-%ebx -DHW_LM -fomit-frame-pointer -fno-exceptions -mcpu=k6 
#CC = gcc -O2 -ffixed-%ebx -DHW_LM -fomit-frame-pointer -fno-exceptions -mcpu=k6 

# *************8 DEBUG VERSION ONLY ********************
#CC = gcc   -O2 -DUSE3DNOW -ffixed-%ebx -DHW_LM -fomit-frame-pointer -fno-exceptions -mcpu=k6 -DBIGANDFAST
#LIBOPTS=


# links into turning on disassembler - see cvmdis.c and bfdstuff.h 
# to use this
LIBOPTS=-Lbinutils/libiberty  -Lbinutils/opcodes  -lopcodes -liberty



# 68030 - debian m68k linux - native compile
#CC = gcc -O2 -fomit-frame-pointer -fno-exceptions\
# -m68030 -DBIGENDIAN -DM68K


# -- powerpc -- MKLINUX - 601 - native compile
#CC = gcc -O2 -fomit-frame-pointer -fno-exceptions\
#	-mcpu=601 -DBIGENDIAN -Dmklinux -fsigned-char  


#############################
# VARIOUS COMPILER OPTIONS - IMPORTANT
#############################

# comment this in for Solaris 7 builds
#LIBOPTS=-lposix4 ## solaris needs these

#comment the ALIGN in if you need strict natural alignment to 32 bits, - 64 bit items
# can fall on 32 bit boundarys
#ALIGN=-DICVM_NEEDALIGN32

#comment the ALIGN in if you need strict natural alignment to 64 bits
# SPARC,MIPS,ALPHA - COMMENT THIS IN
#ALIGN=-DICVM_NEEDALIGN64



########################################################
###### CROSS COMPILE SECTION ###########################
########################################################

#############################################
# THE FOLLOWING ARE MY CROSS-COMPILE BUILDS
# LEFT HERE FOR MY OWN USE, OR YOURS, IF YOU LIKE
#
# some files will be missing. They are not necessary to produce a working VM.
# The copy readmes, etc, and build a compressed tar image.
# So modify and use, if you like. We do not support the following material,
# and it is only here to allow us to mass create our builds, 
# and perhaps to give you some ideas on how we created our builds. 
#############################################

### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
## NEED TO ADD -ffixed-%ebx -DHW_LM to all 86 builds
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[
### %%%%%%%%%%%% ]]]]]] [[[[[[[[  ]]]]]]]]]]]]]p       ]]]]]]]]][[[[[[

# cross-compiled (on my system) high speed versions
# you should modify these cross-compiler versions for your own setup
# also you should remove the the @n and space that precedes each line
# UNSUPPORTED so do not ask me about these!!!


# glibc21 linux pentium pro, pentium II, pentium III - native build
#0 CC = gcc -O2 -fomit-frame-pointer -ffixed-%ebx  -fno-exceptions -mpentiumpro -DBIGANDFAST
cross/icvm_linux_glibc21_pentiumII.tar.gz:
	sed -e "s/#@0 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_linux_glibc21_pentiumII.tar.gz

# glibc21 linux pentium  - native build
#00 CC = gcc -O2 -fomit-frame-pointer -ffixed-%ebx  -fno-exceptions -mpentium -DBIGANDFAST
cross/icvm_linux_glibc21_pentium.tar.gz:
	sed -e "s/#@00 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_linux_glibc21_pentium.tar.gz


# glibc21 linux K6  - native build
#01 CC = gcc -O2 -fomit-frame-pointer -ffixed-%ebx  -fno-exceptions -mcpu=k6 -DBIGANDFAST
cross/icvm_linux_glibc21_k6.tar.gz:
	sed -e "s/#@01 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_linux_glibc21_k6.tar.gz


# 68030 debian m68k linux  - crosscompile
#@1 CC = m68k-linux-gcc  -O2 -fomit-frame-pointer\
#@1 	 -I/usr/local/crossx/m68k-linux/sys-include \
#@1 	 -L/usr/local/crossx/m68k-linux/lib\
#@1 	-fno-exceptions -DM68K -m68030 -DBIGENDIAN -DBIGANDFAST 

cross/icvm_linux_debian_m68k.tar.gz:
	sed -e "s/#@1 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_linux_debian_m68k.tar.gz

# 68030 m68k netbsd  - crosscompile
#@10 CC = m68k-netbsd-gcc  -O2 -fomit-frame-pointer\
#@10 	 -I/usr/local/crossx/m68k-netbsd/sys-include \
#@10 	 -L/usr/local/crossx/m68k-netbsd/lib\
#@10 	-fno-exceptions -DM68K -m68030 -DBIGENDIAN -DBIGANDFAST  
cross/experimental/icvm_netbsd_mac_m68k.tar.gz:
	sed -e "s/#@10 //" <Makefile >Makefile.cross
	make buildexperimentalcrosstgz
	mv cross.tgz cross/experimental/icvm_netbsd_mac_m68k.tar.gz



# -- powerpc -- MKLINUX - 601 - cross compile
#@2 CC = powerpc-unknown-linux-gnulibc1-gcc -O2 -fomit-frame-pointer -fno-exceptions\
#@2 	 -I/usr/local/crossx/powerpc-unknown-linux-gnulibc1/sys-include \
#@2 	 -L/usr/local/crossx/powerpc-unknown-linux-gnulibc1/lib\
#@2 	-mcpu=601 -DBIGENDIAN -Dmklinux -fsigned-char -DBIGANDFAST  

cross/icvm_mklinux_dr3_ppc.tar.gz:
	sed -e "s/#@2 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_mklinux_dr3_ppc.tar.gz


# -- solaris 7 X86 Pentium Pro/II compile

#@3 CC = i586-solaris-gcc -O2 -ffixed-%ebx  -fomit-frame-pointer -fno-exceptions\
#@3 	 -I/usr/local/crossx/i586-solaris/sys-include \
#@3 	 -L/usr/local/crossx/i586-solaris/lib\
#@3 	-mpentiumpro  -DBIGANDFAST  
#@3 LIBOPTS=-lposix4 ## solaris needs these

cross/icvm_solaris7_pentiumII.tar.gz:
	sed -e "s/#@3 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_solaris7_pentiumII.tar.gz

# -- solaris 7 X86 Pentium compile

#@30 CC = i586-solaris-gcc -O2 -ffixed-%ebx  -fomit-frame-pointer -fno-exceptions\
#@30 	 -I/usr/local/crossx/i586-solaris/sys-include \
#@30 	 -L/usr/local/crossx/i586-solaris/lib\
#@30 	-mpentium  -DBIGANDFAST  
#@30 LIBOPTS=-lposix4 ## solaris needs these

cross/icvm_solaris7_pentium.tar.gz:
	sed -e "s/#@30 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_solaris7_pentium.tar.gz

# -- solaris 7 X86 K6 compile

#@31 CC = i586-solaris-gcc -O2 -ffixed-%ebx  -fomit-frame-pointer -fno-exceptions\
#@31 	 -I/usr/local/crossx/i586-solaris/sys-include \
#@31 	 -L/usr/local/crossx/i586-solaris/lib\
#@31 	-mcpu=k6  -DBIGANDFAST  
#@31 LIBOPTS=-lposix4 ## solaris needs these

cross/icvm_solaris7_k6.tar.gz:
	sed -e "s/#@31 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_solaris7_k6.tar.gz

# -- freebsd X86 Pentium Pro/II compile

#@4 CC = i586-freebsdelf-gcc -O2 -ffixed-%ebx  -fomit-frame-pointer -fno-exceptions\
#@4 	 -I/usr/local/crossx/i586-freebsdelf/sys-include \
#@4 	 -L/usr/local/crossx/i586-freebsdelf/lib\
#@4 	-mpentiumpro  -DBIGANDFAST  

cross/icvm_freebsd_pentiumII.tar.gz:
	sed -e "s/#@4 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_freebsd_pentiumII.tar.gz

# -- freebsd X86 Pentium  compile

#@40 CC = i586-freebsdelf-gcc -O2 -ffixed-%ebx  -fomit-frame-pointer -fno-exceptions\
#@40 	 -I/usr/local/crossx/i586-freebsdelf/sys-include \
#@40 	 -L/usr/local/crossx/i586-freebsdelf/lib\
#@40 	-mpentium  -DBIGANDFAST  

cross/icvm_freebsd_pentium.tar.gz:
	sed -e "s/#@40 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_freebsd_pentium.tar.gz


# -- freebsd X86 K6  compile

#@41 CC = i586-freebsdelf-gcc -O2 -ffixed-%ebx  -fomit-frame-pointer -fno-exceptions\
#@41 	 -I/usr/local/crossx/i586-freebsdelf/sys-include \
#@41 	 -L/usr/local/crossx/i586-freebsdelf/lib\
#@41 	-mcpu=k6  -DBIGANDFAST  

cross/icvm_freebsd_k6.tar.gz:
	sed -e "s/#@41 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_freebsd_k6.tar.gz

# -- netbsd X86 Pentium Pro/II compile

#@5 CC = i586-netbsd-gcc -O2 -ffixed-%ebx  -fomit-frame-pointer -fno-exceptions\
#@5 	 -I/usr/local/crossx/i586-netbsd/sys-include \
#@5 	 -L/usr/local/crossx/i586-netbsd/lib\
#@5 	-mpentiumpro  -DBIGANDFAST   

cross/icvm_netbsd_pentiumII.tar.gz:
	sed -e "s/#@5 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_netbsd_pentiumII.tar.gz


# -- netbsd X86 Pentium  compile

#@50 CC = i586-netbsd-gcc -O2 -ffixed-%ebx  -fomit-frame-pointer -fno-exceptions\
#@50 	 -I/usr/local/crossx/i586-netbsd/sys-include \
#@50 	 -L/usr/local/crossx/i586-netbsd/lib\
#@50 	-mpentium  -DBIGANDFAST  

cross/icvm_netbsd_pentium.tar.gz:
	sed -e "s/#@50 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_netbsd_pentium.tar.gz


# -- netbsd X86 K6  compile

#@51 CC = i586-netbsd-gcc -O2 -ffixed-%ebx  -fomit-frame-pointer -fno-exceptions\
#@51 	 -I/usr/local/crossx/i586-netbsd/sys-include \
#@51 	 -L/usr/local/crossx/i586-netbsd/lib\
#@51 	-mcpu=k6  -DBIGANDFAST  

cross/icvm_netbsd_k6.tar.gz:
	sed -e "s/#@51 //" <Makefile >Makefile.cross
	make buildcrosstgz
	mv cross.tgz cross/icvm_netbsd_k6.tar.gz




cross:
	mkdir cross
	mkdir cross/experimental

buildcross: cross
	make -if Makefile.cross clean
	make -f Makefile.cross
	mv icvm icvm.bin
	mkdir icvm
	mv icvm.bin icvm/icvm
	cp ../VERSION.txt icvm
	cp ../VISION.txt icvm
	cp ../WHAT.txt icvm
	cp ../TODO.txt icvm
	cp INSTALL.bin.nodist icvm/INSTALL.txt
	cp install.sh icvm
	cp COPYRIGHT.txt icvm/COPYRIGHT.txt


buildcrosstgz: buildcross
	tar -cvzf cross.tgz icvm
	rm Makefile.cross
	rm -rf icvm	

buildexperimentalcrosstgz: buildcross
	cp README.experimental.nodist icvm/READMEFIRST.txt
	tar -cvzf cross.tgz icvm
	rm Makefile.cross
	rm -rf icvm	





allcross:  \
 cross/icvm_mklinux_dr3_ppc.tar.gz\
 cross/icvm_linux_debian_m68k.tar.gz\
 cross/icvm_solaris7_pentium.tar.gz\
 cross/icvm_solaris7_pentiumII.tar.gz\
 cross/icvm_solaris7_k6.tar.gz\
 cross/icvm_freebsd_pentium.tar.gz\
 cross/icvm_freebsd_pentiumII.tar.gz\
 cross/icvm_freebsd_k6.tar.gz\
 cross/icvm_netbsd_pentium.tar.gz\
 cross/icvm_netbsd_pentiumII.tar.gz\
 cross/icvm_netbsd_k6.tar.gz\
 cross/experimental/icvm_netbsd_mac_m68k.tar.gz\
 cross/icvm_linux_glibc21_pentiumII.tar.gz\
 cross/icvm_linux_glibc21_pentium.tar.gz\
 cross/icvm_linux_glibc21_k6.tar.gz\




######################################################################
######################################################################
########  End Cross-compile Section ##################################
######################################################################
######################################################################

SUPOPTS=  -c  

.c.o:
	${CC} ${ALIGN} -c $*.c

.cc.o:
	${CC} ${ALIGN} -c $*.cc



clean:
	rm -rf *.o icvm

realclean: clean
	rm  cross/*.gz cross/experimental/*.gz

icvm : ${INC} icvmload.h  ${ICVMOBJS}
	${CC}    \
		 ${ICVMOBJS} \
		${ADDLLIBS} \
		${LIBOPTS} -o icvm

install: icvm
	cp icvm /bin

icvmsup.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP $*.c -o $*.o

icvmsup1.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP1 icvmsup.c -o icvmsup1.o

icvmsup2.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP2 icvmsup.c -o icvmsup2.o

icvmsup3.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP3 icvmsup.c -o icvmsup3.o

icvmsup4.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP4 icvmsup.c -o icvmsup4.o

icvmsup5.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP5 icvmsup.c -o icvmsup5.o

icvmsup6.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP6 icvmsup.c -o icvmsup6.o

icvmsup7.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP7 icvmsup.c -o icvmsup7.o

icvmsup8.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP8 icvmsup.c -o icvmsup8.o

icvmsup9.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP9 icvmsup.c -o icvmsup9.o

icvmsup10.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP10 icvmsup.c -o icvmsup10.o

icvmsup11.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP11 icvmsup.c -o icvmsup11.o

icvmsup12.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP12 icvmsup.c -o icvmsup12.o

icvmsup13.o: icvmsup.c
	${CC} ${SUPOPTS} -DICVMSUP13 icvmsup.c -o icvmsup13.o


