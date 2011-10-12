
CC=igcc  ${INCLUDES} -O2    -c  -DFLOATING_POINT
AR=iar -qc 



.c.o:
	${CC} $*.c

OBJ =\
asprintf.o \
clrerr.o \
fclose.o \
fdopen.o \
feof.o \
ferror.o \
fflush.o \
fgetc.o \
fgetln.o \
fgetpos.o \
fgets.o \
fileno.o \
findfp.o \
flags.o \
fopen.o \
fprintf.o \
fpurge.o \
fputc.o \
fputs.o \
fread.o \
freopen.o \
fscanf.o \
fseek.o \
fsetpos.o \
ftell.o \
funopen.o \
fvwrite.o \
fwalk.o \
fwrite.o \
getc.o \
getchar.o \
gets.o \
gettemp.o \
getw.o \
makebuf.o \
mkdtemp.o \
mkstemp.o \
mktemp.o \
perror.o \
printf.o \
putc.o \
putchar.o \
puts.o \
putw.o \
refill.o \
remove.o \
rewind.o \
rget.o \
scanf.o \
setbuf.o \
setbuffer.o \
setvbuf.o \
snprintf.o \
sprintf.o \
sscanf.o \
stdio.o \
strerror.o \
tempnam.o \
tmpfile.o \
tmpnam.o \
ungetc.o \
vasprintf.o \
vfprintf.o \
vfscanf.o \
vprintf.o \
vscanf.o \
vsnprintf.o \
vsprintf.o \
vsscanf.o \
wbuf.o \
wsetup.o \

#fgetline.o \


all: ${OBJ} stdio.a

stdio.a: ${OBJ}
	touch stdio.a
	rm stdio.a
	${AR} stdio.a ${OBJ}

clean:
	rm *.o *.a

 