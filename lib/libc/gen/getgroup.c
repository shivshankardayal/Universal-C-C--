#include <stdlib.h>
#include <stdio.h>
#include <grp.h>

const char *group_from_gid(gid_t id,int noname){
	struct group *gp;
	gp=getgrgid(id);
	if(gp)
		return gp->gr_name;
	return NULL;
}

