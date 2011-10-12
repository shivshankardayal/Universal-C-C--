#include "cvm.h"
#include <string.h>


// Copyright (C) 2000,2001 Bob Dayley


#ifndef __WIN32__

#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>

#include "icvmsys.h"

#endif // !__WIN32__


s32  ICVM_strcat(void){
	R_R0=Iptr(strcat(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	));
	return R_R0;
}


s32 ICVM_strcmp(void){
	R_R0=strcmp(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	);
	return R_R0;
}

s32 ICVM_strcpy(void){
	R_R0=Iptr(strcpy(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	));
	return R_R0;
}

s32 ICVM_strlen(void){
	R_R0=strlen(
		(char *)Nptr(rSTKs32(0))
	);
	return R_R0;
}

s32 ICVM_strncpy(void){
	R_R0=Iptr(strncpy(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1)),
		rSTKs32(2)
	));
	return R_R0;
}

s32 ICVM_strncat(void){
	R_R0=Iptr(strncat(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1)),
		rSTKs32(2)
	));
	return R_R0;
}


s32 ICVM_strncmp(void){
	R_R0=strncmp(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1)),
		rSTKs32(2)
	);
	return R_R0;
}


s32 ICVM_memcpy(void){
	R_R0=Iptr(memcpy(
		(void *)Nptr(rSTKs32(0)),
		(void *)Nptr(rSTKs32(1)),
		rSTKs32(2)
	));
	return R_R0;
}

void ICVM_bcopy(void){
	int len=rSTKs32(2);
	if(len)
		memcpy(
			(void *)Nptr(rSTKs32(1)),
			(void *)Nptr(rSTKs32(0)),
				len
		);
}

s32 ICVM_memmove(void){
	void *d=(void *)Nptr(rSTKs32(0));
	void *s=(void *)Nptr(rSTKs32(1));
	u32 n=rSTKs32(2);
	R_R0=Iptr(d);
	memmove(d,s,n);
	return R_R0;
}


s32 ICVM_memcmp(void){
	register unsigned char *p1=(unsigned char *)Nptr(rSTKs32(0));
	register unsigned char *p2=(unsigned char *)Nptr(rSTKs32(1));
	register int n=rSTKu32(2);
	R_R0=memcmp(p1,p2,n);
	return R_R0;
}

s32 ICVM_memset(void){
	R_R0=Iptr(memset(
			(void *)Nptr(rSTKs32(0)),
			rSTKs32(1),
			rSTKs32(2)
	));
	return R_R0;
}


void ICVM_bzero(void){
	int len=rSTKs32(1);
	if(len)
		memset(
			(void *)Nptr(rSTKs32(0)),
			0,
			len
	);
}


s32 ICVM_bcmp(void){
	register s8 *p1;
	register s8 *p2;
	register s32 length=rSTKs32(2);
	if (length){
		p1=(s8 *)Nptr(rSTKs32(0));
		p2=(s8 *)Nptr(rSTKs32(1));
		do{
			if (*p1++ != *p2++)
				break;
		}while (--length);
	}
	R_R0=length;
	return R_R0;
}




void ICVM_strcspn(void){
	R_R0=strcspn(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	);
}



void ICVM_strncasecmp(void){
	R_R0=strncasecmp(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1)),
		rSTKs32(2)
	);
}

void ICVM_strpbrk(void){
	R_R0=Iptr(strpbrk(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	));
}


void ICVM_strspn(void){
	R_R0=Iptr(strspn(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	));
}

void ICVM_strstr(void){
	R_R0=Iptr(strstr(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	));
}


void ICVM_strtok(void){
	R_R0=Iptr(strtok(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	));
}


void ICVM_strchr(void){
	R_R0=Iptr(strchr(
		(char *)Nptr(rSTKs32(0)),
		rSTKs32(1)
	));
}


void ICVM_strrchr(void){
	R_R0=Iptr(strrchr(
		(char *)Nptr(rSTKs32(0)),
		rSTKs32(1)
	));
}






void ICVM_memccpy(void){
	register unsigned char *tp=(unsigned char *)Nptr(rSTKs32(0));
	register unsigned char *fp=(unsigned char *)Nptr(rSTKs32(1));
	int c=rSTKs32(2);
	s32  n=rSTKs32(3);
	R_R0=Iptr(memccpy(tp,fp,c,n));
}


void ICVM_memchr(void){
	register unsigned char *tp=(unsigned char *)Nptr(rSTKs32(0));
	int c=rSTKs32(1);
	s32  n=rSTKs32(2);
	R_R0=Iptr(memchr(tp,c,n));
}






void ICVM_swab(void){
	register char *s=(char *)Nptr(rSTKs32(0));
	register char *d=(char *)Nptr(rSTKs32(1));
	register int n=rSTKu32(2);
	register char c;
	n>>=1;
	n++;
	while(n){
		c=*s++;
		*d++=*s++;
		*d++=c;
		--n;
	}
}


char *i_strdup(char *p){
	int len=strlen(p)+1;
	char *s=(char *)i_malloc(len);
	if(s)
		memcpy(s,p,len);
	return s;
};
	
void ICVM_strdup(void){
	char *p=(char *)Nptr(rSTKs32(0));
	R_R0=Iptr(i_strdup(p));
	icvm_SetErr();
}


void ICVM_ffs(void){
	register s32 mask=rSTKu32(0);
	register int bit;
	R_R0=0;
	if (mask == 0)
		return;
	for (bit = 1; !(mask & 1); bit++)
		mask >>= 1;
	R_R0=bit;
}


void ICVM_strcasecmp(void){
	R_R0=strcasecmp(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	);
}



#ifdef NOT_NOW

static const unsigned char charmap[] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
	'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
	'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
	'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
	'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};



void ICVM_strcasecmp(void){
	register unsigned char *us1=(unsigned char *)Nptr(rSTKu32(0));
	register unsigned char *us2=(unsigned char *)Nptr(rSTKu32(1));
	register const unsigned char *cm = charmap;
	R_R0=0;
	while (cm[*us1] == cm[*us2++])
		if (*us1++ == '\0')
			return;
	R_R0= (cm[*us1] - cm[*--us2]);
}

void ICVM_strncasecmp(void){
	register unsigned char *us1=(unsigned char *)Nptr(rSTKu32(0));
	register unsigned char *us2=(unsigned char *)Nptr(rSTKu32(1));
	register int n=rSTKu32(2);
	R_R0=0;
	if (n != 0) {
		register const u_char *cm = charmap;
		do {
			if (cm[*us1] != cm[*us2++]){
				R_R0=(cm[*us1] - cm[*--us2]);
				break;
			}
			if (*us1++ == '\0')
				break;
		} while (--n != 0);
	}
}

#endif



	


