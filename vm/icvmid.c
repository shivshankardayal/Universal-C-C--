#include "cvm.h"

#include "sys.h"
#include "icvmsys.h"
#include "id.h"

// Copyright (C) 2000,2001 Bob Dayley


#ifndef __WIN32__

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>



#endif // !__WIN32





void ICVM_getuid(void){
#ifdef __WIN32__
	R_R0=DEFAULT_UID;
#endif
#ifdef UNIXSYSTEM
	R_R0=getuid();
#endif
};


// icvm pw offsets
#define ICVM_pw_name		0
#define ICVM_pw_passwd		4
#define	ICVM_pw_uid			8
#define	ICVM_pw_gid			12
#define	ICVM_pw_change		16
#define	ICVM_pw_class		20
#define	ICVM_pw_gecos		24
#define	ICVM_pw_dir			28
#define	ICVM_pw_shell		32
#define	ICVM_pw_expire		36
#define	ICVM_pw_sizeof		40

typedef s8 struct_ICVM_passwd[ICVM_pw_sizeof];


// icvm group structure offsets

#define ICVM_gr_name	0
#define ICVM_gr_passwd	4
#define ICVM_gr_gid		8
#define ICVM_gr_mem		12
#define ICVM_gr_sizeof	16

typedef s8 struct_ICVM_group[ICVM_gr_sizeof];


#ifdef UNIXSYSTEM
static s8 *MakeICVMPasswd(struct passwd *p){
	static struct_ICVM_passwd _pw;
	s8 *pw=(s8 *)&_pw;
	WDs32(pw+ICVM_pw_name,p->pw_name,0);
	WDs32(pw+ICVM_pw_uid,p->pw_uid,0);
	WDs32(pw+ICVM_pw_gid,p->pw_gid,0);
	WDs32(pw+ICVM_pw_dir,p->pw_dir,0);
	WDs32(pw+ICVM_pw_shell,p->pw_shell,0);
	WDs32(pw+ICVM_pw_passwd,"*",0);
	WDs32(pw+ICVM_pw_gecos,"",0);
	WDs32(pw+ICVM_pw_class,"",0);
	WDu32(pw+ICVM_pw_change,0x7fffffff,0);
	WDu32(pw+ICVM_pw_expire,0x7fffffff,0);
	return pw;
};

#endif

#ifdef __WIN32__
static s8 *MakeICVMPasswd(void){
	static struct_ICVM_passwd _pw;
	s8 *pw=(s8 *)&_pw;
	WDs32(pw+ICVM_pw_name,DEFAULT_UNAME,0);
	WDs32(pw+ICVM_pw_uid,DEFAULT_UID,0);
	WDs32(pw+ICVM_pw_gid,DEFAULT_GID,0);
	WDs32(pw+ICVM_pw_dir,".",0);
	WDs32(pw+ICVM_pw_shell,"sh",0);
	WDs32(pw+ICVM_pw_passwd,"*",0);
	WDs32(pw+ICVM_pw_gecos,"",0);
	WDs32(pw+ICVM_pw_class,"",0);
	WDu32(pw+ICVM_pw_change,0x7fffffff,0);
	WDu32(pw+ICVM_pw_expire,0x7fffffff,0);
	return pw;
};

#endif // __WIN32__


void ICVM_getpwuid(void){
#ifdef __WIN32__
	R_R0=(p32)MakeICVMPasswd();
#endif
#ifdef UNIXSYSTEM
	struct passwd *g=getpwuid(rSTKs32(0));
	if(g)
		R_R0=(p32)MakeICVMPasswd(g);
	else
		R_R0=0;
	icvm_SetErr();
#endif

}


#ifdef UNIXSYSTEM
static s8 *MakeICVMGroup(struct group *g){
	static struct_ICVM_group _gi;
	s8 *gi=(s8 *)&_gi;
	WDu32(gi+ICVM_gr_name,g->gr_name,0);
	WDu32(gi+ICVM_gr_passwd,"*",0);
	WDu32(gi+ICVM_gr_mem,g->gr_mem,0);
	WDu32(gi+ICVM_gr_gid,g->gr_gid,0);
	return gi;
};
#endif // UNIXSYSTEM

