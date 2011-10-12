#include "icvmload.h"
#include "sys.h"

// Copyright (C) 2000,2001 Bob Dayley

extern "C" {

	void i_initfilesystem(void);
};


main(int argc,char *argv[]){
	FILE *f;
	ICVMFile *fr;
	ElfFile *e;
	if(argc<2){
		fprintf(stderr,"use : icvm icvmfile <arg> ...\n");
		exit(3);
	}
#ifdef __WIN32__
	if(strcmp(argv[1],"-@@@-")==0){
//fprintf(stderr,"FORK CALLED %d %d\n",atoi(argv[2]),atoi(argv[3]));
		i_thisisfork=1;
		if(i_artificialforkchild(atoi(argv[2]),atoi(argv[3]))==-1)
			exit(3);
		ICVMMain(0,0,0);
		return 0;
	}
	
#endif // __WIN32__

	if(i_initsystem()<0){
		fprintf(stderr,"System Failure\n");
		exit(3);
	}
#ifndef __WIN32__
	f=fopen(argv[1],"r");
#else // __WIN32__
	f=fopen(argv[1],"rb");
#endif // __WIN32__
	if(f==0){
		fprintf(stderr,"Could not open %s\n",argv[1]);
		exit(3);;
	}

	fr=new ICVMFile(f);
	e=new ElfFile();
//	fprintf(stderr,"Reading %s\n",argv[1]);
//	fprintf(stderr,"Reading LittleEndian\n");
	e->Read(fr,argv[1]);
	if(fr->Error()){
		fprintf(stderr,"Error reading %s\n",argv[1]);
		exit(3);
	}
	
//	fprintf(stderr,"Done reading %s err=%d\n",argv[1],fr->Error());

	ICVMMain(argc-1,argv+1,Iptr(e->GetTextSectionData()));
	return 0;
}


