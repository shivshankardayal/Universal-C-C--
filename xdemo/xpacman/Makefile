																																																																				
CC=igcc -O2   -c 
	    

AR=iar -qc 




.c.o:
	${CC}   $*.c

OBJ=\
xpacman.o \


all: xpacman

xpacman: $(OBJ) 
	igcc $(OBJ)  -lXaw  -lXmu -lXext -lXp\
		   -lSM -lICE  -lX11  -lm  -o xpacman 




clean:
	rm *.o *.a

 