#ifdef __WIN32__
static s8 *MakeICVMGroup(void){
	static struct_ICVM_group _gi;
	s8 *gi=(s8 *)&_gi;
	WDu32(gi+ICVM_gr_name,DEFAULT_GNAME,0);
	WDu32(gi+ICVM_gr_passwd,"*",0);
	WDu32(gi+ICVM_gr_mem,0,0);
	WDu32(gi+ICVM_gr_gid,DEFAULT_GID,0);
	return gi;
};
#endif // UNIXSYSTEM

void ICVM_getgrgid(void){
#ifdef __WIN32__
		R_R0=(p32)MakeICVMGroup();
#endif

#ifdef UNIXSYSTEM
	struct group *g=getgrgid(rSTKs32(0));
	if(g)
		R_R0=(p32)MakeICVMGroup(g);
	else
		R_R0=0;
	icvm_SetErr();
#endif

}


#ifndef __WIN32__
void ICVM_gethostid(void){R_R0=gethostid();};
void ICVM_geteuid(void){R_R0=geteuid();};
void ICVM_getegid(void){R_R0=getegid();};
void ICVM_getgid(void){
#ifdef __WIN32__
	R_R0=DEFAULT_UID;
#endif
#ifdef UNIXSYSTEM
	R_R0=getgid();
#endif
};



void ICVM_getlogin(void){R_R0=(p32)getlogin();};

void ICVM_setuid(void){
	R_R0=setuid(rSTKs32(0));
	icvm_SetErr();
}

void ICVM_sethostid(void){
// ********** OBSOLETE ********* REMOVE	
//	R_R0=sethostid(rSTKs32(0));
}

void ICVM_seteuid(void){
	R_R0=seteuid(rSTKs32(0));
	icvm_SetErr();
}

void ICVM_setreuid(void){
	R_R0=setreuid(
		rSTKs32(0),
		rSTKs32(1)
	);
	icvm_SetErr();
}

void ICVM_setregid(void){
	R_R0=setregid(
		rSTKs32(0),
		rSTKs32(1)
	);
	icvm_SetErr();
}

void ICVM_setgid(void){
	R_R0=setgid(rSTKs32(0));
	icvm_SetErr();
}

void ICVM_setegid(void){
	R_R0=setegid(rSTKs32(0));
	icvm_SetErr();
}

void ICVM_getpriority(void){
	R_R0=getpriority(
		rSTKs32(0),
		rSTKs32(1)
	);
	icvm_SetErr();
}

void ICVM_setpriority(void){
	R_R0=getpriority(
		rSTKs32(0),
		rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}




void ICVM_getgrnam(void){
	struct group *g=getgrnam((char *)rSTKs32(0));
	if(g)
		R_R0=(p32)MakeICVMGroup(g);
	else
		R_R0=0;
	icvm_SetErr();
}

void ICVM_getpwnam(void){
	struct passwd *g=getpwnam((char *)rSTKs32(0));
	if(g)
		R_R0=(p32)MakeICVMPasswd(g);
	else
		R_R0=0;
	icvm_SetErr();
}

void ICVM_getpwent(void){
	struct passwd *g=getpwent();
	if(g)
		R_R0=(p32)MakeICVMPasswd(g);
	else
		R_R0=0;
	icvm_SetErr();
}

void ICVM_setpwent(void){
	setpwent();
	icvm_SetErr();
}

void ICVM_endpwent(void){
	endpwent();
	icvm_SetErr();
}

void ICVM_getgrent(void){
	struct group *g=getgrent();
	if(g)
		R_R0=(p32)MakeICVMGroup(g);
	else
		R_R0=0;
	icvm_SetErr();
}

void ICVM_setgrent(void){
	setgrent();
	icvm_SetErr();
}

void ICVM_endgrent(void){
	endgrent();
	icvm_SetErr();
}


#endif // !__WIN32__

