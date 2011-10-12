#include <sys/fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
main()
{
	int fh;
	int dupfh;
	struct stat st;	
	write(1,"Test Stdout\n",strlen("Test Stdout\n"));
	write(2,"Test Stderr\n",strlen("Test Stderr\n"));
	fh=open("/home/t/testfile",O_CREAT | O_TRUNC | O_WRONLY,0x1ff);
	if(fh<0){
		kprintf("open failed\n");
		exit(3);
	}
	write(fh,"Howdy\n",strlen("Howdy\n"));
	kprintf("Dup fh=%d\n",dupfh=dup(fh));
	write(dupfh,"Writing to DUPFH\n",17);
	kprintf("Dup2 fh,21=%d\n",dupfh=dup2(fh,21));
	write(dupfh,"Writing to DUP2FH\n",18);
	close(fh);

	fh=open("/dev/stderr",O_WRONLY);
	if(fh<0){
		kprintf("open stderr failed\n");
		exit(3);
	}
	write(fh,"Howdy Stderr\n",strlen("Howdy Stderr\n"));
	kprintf("fstat fh %d\n",fstat(fh,&st));
	kprintf("Dup fh=%d\n",dupfh=dup(fh));
	write(dupfh,"Writing to DUPFH\n",17);
	kprintf("Dup2 fh,20=%d\n",dupfh=dup2(fh,20));
	write(dupfh,"Writing to DUP2FH\n",18);
	close(fh);
	kprintf("stat /dev/stderr %d\n",stat("/dev/stderr",&st));
	kprintf("stat /home/t %d\n",stat("/home/t",&st));
	kprintf("stat /home/t/testfile %d\n",stat("/home/t/testfile",&st));
	kprintf("stat /home/t/nofile %d\n",stat("/home/t/nofile",&st));
	fprintf(stderr,"Test fprintf stderr\n");
	fprintf(stdout,"Test fprintf stdout\n");
	printf("Test printf stdout\n");
}
