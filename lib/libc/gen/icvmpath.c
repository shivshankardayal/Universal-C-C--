#include <icvmpath.h>
#include <string.h>
#include <stdio.h>


struct icvmpath{
	struct icvmpath *next;
	char path[1];
};

static struct icvmpath *paths=0;

#define CHECKPATH checkpath

static char *checkpath(char *s){
	FILE *f=fopen(s,"r");
	if(!f)
		kprintf("WARNING: %s DOES NOT EXIST\n",s);
	else
		fclose(f);
	return s;
}

char *__icvmpath(char *inpath){
	char *s=__geticvmbasepath();
	static char path[1024];
	int n;
	struct icvmpath *p;
	strcpy(path,s);
	n=strlen(path);
	if(n){
		--n;
		if(path[n]=='/'){
			if(inpath[0]=='/')
				path[n]=0;
		}else{
			if(inpath[0]!='/')
				strcat(path,"/");
		}
	}else
		if(inpath[0]!='/')
			strcpy(path,"/");
	strcat(path,inpath);
	p=paths;
	while(p){
		if(strcmp(p->path,path)==0)
			return CHECKPATH(p->path);
		p=p->next;
	}
	p=(struct icvmpath *)malloc(sizeof(struct icvmpath)+strlen(path));
	if(p){
		p->next=paths;
		paths=p;
		strcpy(p->path,path);
		return CHECKPATH(p->path);
	}
	return CHECKPATH(path);
}
			



				
			