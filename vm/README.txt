Compilation and installation of Internet C++ Virtual Machine.

You must be SU to install these various components. 
You must install in the order listed.

Also, /usr/local/bin MUST be in your PATH variable.

####################################################
INSTALLATION of the Internet C++ Virtual Machine (ICVM):
####################################################

To Build and install VM:

	cd vm
	make 
	make install

icvm will be located in /bin


After building and installing the gcc, libraries, and vm, you may compile and run
ICVM programs. ICVM uses a shell header at the beginning of it's executables,
so it is only necessary to type the name of the command to run it.


For instance, to compile a program called testit:

	igcc -O2 testit.c -o testit

to run the program:
	
	./testit

To compile a program using the stdc++ libraries use:

	ig++ -O2 program.cc -o program

It is not necessary to compile C++ programs using ig++ if you are not using
IO streams or other standard C++ features. Most C++ programs will compile
and run just fine using igcc.






	