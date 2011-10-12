
CC=igcc   -D_LIBC -O2  -c 
CXX=igcc   -D_LIBC -fexceptions  -O2  -c
AR=iar -qc 


.c.o:
	${CC} $*.c

.cc.o:
	${CXX} $*.cc



OBJ =\
PlotFile.o \
SFile.o \
builtinbuf.o \
cleanup.o \
cmathi.o \
cstdlibi.o \
cstringi.o \
editbuf.o \
filebuf.o \
filedoalloc.o \
fileops.o \
floatconv.o \
fstream.o \
genops.o \
indstream.o \
ioassign.o \
ioextend.o \
iofclose.o \
iofdopen.o \
iofeof.o \
ioferror.o \
iofflush.o \
iofflush_u.o \
iofgetpos.o \
iofgets.o \
iofopen.o \
iofprintf.o \
iofputs.o \
iofread.o \
iofscanf.o \
iofsetpos.o \
ioftell.o \
iofwrite.o \
iogetc.o \
iogetdelim.o \
iogetline.o \
iogets.o \
ioignore.o \
iomanip.o \
iopadn.o \
ioperror.o \
iopopen.o \
ioprims.o \
ioprintf.o \
ioputc.o \
ioputs.o \
ioscanf.o \
ioseekoff.o \
ioseekpos.o \
iosetbuffer.o \
iosetvbuf.o \
iosprintf.o \
iosscanf.o \
iostream.o \
iostrerror.o \
ioungetc.o \
iovfprintf.o \
iovfscanf.o \
iovsprintf.o \
iovsscanf.o \
isgetline.o \
isgetsb.o \
isscan.o \
osform.o \
outfloat.o \
parsestream.o \
peekc.o \
pfstream.o \
procbuf.o \
sbform.o \
sbgetline.o \
sbscan.o \
stdiostream.o \
stdstrbufs.o \
stdstreams.o \
stlinst.o \
stream.o \
streambuf.o \
strops.o \
strstream.o \
stdexcepti.o \
scinst_c.o\
scinst_w.o\





## Templates have problems for now, for some reason
#csinst.o  \
#fcinst.o  \
#dcinst.o 

all: ${OBJ} libstdc++.a

libstdc++.a: ${OBJ}
	touch libstdc++.a
	rm libstdc++.a
	${AR} libstdc++.a ${OBJ}


CINSTINC=\
		-DREP  -DADDSS -DADDPS -DADDCS\
		 -DADDSP -DADDSC -DEQSS -DEQPS -DEQSP \
		-DNESS -DNEPS -DNESP -DLTSS -DLTPS -DLTSP\
		 -DGTSS -DGTPS -DGTSP -DLESS -DLEPS -DLESP \
		-DGESS -DGEPS -DGESP -DEXTRACT -DINSERT -DGETLINE




fcinst.o: cinst.cc
	${CXX} -DF $(CINSTINC) \
	cinst.cc -o fcinst.o
	
dcinst.o: cinst.cc
	${CXX} -DD $(CINSTINC) \
	cinst.cc -o dcinst.o


SINSTINC=\
		-DREP -DMAIN -DTRAITS -DADDSS -DADDPS -DADDCS\
		 -DADDSP -DADDSC -DEQSS -DEQPS -DEQSP \
		-DNESS -DNEPS -DNESP -DLTSS -DLTPS -DLTSP -DGTSS\
		-DGTPS -DGTSP -DLESS -DLEPS -DLESP \
		-DGESS -DGEPS -DGESP -DEXTRACT -DINSERT -DGETLINE

csinst.o: sinst.cc
	${CXX} -DC $(CINSTINC) \
	sinst.cc -o csinst.o
	
	
clean:
	rm *.o *.a

install: libstdc++.a
	cp libstdc++.a /usr/local/internetc/lib

SENABLE = -DREP  -DTRAITS -DADDSS -DADDPS -DADDCS -DADDSP -DADDSC \
	   -DEQSS -DEQPS -DEQSP -DNESS -DNEPS -DNESP -DLTSS -DLTPS -DLTSP -DGTSS -DGTPS -DGTSP \
	   -DLESS -DLEPS -DLESP -DGESS -DGEPS -DGESP -DEXTRACT -DINSERT -DGETLINE


scinst_c.o: sinst.cc
	${CXX} -DC -DMAIN $(SENABLE) sinst.cc -o scinst_c.o

scinst_w.o: sinst.cc
	${CXX} -DW $(SENABLE) sinst.cc -o scinst_w.o




 