#include "cvm.h"


// Copyright (C) 2000,2001 Bob Dayley

#ifndef __WIN32__

#include <sys/time.h>
#include <sys/uio.h>
#include "icvmsys.h"
#include "icvmerr.h"
#include <X11/Xlib.h>




// simple x declarations - localized for our use

typedef void *ciptr;
typedef int BytesReadable;


#define XTRANS_MAX_ADDR_LEN	128	/* large enough to hold sun_path */

typedef	struct {
    unsigned char	addr[XTRANS_MAX_ADDR_LEN];
} Xtransaddr;




int _X11TransGetConnectionNumber(ciptr ptr);
int _X11TransBytesReadable(ciptr ptr,BytesReadable *r);
int _X11TransRead(ciptr ptr,char *buf,int size);
int _X11TransWrite(ciptr ptr,char *buf,int size);
int _X11TransReadv(ciptr ptr,struct iovec *buf,int size);
int _X11TransWritev(ciptr ptr,struct iovec *buf,int size);
int _X11TransClose(ciptr ptr);
int _X11TransCloseForCloning(ciptr ptr);
int _X11TransIsLocal(ciptr ptr);
int _X11TransConnect(ciptr ptr,char *r);
int _X11TransGetHostname(ciptr ptr,char *r);
int _X11TransOpenCOTSClient(char *addr);
int _X11TransOpenCLTSClient(char *addr);
int _X11TransGetPeerAddr(ciptr ptr,int *familyp,int *addrlenp, 
	Xtransaddr **addrp);
int _X11TransConvertAddress(int *familyp,int *addrlenp, Xtransaddr **addrp);
int _X11TransGetMyAddr(ciptr ptr,int *familyp,int *addrlenp, 
	Xtransaddr **addrp);
int _X11TransSetOption(ciptr ptr,int option,int arg);
int _X11TransDisconnect(ciptr ptr);
int _X11TransFreeConnInfo(ciptr ptr);



