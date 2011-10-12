																																																																				
CC=igcc  -O2   -c 
CPP=ig++ -O2   -c 
	    





.c.o:
	${CC} $*.c

.cc.o:
	${CPP} $*.cc


EXECS=bounce pendulum atlantis rings gears morph3d steam \
	rollercoaster dragnet2k


all: $(EXECS)


bounce: bounce.o
	igcc -O2  bounce.o \
		 -lglut -lglu -lgl  -lm  -o bounce 

gears: gears.o
	igcc -O2  gears.o \
		 -lglut -lglu -lgl  -lm  -o gears 

morph3d: morph3d.o
	igcc -O2  morph3d.o \
		 -lglut -lglu -lgl  -lm  -o morph3d 

pendulum: pendulum.o
	igcc -O2  pendulum.o \
		 -lglut -lglu -lgl  -lm  -o pendulum 

atlantis: atlantis.o atlantis.h shark.o dolphin.o swim.o whale.o
	igcc -O2  atlantis.o shark.o dolphin.o swim.o whale.o\
		 -lglut -lglu -lgl  -lm  -o atlantis 

rings: rings.o
	igcc -O2  rings.o \
		 -lglut -lglu -lgl  -lm  -o rings 

steam: steam.o
	igcc -O2  steam.o \
		 -lglut -lglu -lgl  -lm  -o steam 

dragnet2k: dragnet2k.o
	igcc -O2  dragnet2k.o \
		 -lglut -lglu -lgl  -lm  -o dragnet2k 

rollercoaster: rollercoaster.o bdefrc.o bmatrix.o bmatrix.h
	igcc -O2  rollercoaster.o  bdefrc.o bmatrix.o\
		 -lglut -lglu -lgl  -lm  -o rollercoaster 


clean:
	rm *.o *.a $(EXECS)


 