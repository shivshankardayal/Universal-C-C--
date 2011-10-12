#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>

const char *user_from_uid(uid_t id,int noname){
	struct passwd *pw;
	pw=getpwuid(id);
	if(pw)
		return pw->pw_name;
	return NULL;
}