void ICVM_X11TransGetConnectionNumber(void){
	R_R0=_X11TransGetConnectionNumber((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_X11TransBytesReadable(void){
	BytesReadable br;
	s32 *ibr=(s32 *)rSTKs32(1);
	R_R0=_X11TransBytesReadable((ciptr)rSTKs32(0),&br);
	icvm_SetErr();
	if(ibr)
		WDs32(ibr,br,0);

}

void ICVM_X11TransRead(void){
	R_R0=_X11TransRead(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}

void ICVM_X11TransReadv(void){
	char *iov=(char *)rSTKs32(1);
	int cnt=rSTKs32(2);

	struct iovec *sysiov;
	int cnt1=0;
	sysiov=alloca(sizeof(struct iovec)*cnt);
	while(cnt1<cnt){
		sysiov[cnt1].iov_base=(void *)RDs32(iov,0);	
		sysiov[cnt1].iov_len=RDs32(iov+4,0);	
		iov+=8;
		++cnt1;
	}
	R_R0=_X11TransReadv((ciptr)rSTKs32(0),sysiov,cnt);
	icvm_SetErr();
					
}



void ICVM_X11TransWrite(void){
	R_R0=_X11TransWrite(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}



void ICVM_X11TransWritev(void){
	char *iov=(char *)rSTKs32(1);
	int cnt=rSTKs32(2);

	struct iovec *sysiov;
	int cnt1=0;
	sysiov=alloca(sizeof(struct iovec)*cnt);
	while(cnt1<cnt){
		sysiov[cnt1].iov_base=(void *)RDs32(iov,0);	
		sysiov[cnt1].iov_len=RDs32(iov+4,0);	
		iov+=8;
		++cnt1;
	}
	R_R0=_X11TransWritev((ciptr)rSTKs32(0),sysiov,cnt);
	icvm_SetErr();
					
}

void ICVM_X11TransClose(void){
	R_R0=_X11TransClose((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_X11TransIsLocal(void){
	R_R0=_X11TransIsLocal((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_X11TransCloseForCloning(void){
	R_R0=_X11TransCloseForCloning((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_X11TransConnect(void){
	R_R0=_X11TransConnect(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1)
	);
	icvm_SetErr();
}

void ICVM_X11TransGetHostname(void){
	R_R0=_X11TransGetHostname(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1)
	);
	icvm_SetErr();
}

void ICVM_X11TransOpenCOTSClient(void){
	R_R0=(s32)_X11TransOpenCOTSClient(
		(char *)rSTKs32(0)
	);
	icvm_SetErr();
}

void ICVM_X11TransOpenCLTSClient(void){
	R_R0=(s32)_X11TransOpenCLTSClient(
		(char *)rSTKs32(0)
	);
	icvm_SetErr();
}



void ICVM_X11TransGetPeerAddr(void){
	Xtransaddr *ma;
	void *m;
	int family;
	int addrlen;

	R_R0=_X11TransGetPeerAddr(
		(char *)rSTKs32(0),
		&family,&addrlen,&ma
	);
	icvm_SetErr();
	if(R_R0==-1 || ma==0)
		return;
	WDs32(rSTKs32(1),family,0);
	WDs32(rSTKs32(2),addrlen,0);
	m=i_malloc(addrlen);

	if(m)
		memcpy(m,ma,addrlen);
	else
		R_ERRNO1=ICVM_ENOMEM;
	free(ma);
	WDs32(rSTKs32(3),m,0);
}

void ICVM_X11TransGetMyAddr(void){
	Xtransaddr *ma;
	void *m;
	int family;
	int addrlen;

	R_R0=_X11TransGetMyAddr(
		(char *)rSTKs32(0),
		&family,&addrlen,&ma
	);
	icvm_SetErr();
	if(R_R0==-1 || ma==0)
		return;
	WDs32(rSTKs32(1),family,0);
	WDs32(rSTKs32(2),addrlen,0);
	m=i_malloc(addrlen);

	if(m)
		memcpy(m,ma,addrlen);
	else
		R_ERRNO1=ICVM_ENOMEM;
	free(ma);
	WDs32(rSTKs32(3),m,0);
}


void ICVM_X11TransConvertAddress(void){
	Xtransaddr *ma;
	void *m;
	int family;
	int addrlen;

	family=RDs32(rSTKs32(0),0);
	addrlen=RDs32(rSTKs32(1),0);
	m=(void *)RDs32(rSTKs32(2),0);

	R_R0=-1;
	ma=(Xtransaddr *)malloc(addrlen);
	if(ma==0){
		R_ERRNO1=ICVM_ENOMEM;
		return;
	}
	memcpy(ma,m,addrlen);
	i_free(m);

	R_R0=_X11TransConvertAddress(&family,&addrlen,&ma);
	icvm_SetErr();
	if(R_R0==-1 || ma==0)
		icvm_SetErr();

	WDs32(rSTKs32(0),family,0);
	WDs32(rSTKs32(1),addrlen,0);
	m=i_malloc(addrlen);
	if(m)
		memcpy(m,ma,addrlen);
	else
		R_ERRNO1=ICVM_ENOMEM;
	free(ma);
	WDs32(rSTKs32(2),m,0);
}

void ICVM_X11TransSetOption(void){
	R_R0=_X11TransSetOption(
		(ciptr)rSTKs32(0),
		rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}

void ICVM_X11TransDisconnect(void){
	R_R0=_X11TransDisconnect((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_X11TransFreeConnInfo(void){
	R_R0=_X11TransFreeConnInfo((ciptr)rSTKs32(0));
	icvm_SetErr();
}



int _IceTransGetConnectionNumber(ciptr ptr);
int _IceTransBytesReadable(ciptr ptr,BytesReadable *r);
int _IceTransRead(ciptr ptr,char *buf,int size);
int _IceTransWrite(ciptr ptr,char *buf,int size);
int _IceTransReadv(ciptr ptr,struct iovec *buf,int size);
int _IceTransWritev(ciptr ptr,struct iovec *buf,int size);
int _IceTransClose(ciptr ptr);
int _IceTransIsLocal(ciptr ptr);
int _IceTransCloseForCloning(ciptr ptr);
int _IceTransConnect(ciptr ptr,char *r);
int _IceTransGetHostname(ciptr ptr,char *r);
int _IceTransOpenCOTSClient(char *addr);
int _IceTransOpenCLTSClient(char *addr);
int _IceTransGetPeerAddr(ciptr ptr,int *familyp,int *addrlenp, 
	Xtransaddr **addrp);
int _IceTransGetMyAddr(ciptr ptr,int *familyp,int *addrlenp, 
	Xtransaddr **addrp);
int _IceTransSetOption(ciptr ptr,int option,int arg);
int _IceTransDisconnect(ciptr ptr);
int _IceTransFreeConnInfo(ciptr ptr);
char *_IceTransGetPeerNetworkId(ciptr ptr);
char *_IceTransGetMyNetworkId(ciptr ptr);



void ICVM_IceTransGetConnectionNumber(void){
	R_R0=_IceTransGetConnectionNumber((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_IceTransBytesReadable(void){
	BytesReadable br;
	s32 *ibr=(s32 *)rSTKs32(1);
	R_R0=_IceTransBytesReadable((ciptr)rSTKs32(0),&br);
	icvm_SetErr();
	if(ibr)
		WDs32(ibr,br,0);

}

void ICVM_IceTransRead(void){
	R_R0=_IceTransRead(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}

void ICVM_IceTransReadv(void){
	char *iov=(char *)rSTKs32(1);
	int cnt=rSTKs32(2);

	struct iovec *sysiov;
	int cnt1=0;
	sysiov=alloca(sizeof(struct iovec)*cnt);
	while(cnt1<cnt){
		sysiov[cnt1].iov_base=(void *)RDs32(iov,0);	
		sysiov[cnt1].iov_len=RDs32(iov+4,0);	
		iov+=8;
		++cnt1;
	}
	R_R0=_IceTransReadv((ciptr)rSTKs32(0),sysiov,cnt);
	icvm_SetErr();
					
}

void ICVM_IceTransWritev(void){
	char *iov=(char *)rSTKs32(1);
	int cnt=rSTKs32(2);

	struct iovec *sysiov;
	int cnt1=0;
	sysiov=alloca(sizeof(struct iovec)*cnt);
	while(cnt1<cnt){
		sysiov[cnt1].iov_base=(void *)RDs32(iov,0);	
		sysiov[cnt1].iov_len=RDs32(iov+4,0);	
		iov+=8;
		++cnt1;
	}
	R_R0=_IceTransWritev((ciptr)rSTKs32(0),sysiov,cnt);
	icvm_SetErr();
					
}


void ICVM_IceTransWrite(void){
	R_R0=_IceTransWrite(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}



void ICVM_IceTransClose(void){
	R_R0=_IceTransClose((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_IceTransIsLocal(void){
	R_R0=_IceTransIsLocal((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_IceTransCloseForCloning(void){
	R_R0=_IceTransCloseForCloning((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_IceTransConnect(void){
	R_R0=_IceTransConnect(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1)
	);
	icvm_SetErr();
}

void ICVM_IceTransGetHostname(void){
	R_R0=_IceTransGetHostname(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1)
	);
	icvm_SetErr();
}

void ICVM_IceTransOpenCOTSClient(void){
	R_R0=(s32)_IceTransOpenCOTSClient(
		(char *)rSTKs32(0)
	);
	icvm_SetErr();
}


void ICVM_IceTransOpenCLTSClient(void){
	R_R0=(s32)_IceTransOpenCLTSClient(
		(char *)rSTKs32(0)
	);
	icvm_SetErr();
}



void ICVM_IceTransGetPeerAddr(void){
	Xtransaddr *ma;
	void *m;
	int family;
	int addrlen;

	R_R0=_IceTransGetPeerAddr(
		(char *)rSTKs32(0),
		&family,&addrlen,&ma
	);
	icvm_SetErr();
	if(R_R0==-1 || ma==0)
		return;
	WDs32(rSTKs32(1),family,0);
	WDs32(rSTKs32(2),addrlen,0);
	m=i_malloc(addrlen);

	if(m)
		memcpy(m,ma,addrlen);
	else
		R_ERRNO1=ICVM_ENOMEM;
	free(ma);
	WDs32(rSTKs32(3),m,0);
}


void ICVM_IceTransGetMyAddr(void){
	Xtransaddr *ma;
	void *m;
	int family;
	int addrlen;

	R_R0=_IceTransGetMyAddr(
		(char *)rSTKs32(0),
		&family,&addrlen,&ma
	);
	icvm_SetErr();
	if(R_R0==-1 || ma==0)
		return;
	WDs32(rSTKs32(1),family,0);
	WDs32(rSTKs32(2),addrlen,0);
	m=i_malloc(addrlen);

	if(m)
		memcpy(m,ma,addrlen);
	else
		R_ERRNO1=ICVM_ENOMEM;
	free(ma);
	WDs32(rSTKs32(3),m,0);
}




void ICVM_IceTransSetOption(void){
	R_R0=_IceTransSetOption(
		(ciptr)rSTKs32(0),
		rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}

void ICVM_IceTransDisconnect(void){
	R_R0=_IceTransDisconnect((ciptr)rSTKs32(0));
	icvm_SetErr();
}


void ICVM_IceTransFreeConnInfo(void){
	R_R0=_IceTransFreeConnInfo((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_IceTransGetPeerNetworkId(void){
	char *t=0;
	char *s=_IceTransGetPeerNetworkId((ciptr)rSTKs32(0));
	if(s){
		t=i_malloc(strlen(s)+1);
		if(t)
			strcpy(t,s);
		free(s);
	}
	R_R0=(s32) t;
	icvm_SetErr();
}

void ICVM_IceTransGetMyNetworkId(void){
	char *t=0;
	char *s=_IceTransGetMyNetworkId((ciptr)rSTKs32(0));
	if(s){
		t=i_malloc(strlen(s)+1);
		if(t)
			strcpy(t,s);
		free(s);
	}
	R_R0=(s32) t;
	icvm_SetErr();
}

// sys calls in place, but font server needs to be enabled if you would use it
// you will also need to modify the makefile to link it in

#ifdef FONT_SERVER_SUPPORT
int _FSTransGetConnectionNumber(ciptr ptr);
int _FSTransBytesReadable(ciptr ptr,BytesReadable *r);
int _FSTransRead(ciptr ptr,char *buf,int size);
int _FSTransWrite(ciptr ptr,char *buf,int size);
int _FSTransReadv(ciptr ptr,struct iovec *buf,int size);
int _FSTransWritev(ciptr ptr,struct iovec *buf,int size);
int _FSTransClose(ciptr ptr);
int _FSTransCloseForCloning(ciptr ptr);
int _FSTransIsLocal(ciptr ptr);
int _FSTransConnect(ciptr ptr,char *r);
int _FSTransGetHostname(ciptr ptr,char *r);
int _FSTransOpenCOTSClient(char *addr);
int _FSTransOpenCLTSClient(char *addr);
int _FSTransGetPeerAddr(ciptr ptr,int *familyp,int *addrlenp, 
	Xtransaddr **addrp);
int _FSTransGetMyAddr(ciptr ptr,int *familyp,int *addrlenp, 
	Xtransaddr **addrp);
int _FSTransSetOption(ciptr ptr,int option,int arg);
int _FSTransDisconnect(ciptr ptr);
int _FSTransFreeConnInfo(ciptr ptr);



void ICVM_FSTransGetConnectionNumber(void){
	R_R0=_FSTransGetConnectionNumber((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_FSTransBytesReadable(void){
	BytesReadable br;
	s32 *ibr=(s32 *)rSTKs32(1);
	R_R0=_FSTransBytesReadable((ciptr)rSTKs32(0),&br);
	icvm_SetErr();
	if(ibr)
		WDs32(ibr,br);

}

void ICVM_FSTransRead(void){
	R_R0=_FSTransRead(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}


void ICVM_FSTransReadv(void){
	char *iov=(char *)rSTKs32(1);
	int cnt=rSTKs32(2);

	struct iovec *sysiov;
	int cnt1=0;
	sysiov=alloca(sizeof(struct iovec)*cnt);
	while(cnt1<cnt){
		sysiov[cnt1].iov_base=(void *)RDs32(iov);	
		sysiov[cnt1].iov_len=RDs32(iov+4);	
		iov+=8;
		++cnt1;
	}
	R_R0=_FSTransReadv((ciptr)rSTKs32(0),sysiov,cnt);
	icvm_SetErr();
					
}

void ICVM_FSTransWritev(void){
	char *iov=(char *)rSTKs32(1);
	int cnt=rSTKs32(2);

	struct iovec *sysiov;
	int cnt1=0;
	sysiov=alloca(sizeof(struct iovec)*cnt);
	while(cnt1<cnt){
		sysiov[cnt1].iov_base=(void *)RDs32(iov);	
		sysiov[cnt1].iov_len=RDs32(iov+4);	
		iov+=8;
		++cnt1;
	}
	R_R0=_FSTransWritev((ciptr)rSTKs32(0),sysiov,cnt);
	icvm_SetErr();
					
}




void ICVM_FSTransWrite(void){
	R_R0=_FSTransWrite(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}




void ICVM_FSTransClose(void){
	R_R0=_FSTransClose((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_FSTransIsLocal(void){
	R_R0=_FSTransIsLocal((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_FSTransCloseForCloning(void){
	R_R0=_FSTransCloseForCloning((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_FSTransConnect(void){
	R_R0=_FSTransConnect(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1)
	);
	icvm_SetErr();
}

void ICVM_FSTransGetHostname(void){
	R_R0=_FSTransGetHostname(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1)
	);
	icvm_SetErr();
}

void ICVM_FSTransOpenCOTSClient(void){
	R_R0=(s32)_FSTransOpenCOTSClient(
		(char *)rSTKs32(0)
	);
	icvm_SetErr();
}

void ICVM_FSTransOpenCLTSClient(void){
	R_R0=(s32)_FSTransOpenCLTSClient(
		(char *)rSTKs32(0)
	);
	icvm_SetErr();
}



void ICVM_FSTransGetPeerAddr(void){
	Xtransaddr *ma;
	void *m;
	int family;
	int addrlen;

	R_R0=_FSTransGetPeerAddr(
		(char *)rSTKs32(0),
		&family,&addrlen,&ma
	);
	icvm_SetErr();
	if(R_R0==-1 || ma==0)
		return;
	WDs32(rSTKs32(1),family);
	WDs32(rSTKs32(2),addrlen);
	m=i_malloc(addrlen);

	if(m)
		memcpy(m,ma,addrlen);
	else
		R_ERRNO1=ICVM_ENOMEM;
	free(ma);
	WDs32(rSTKs32(3),m);
}

void ICVM_FSTransGetMyAddr(void){
	Xtransaddr *ma;
	void *m;
	int family;
	int addrlen;

	R_R0=_FSTransGetMyAddr(
		(char *)rSTKs32(0),
		&family,&addrlen,&ma
	);
	icvm_SetErr();
	if(R_R0==-1 || ma==0)
		return;
	WDs32(rSTKs32(1),family);
	WDs32(rSTKs32(2),addrlen);
	m=i_malloc(addrlen);

	if(m)
		memcpy(m,ma,addrlen);
	else
		R_ERRNO1=ICVM_ENOMEM;
	free(ma);
	WDs32(rSTKs32(3),m);
}



void ICVM_FSTransSetOption(void){
	R_R0=_FSTransSetOption(
		(ciptr)rSTKs32(0),
		rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}

void ICVM_FSTransDisconnect(void){
	R_R0=_FSTransDisconnect((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_FSTransFreeConnInfo(void){
	R_R0=_FSTransFreeConnInfo((ciptr)rSTKs32(0));
	icvm_SetErr();
}
#endif // FONTSERVER

int _XimXTransGetConnectionNumber(ciptr ptr);
int _XimXTransBytesReadable(ciptr ptr,BytesReadable *r);
int _XimXTransRead(ciptr ptr,char *buf,int size);
int _XimXTransWrite(ciptr ptr,char *buf,int size);
int _XimXTransReadv(ciptr ptr,struct iovec *buf,int size);
int _XimXTransWritev(ciptr ptr,struct iovec *buf,int size);
int _XimXTransClose(ciptr ptr);
int _XimXTransCloseForCloning(ciptr ptr);
int _XimXTransIsLocal(ciptr ptr);
int _XimXTransConnect(ciptr ptr,char *r);
int _XimXTransGetHostname(ciptr ptr,char *r);
int _XimXTransOpenCOTSClient(char *addr);
int _XimXTransOpenCLTSClient(char *addr);
int _XimXTransGetPeerAddr(ciptr ptr,int *familyp,int *addrlenp, 
	Xtransaddr **addrp);
int _XimXTransGetMyAddr(ciptr ptr,int *familyp,int *addrlenp, 
	Xtransaddr **addrp);
int _XimXTransSetOption(ciptr ptr,int option,int arg);
int _XimXTransDisconnect(ciptr ptr);
int _XimXTransFreeConnInfo(ciptr ptr);



void ICVM_XimXTransGetConnectionNumber(void){
	R_R0=_XimXTransGetConnectionNumber((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_XimXTransBytesReadable(void){
	BytesReadable br;
	s32 *ibr=(s32 *)rSTKs32(1);
	R_R0=_XimXTransBytesReadable((ciptr)rSTKs32(0),&br);
	icvm_SetErr();
	if(ibr)
		WDs32(ibr,br,0);

}

void ICVM_XimXTransRead(void){
	R_R0=_XimXTransRead(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}

void ICVM_XimXTransReadv(void){
	char *iov=(char *)rSTKs32(1);
	int cnt=rSTKs32(2);

	struct iovec *sysiov;
	int cnt1=0;
	sysiov=alloca(sizeof(struct iovec)*cnt);
	while(cnt1<cnt){
		sysiov[cnt1].iov_base=(void *)RDs32(iov,0);	
		sysiov[cnt1].iov_len=RDs32(iov+4,0);	
		iov+=8;
		++cnt1;
	}
	R_R0=_XimXTransReadv((ciptr)rSTKs32(0),sysiov,cnt);
	icvm_SetErr();
					
}


void ICVM_XimXTransWrite(void){
	R_R0=_XimXTransWrite(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}


void ICVM_XimXTransWritev(void){
	char *iov=(char *)rSTKs32(1);
	int cnt=rSTKs32(2);

	struct iovec *sysiov;
	int cnt1=0;
	sysiov=alloca(sizeof(struct iovec)*cnt);
	while(cnt1<cnt){
		sysiov[cnt1].iov_base=(void *)RDs32(iov,0);	
		sysiov[cnt1].iov_len=RDs32(iov+4,0);	
		iov+=8;
		++cnt1;
	}
	R_R0=_XimXTransWritev((ciptr)rSTKs32(0),sysiov,cnt);
	icvm_SetErr();
					
}


void ICVM_XimXTransClose(void){
	R_R0=_XimXTransClose((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_XimXTransIsLocal(void){
	R_R0=_XimXTransIsLocal((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_XimXTransCloseForCloning(void){
	R_R0=_XimXTransCloseForCloning((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_XimXTransConnect(void){
	R_R0=_XimXTransConnect(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1)
	);
	icvm_SetErr();
}

void ICVM_XimXTransGetHostname(void){
	R_R0=_XimXTransGetHostname(
		(ciptr)rSTKs32(0),
		(char *)rSTKs32(1)
	);
	icvm_SetErr();
}

void ICVM_XimXTransOpenCOTSClient(void){
	R_R0=(s32)_XimXTransOpenCOTSClient(
		(char *)rSTKs32(0)
	);
	icvm_SetErr();
}

void ICVM_XimXTransOpenCLTSClient(void){
	R_R0=(s32)_XimXTransOpenCLTSClient(
		(char *)rSTKs32(0)
	);
	icvm_SetErr();
}



void ICVM_XimXTransGetPeerAddr(void){
	Xtransaddr *ma;
	void *m;
	int family;
	int addrlen;

	R_R0=_XimXTransGetPeerAddr(
		(char *)rSTKs32(0),
		&family,&addrlen,&ma
	);
	icvm_SetErr();
	if(R_R0==-1 || ma==0)
		return;
	WDs32(rSTKs32(1),family,0);
	WDs32(rSTKs32(2),addrlen,0);
	m=i_malloc(addrlen);

	if(m)
		memcpy(m,ma,addrlen);
	else
		R_ERRNO1=ICVM_ENOMEM;
	free(ma);
	WDs32(rSTKs32(3),m,0);
}

void ICVM_XimXTransGetMyAddr(void){
	Xtransaddr *ma;
	void *m;
	int family;
	int addrlen;

	R_R0=_XimXTransGetMyAddr(
		(char *)rSTKs32(0),
		&family,&addrlen,&ma
	);
	icvm_SetErr();
	if(R_R0==-1 || ma==0)
		return;
	WDs32(rSTKs32(1),family,0);
	WDs32(rSTKs32(2),addrlen,0);
	m=i_malloc(addrlen);

	if(m)
		memcpy(m,ma,addrlen);
	else
		R_ERRNO1=ICVM_ENOMEM;
	free(ma);
	WDs32(rSTKs32(3),m,0);
}



void ICVM_XimXTransSetOption(void){
	R_R0=_XimXTransSetOption(
		(ciptr)rSTKs32(0),
		rSTKs32(1),
		rSTKs32(2)
	);
	icvm_SetErr();
}

void ICVM_XimXTransDisconnect(void){
	R_R0=_XimXTransDisconnect((ciptr)rSTKs32(0));
	icvm_SetErr();
}

void ICVM_XimXTransFreeConnInfo(void){
	R_R0=_XimXTransFreeConnInfo((ciptr)rSTKs32(0));
	icvm_SetErr();
}


// ICVM custom function

// ICVM custom function
// translate an 8 bit colormap to 8 bits given an input color palette table
// colormap is 256 entries each to translate with
// ICVM_BltTranslateCM8To8(u8 *dst,u8 *src,int cnt,u8 *colormap)

void ICVM_BltTranslateCM8To8(void){
	u8 *dst=(u8 *)rSTKs32(0);
	u8 *src=(u8 *)rSTKs32(1);
	int cnt=rSTKs32(2);
	u8 *map=(u8 *)rSTKs32(3);
	while(cnt>=16){
		WDu8(dst,RDu8(map+RDu8(src,0),0),0);
		WDu8(dst+1,RDu8(map+RDu8(src+1,0),0),0);
		WDu8(dst+2,RDu8(map+RDu8(src+2,0),0),0);
		WDu8(dst+3,RDu8(map+RDu8(src+3,0),0),0);
		WDu8(dst+4,RDu8(map+RDu8(src+4,0),0),0);
		WDu8(dst+5,RDu8(map+RDu8(src+5,0),0),0);
		WDu8(dst+6,RDu8(map+RDu8(src+6,0),0),0);
		WDu8(dst+7,RDu8(map+RDu8(src+7,0),0),0);
		WDu8(dst+8,RDu8(map+RDu8(src+8,0),0),0);
		WDu8(dst+9,RDu8(map+RDu8(src+9,0),0),0);
		WDu8(dst+10,RDu8(map+RDu8(src+10,0),0),0);
		WDu8(dst+11,RDu8(map+RDu8(src+11,0),0),0);
		WDu8(dst+12,RDu8(map+RDu8(src+12,0),0),0);
		WDu8(dst+13,RDu8(map+RDu8(src+13,0),0),0);
		WDu8(dst+14,RDu8(map+RDu8(src+14,0),0),0);
		WDu8(dst+15,RDu8(map+RDu8(src+15,0),0),0);
		cnt-=16;
		src+=16;
		dst+=16;
	}
	while(cnt>0){
		WDu8(dst,RDu8(map+RDu8(src,0),0),0);
		--cnt;
		++src;
		++dst;
	}
}



// translate an 8 bit colormap to 16 bits given an input color palette table
// colormap is 256 entries each to translate with
// ICVM_BltTranslateCM8To16(u16 *dst,u8 *src,int cnt,u16 *colormap)

void ICVM_BltTranslateCM8To16(void){
	u16 *dst=(u16 *)rSTKs32(0);
	u8 *src=(u8 *)rSTKs32(1);
	int cnt=rSTKs32(2);
	u16 *map=(u16 *)rSTKs32(3);
	while(cnt>=16){
		WDu16(dst,RDu16(map+RDu8(src,0),0),0);
		WDu16(dst+1,RDu16(map+RDu8(src+1,0),0),0);
		WDu16(dst+2,RDu16(map+RDu8(src+2,0),0),0);
		WDu16(dst+3,RDu16(map+RDu8(src+3,0),0),0);
		WDu16(dst+4,RDu16(map+RDu8(src+4,0),0),0);
		WDu16(dst+5,RDu16(map+RDu8(src+5,0),0),0);
		WDu16(dst+6,RDu16(map+RDu8(src+6,0),0),0);
		WDu16(dst+7,RDu16(map+RDu8(src+7,0),0),0);
		WDu16(dst+8,RDu16(map+RDu8(src+8,0),0),0);
		WDu16(dst+9,RDu16(map+RDu8(src+9,0),0),0);
		WDu16(dst+10,RDu16(map+RDu8(src+10,0),0),0);
		WDu16(dst+11,RDu16(map+RDu8(src+11,0),0),0);
		WDu16(dst+12,RDu16(map+RDu8(src+12,0),0),0);
		WDu16(dst+13,RDu16(map+RDu8(src+13,0),0),0);
		WDu16(dst+14,RDu16(map+RDu8(src+14,0),0),0);
		WDu16(dst+15,RDu16(map+RDu8(src+15,0),0),0);
		cnt-=16;
		src+=16;
		dst+=16;
	}
	while(cnt>0){
		WDu16(dst,RDu16(map+RDu8(src,0),0),0);
		--cnt;
		++src;
		++dst;
	}
}

// ICVM custom function
// translate an 8 bit colormap to 32 bits given an input color palette table
// colormap is 256 entries each to translate with
// ICVM_BltTranslateCM8To32(u32 *dst,u8 *src,int cnt,u32 *colormap)

void ICVM_BltTranslateCM8To32(void){
	u32 *dst=(u32 *)rSTKs32(0);
	u8 *src=(u8 *)rSTKs32(1);
	int cnt=rSTKs32(2);
	u32 *map=(u32 *)rSTKs32(3);
	while(cnt>=16){
		WDs32(dst,RDs32(map+RDu8(src,0),0),0);
		WDs32(dst+1,RDs32(map+RDu8(src+1,0),0),0);
		WDs32(dst+2,RDs32(map+RDu8(src+2,0),0),0);
		WDs32(dst+3,RDs32(map+RDu8(src+3,0),0),0);
		WDs32(dst+4,RDs32(map+RDu8(src+4,0),0),0);
		WDs32(dst+5,RDs32(map+RDu8(src+5,0),0),0);
		WDs32(dst+6,RDs32(map+RDu8(src+6,0),0),0);
		WDs32(dst+7,RDs32(map+RDu8(src+7,0),0),0);
		WDs32(dst+8,RDs32(map+RDu8(src+8,0),0),0);
		WDs32(dst+9,RDs32(map+RDu8(src+9,0),0),0);
		WDs32(dst+10,RDs32(map+RDu8(src+10,0),0),0);
		WDs32(dst+11,RDs32(map+RDu8(src+11,0),0),0);
		WDs32(dst+12,RDs32(map+RDu8(src+12,0),0),0);
		WDs32(dst+13,RDs32(map+RDu8(src+13,0),0),0);
		WDs32(dst+14,RDs32(map+RDu8(src+14,0),0),0);
		WDs32(dst+15,RDs32(map+RDu8(src+15,0),0),0);
		cnt-=16;
		src+=16;
		dst+=16;
	}
	while(cnt>0){
		WDs32(dst,RDs32(map+RDu8(src,0),0),0);
		--cnt;
		++src;
		++dst;
	}
}

// ******** helper macros for BltTranslate functions

// single line in x direction  - power of 2 limited
#define blttranPOW2xcvt(getfunc,putfunc) {\
	if(dheight==1){\
		s32 sx=soffsx;\
		s32 sy=soffsy;\
		int cntx=dwidth;\
		while(cntx>0){\
			putfunc(dst,\
				getfunc(map+\
					RDu8(src +\
						((sx & xmask)>>16) +\
						((sy & ymask)>>16)*swidth\
						,0\
					)\
					,0\
				)\
				,0\
			);\
			sx+=sxdx;\
			sy+=sxdy;\
			dst++;\
			--cntx;\
		}\
		return;\
	}\
}

// single line in y direction - power of two limited
#define blttranPOW2ycvt(getfunc,putfunc) {\
	if(dwidth==1){\
		s32 sx=soffsx;\
		s32 sy=soffsy;\
		int cnty=dheight;\
		if(sydx==0){\
			src+=((sx & xmask)>>16);\
			while(cnty>0){\
				putfunc(dst,\
					getfunc(map+\
						RDu8(src +\
							((sy & ymask)>>16)*swidth\
							,0\
						)\
						,0\
					)\
					,0\
				);\
				sy+=sydy;\
				dst+=screenwidth;\
				--cnty;\
			}\
			return;\
		}\
		while(cnty>0){\
			putfunc(dst,\
				getfunc(map+\
					RDu8(src +\
						((sx & xmask)>>16) +\
						((sy & ymask)>>16)*swidth\
						,0\
					)\
					,0\
				)\
				,0\
			);\
			sx+=sydx;\
			sy+=sydy;\
			dst+=screenwidth;\
			--cnty;\
		}\
		return;\
	}\
}

// power of two limited blit - bot directions
#define blttranPOW2xycvt(outtype,getfunc,putfunc) { \
		outtype *odst=dst;\
		int cnty=dheight;\
		while(cnty>0){\
			int cntx=dwidth;\
			s32 sx=soffsx;\
			s32 sy=soffsy;\
			dst=odst;\
			while(cntx>0){\
				putfunc(dst,\
					getfunc(map+\
						RDu8(src + \
							((sx & xmask)>>16) +\
							((sy & ymask)>>16)*swidth  \
							,0\
						)\
						,0\
					)\
					,0\
				);\
				sx+=sxdx;\
				sy+=sxdy;\
				--cntx;\
				++dst;\
			}\
			soffsx+=sydx;\
			soffsy+=sydy;\
			odst+=screenwidth;\
			--cnty;\
		}\
		return;\
	}\


// power of two check setup
#define blttranPOW2cget()  \
	s32 xmask=0;\
	s32 ymask=0;\
	switch(swidth){\
		case 1:\
			xmask=0xffff;\
			break;\
		case 2:\
			xmask=0x1ffff;\
			break;\
		case 4:\
			xmask=0x3ffff;\
			break;\
		case 8:\
			xmask=0x7ffff;\
			break;\
		case 16:\
			xmask=0xfffff;\
			break;\
		case 32:\
			xmask=0x1fffff;\
			break;\
		case 64:\
			xmask=0x3fffff;\
			break;\
		case 128:\
			xmask=0x7fffff;\
			break;\
		case 256:\
			xmask=0xffffff;\
			break;\
	}\
	if(xmask){\
		switch(sheight){\
			case 1:\
				ymask=0xffff;\
				break;\
			case 2:\
				ymask=0x1ffff;\
				break;\
			case 4:\
				ymask=0x3ffff;\
				break;\
			case 8:\
				ymask=0x7ffff;\
				break;\
			case 16:\
				ymask=0xfffff;\
				break;\
			case 32:\
				ymask=0x1fffff;\
				break;\
			case 64:\
				ymask=0x3fffff;\
				break;\
			case 128:\
				ymask=0x7fffff;\
				break;\
			case 256:\
				ymask=0xffffff;\
				break;\
		}\
	}


#define blttran2Scale8(offset) {\
	u8 input=RDu8(map+RDu8(src+offset,0),0);\
	WDu8(dst+offset*2,input,0);\
	WDu8(dst+1+offset*2,input,0);\
}

#define blttran2Scale16(offset) {\
	u16 input=RDu16(map+RDu8(src+offset,0),0);\
	WDu16(dst+offset*2,input,0);\
	WDu16(dst+1+offset*2,input,0);\
}

#define blttran2Scale32(offset) {\
	u32 input=RDu32(map+RDu8(src+offset,0),0);\
	WDu32(dst+offset*2,input,0);\
	WDu32(dst+1+offset*2,input,0);\
}

// check to see if image is just being doubled in size and process accordingly
#define blttranDOUBLE(desttp,tranfunc) {\
	if(sxdx==0x8000 && sxdy==0 && sydx==0 && sydy==0x8000\
			&& (dwidth & 1)==0 && (dheight & 1)==0\
			&& dwidth==swidth<<1 && dheight==sheight<<1){ \
		desttp *odst=dst+(soffsx>>16);\
		u8 *osrc=src+(soffsy>>16);\
		int cnty=dheight>>1;\
		swidth>>15;\
		while(cnty>0){\
			int cntx=dwidth;\
			dst=odst;\
			src=osrc;\
			while(cntx>=32){\
				tranfunc(0);\
				tranfunc(1);\
				tranfunc(2);\
				tranfunc(3);\
				tranfunc(4);\
				tranfunc(5);\
				tranfunc(6);\
				tranfunc(7);\
				tranfunc(8);\
				tranfunc(9);\
				tranfunc(10);\
				tranfunc(11);\
				tranfunc(12);\
				tranfunc(13);\
				tranfunc(14);\
				tranfunc(15);\
				tranfunc(16);\
				tranfunc(17);\
				tranfunc(18);\
				tranfunc(19);\
				tranfunc(20);\
				tranfunc(21);\
				tranfunc(22);\
				tranfunc(23);\
				tranfunc(24);\
				tranfunc(25);\
				tranfunc(26);\
				tranfunc(27);\
				tranfunc(28);\
				tranfunc(29);\
				tranfunc(30);\
				tranfunc(31);\
				src+=32;\
				dst+=64;\
				cntx-=64;\
			}\
			while(cntx>0){\
				tranfunc(0);\
				src++;\
				dst+=2;\
				cntx-=2;\
			}\
			osrc+=swidth;\
			memcpy(odst+screenwidth,odst,sizeof(*odst)*screenwidth);\
			odst+=screenwidth<<1;\
			--cnty;\
		}\
		return;\
	}\
}




// power of two limited blit
#define blttranPOW2cvt(outtype,getfunc,putfunc) { \
	blttranPOW2cget();\
	if(xmask && ymask){\
		{blttranPOW2xcvt(getfunc,putfunc);}\
		{blttranPOW2ycvt(getfunc,putfunc);}\
		{blttranPOW2xycvt(outtype,getfunc,putfunc);}\
	}\
}

// default blit
#define blttrandefaultcvt(outtype,getfunc,putfunc) { \
	outtype *odst=dst;\
	int cnty=dheight;\
	while(cnty>0){\
		s32 sx;\
		s32 sy;\
		int cntx=dwidth;\
		while(soffsx>=swidthfixed)\
			soffsx-=swidthfixed;\
		while(soffsx<0)\
			soffsx+=swidthfixed;\
		while(soffsy>=sheightfixed)\
			soffsy-=sheightfixed;\
		while(soffsy<0)\
			soffsy+=sheightfixed;\
		dst=odst;\
		sx=soffsx;\
		sy=soffsy;\
		while(cntx>0){\
			while(sy>=sheightfixed)\
				sy-=sheightfixed;\
			while(sy<0)\
				sy+=sheightfixed;\
			while(sx>=swidthfixed)\
				sx-=swidthfixed;\
			while(sx<0)\
				sx+=swidthfixed;\
			putfunc(dst,\
				getfunc(map+\
					RDu8(src + \
						(sx>>16) +\
						(sy>>16)*swidth  \
						,0\
					)\
					,0\
				)\
				,0\
			);\
			sx+=sxdx;\
			sy+=sxdy;\
			--cntx;\
			++dst;\
		}\
		soffsx+=sydx;\
		soffsy+=sydy;\
		odst+=screenwidth;\
		--cnty;\
	}\
}

// default blit - single line in y direction
#define blttrandefaultycvt(getfunc,putfunc) { \
	if(dwidth==1){\
		s32 sx=soffsx;\
		s32 sy=soffsy;\
		int cnty=dheight;\
		while(sx>=swidthfixed)\
			sx-=swidthfixed;\
		while(sx<0)\
			sx+=swidthfixed;\
		if(sydx==0){ \
			src+=(sx>>16);\
			while(cnty>0){\
				while(sy>=sheightfixed)\
					sy-=sheightfixed;\
				while(sy<0)\
					sy+=sheightfixed;\
				putfunc(dst,\
					getfunc(map+\
						RDu8(src + \
							(sy>>16)*swidth  \
							,0\
						)\
						,0\
					)\
					,0\
				);\
				sy+=sydy;\
				dst+=screenwidth;\
				--cnty;\
			}\
			return;\
		}\
		while(cnty>0){\
			while(sx>=swidthfixed)\
				sx-=swidthfixed;\
			while(sx<0)\
				sx+=swidthfixed;\
			while(sy>=sheightfixed)\
				sy-=sheightfixed;\
			while(sy<0)\
				sy+=sheightfixed;\
			putfunc(dst,\
				getfunc(map+\
					RDu8(src + \
						(sx>>16) +\
						(sy>>16)*swidth  \
						,0\
					)\
					,0\
				)\
				,0\
			);\
			sx+=sydx;\
			sy+=sydy;\
			dst+=screenwidth;\
			--cnty;\
		}\
		return;\
	}\
}


// default blit - single line in x direction
#define blttrandefaultxcvt(getfunc,putfunc) { \
	if(dheight==1){\
		int cntx=dwidth;\
		while(soffsy>=sheightfixed)\
			soffsy-=sheightfixed;\
		while(soffsy<0)\
			soffsy+=sheightfixed;\
		while(cntx>0){\
			while(soffsx>=swidthfixed)\
				soffsx-=swidthfixed;\
			while(soffsx<0)\
				soffsx+=swidthfixed;\
			while(soffsy>=sheightfixed)\
				soffsy-=sheightfixed;\
			while(soffsy<0)\
				soffsy+=sheightfixed;\
			putfunc(dst,\
				getfunc(map+\
					RDu8(src + \
						(soffsx>>16) +\
						(soffsy>>16)*swidth  \
						,0\
					)\
					,0\
				)\
				,0\
			);\
			soffsx+=sxdx;\
			soffsy+=sxdy;\
			dst++;\
			--cntx;\
		}\
		return;\
	}\
}

#define blitTranslateCvt(dsttype,getfunc,putfunc,scale2func){\
	dsttype *dst=(dsttype *)rSTKs32(0);\
	u8 *src=(u8 *)rSTKs32(1);\
	dsttype *map=(dsttype *)rSTKs32(2);\
	s32 swidth=rSTKs32(3);\
	s32 sheight=rSTKs32(4);\
	s32 dwidth=rSTKs32(5);\
	s32 dheight=rSTKs32(6);\
	s32 sxdx=rSTKs32(7);\
	s32 sxdy=rSTKs32(8);\
	s32 sydx=rSTKs32(9);\
	s32 sydy=rSTKs32(10);\
	s32 soffsx=rSTKs32(11);\
	s32 soffsy=rSTKs32(12);\
	s32 screenwidth=rSTKs32(13);\
	int cnty=0;\
	s32 swidthfixed=swidth<<16;\
	s32 sheightfixed=sheight<<16;\
	blttranPOW2cvt(dsttype,getfunc,putfunc);\
	blttranDOUBLE(dsttype,scale2func);\
	blttrandefaultycvt(getfunc,putfunc);\
	blttrandefaultxcvt(getfunc,putfunc);\
	blttrandefaultcvt(dsttype,getfunc,putfunc);\
}


// ************ NEED TO DOCUMENT

void ICVM_BltTranslateScaleCM8To8(void){
	blitTranslateCvt(u8,RDu8,WDu8,blttran2Scale8);
}



void ICVM_BltTranslateScaleCM8To16(void){
	blitTranslateCvt(u16,RDu16,WDu16,blttran2Scale16);
}


void ICVM_BltTranslateScaleCM8To32(void){
	blitTranslateCvt(u32,RDu32,WDu32,blttran2Scale32);
}




#endif // !__WIN32__


// base X system calls always hardwired into ICVM

SysCallFunction *XSysCalls[1024]={
#ifndef __WIN32__

	ICVM_X11TransGetConnectionNumber, 	//  4096
	ICVM_X11TransBytesReadable, 		//  4097
	ICVM_X11TransRead, 					//  4098
	ICVM_X11TransWrite, 				//  4099
	ICVM_X11TransReadv, 				//  4100
	ICVM_X11TransWritev, 				//  4101
	ICVM_X11TransGetPeerAddr, 			//  4102
	ICVM_X11TransConvertAddress, 		//  4103
	ICVM_X11TransClose, 				//  4104
	ICVM_X11TransConnect, 				//  4105
	ICVM_X11TransOpenCOTSClient, 		//  4106
	ICVM_X11TransSetOption, 			//  4107
	ICVM_X11TransDisconnect, 			//  4108
	ICVM_X11TransFreeConnInfo, 			//  4109
	0, 		//  *** 4110
	0, 		//  *** 4111
	ICVM_X11TransOpenCLTSClient, 		//  4112
	ICVM_X11TransCloseForCloning, 		//  4113
	ICVM_X11TransIsLocal, 				//  4114
	ICVM_X11TransGetMyAddr, 			//  4115
	ICVM_X11TransGetHostname, 			//  4116
	0, // 4117
	0, // 4118
	0, // 4119
	0, // 4120
	0, // 4121
	0, // 4122
	0, // 4123
	0, // 4124
	0, // 4125
	0, // 4126
	0, // 4127
	0, // 4128
	0, // 4129
	0, // 4130
	0, // 4131
	0, // 4132
	0, // 4133
	0, // 4134
	0, // 4135
	ICVM_IceTransGetConnectionNumber, 	//  4136
	ICVM_IceTransBytesReadable, 		//  4137
	ICVM_IceTransRead, 					//  4138
	ICVM_IceTransWrite, 				//  4139
	ICVM_IceTransReadv, 				//  4140
	ICVM_IceTransWritev, 				//  4141
	ICVM_IceTransGetPeerAddr, 			//  4142
	0,	//  4143
	ICVM_IceTransClose, 				//  4144
	ICVM_IceTransConnect, 				//  4145
	ICVM_IceTransOpenCOTSClient, 		//  4146
	ICVM_IceTransSetOption, 			//  4147
	ICVM_IceTransDisconnect, 			//  4148
	ICVM_IceTransFreeConnInfo, 			//  4149
	ICVM_IceTransGetPeerNetworkId, 		//  4150
	ICVM_IceTransGetMyNetworkId, 		//  4151
	ICVM_IceTransOpenCLTSClient, 		//  4152
	ICVM_IceTransCloseForCloning, 		//  4153
	ICVM_IceTransIsLocal, 				//  4154
	ICVM_IceTransGetMyAddr, 			//  4155
	ICVM_IceTransGetHostname, 			//  4156
	0, // 4157
	0, // 4158
	0, // 4159
	0, // 4160
	0, // 4161
	0, // 4162
	0, // 4163
	0, // 4164
	0, // 4165
	0, // 4166
	0, // 4167
	0, // 4168
	0, // 4169
	0, // 4170
	0, // 4171
	0, // 4172
	0, // 4173
	0, // 4174
	0, // 4175
#ifdef FONT_SERVER_SUPPORT
	ICVM_FSTransGetConnectionNumber, 	//  4176
	ICVM_FSTransBytesReadable, 			//  4177
	ICVM_FSTransRead, 					//  4178
	ICVM_FSTransWrite, 					//  4179
	ICVM_FSTransReadv, 					//  4180
	ICVM_FSTransWritev, 				//  4181
	ICVM_FSTransGetPeerAddr, 			//  4182
	0,//  4183
	ICVM_FSTransClose, 					//  4184
	ICVM_FSTransConnect, 				//  4185
	ICVM_FSTransOpenCOTSClient, 		//  4186
	ICVM_FSTransSetOption, 				//  4187
	ICVM_FSTransDisconnect, 			//  4188
	ICVM_FSTransFreeConnInfo, 			//  4189
	ICVM_BltTranslateCM8To8, 				// 4190
	ICVM_BltTranslateScaleCM8To32, 		// 4191
	ICVM_FSTransOpenCLTSClient, 		//  4192
	ICVM_FSTransCloseForCloning, 		//  4193
	ICVM_FSTransIsLocal, 				//  4194
	ICVM_FSTransGetMyAddr, 				//  4195
	ICVM_FSTransGetHostname, 			//  4196
#else
	0, // 4176
	0, // 4177
	0, // 4178
	0, // 4179
	0, // 4180
	0, // 4181
	0, // 4182
	0, // 4183
	0, // 4184
	0, // 4185
	0, // 4186
	0, // 4187
	0, // 4188
	0, // 4189
	ICVM_BltTranslateCM8To8, 				// 4190
	ICVM_BltTranslateScaleCM8To32, 		// 4191
	0, // 4192
	0, // 4193
	0, // 4194
	0, // 4195
	0, // 4196
#endif
	0, // 4197
	0, // 4198
	0, // 4199
	0, // 4200
	0, // 4201
	0, // 4202
	0, // 4203
	0, // 4204
	0, // 4205
	0, // 4206
	0, // 4207
	0, // 4208
	0, // 4209
	0, // 4210
	0, // 4211
	ICVM_BltTranslateCM8To16, // 4212
	ICVM_BltTranslateCM8To32, // 4213
	ICVM_BltTranslateScaleCM8To8, // 4214
	ICVM_BltTranslateScaleCM8To16, // 4215
	ICVM_XimXTransGetConnectionNumber, 	// 4216
	ICVM_XimXTransBytesReadable, 			// 4217
	ICVM_XimXTransRead, 					// 4218
	ICVM_XimXTransWrite, 					// 4219
	ICVM_XimXTransReadv, 					// 4220
	ICVM_XimXTransWritev, 				// 4221
	ICVM_XimXTransGetPeerAddr, 			// 4222
	0,// 4223
	ICVM_XimXTransClose, 					// 4224
	ICVM_XimXTransConnect, 				// 4225
	ICVM_XimXTransOpenCOTSClient, 		// 4226
	ICVM_XimXTransSetOption, 				// 4227
	ICVM_XimXTransDisconnect, 			// 4228
	ICVM_XimXTransFreeConnInfo, 			// 4229
	0, //  *** 4230
	0, //  *** 4231
	ICVM_XimXTransOpenCLTSClient, 		// 4232
	ICVM_XimXTransCloseForCloning, 		// 4233
	ICVM_XimXTransIsLocal, 				// 4234
	ICVM_XimXTransGetMyAddr, 				// 4235
	ICVM_XimXTransGetHostname, 			// 4236

#endif // !__WIN32__

};


