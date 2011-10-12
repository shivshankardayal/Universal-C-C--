
CC=igcc  ${INCLUDES} -O2   -c 
AR=iar -qc 

.c.o:
	${CC} $*.c

OBJ =\
btowc.o \
mbrlen.o \
mbrtowc.o \
mbsinit.o \
mbsnrtowcs.o \
mbsrtowcs.o \
wcpcpy.o \
wcpncpy.o \
wcrtomb.o \
wcscat.o \
wcschr.o \
wcscmp.o \
wcscpy.o \
wcscspn.o \
wcsdup.o \
wcslen.o \
wcsncat.o \
wcsncmp.o \
wcsncpy.o \
wcsnrtombs.o \
wcspbrk.o \
wcsrchr.o \
wcsrtombs.o \
wcsspn.o \
wcsstr.o \
wctob.o \
wmemchr.o \
wmemcmp.o \
wmemcpy.o \
wmemmove.o \
wmemset.o 

#wcswidth.o \ missing stuff
#wcwidth.o \ missing stuff
# LIB conflicts below - does not do things quite the way we doe
#wcstok.o \
#wcstod.o \
#wcstof.o \
#wcstol.o \
#wcstold.o \
#wcstoq.o \
#wcstoul.o \
#wcstouq.o \
#wcscoll.o \
#wcsxfrm.o \
		
all: ${OBJ}


clean:
	rm *.o *.a

 