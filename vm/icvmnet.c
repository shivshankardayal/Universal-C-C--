#include "cvm.h"


// Copyright (C) 2000,2001 Bob Dayley

#ifndef __WIN32__

#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/un.h>
#include <netdb.h>
#include "icvmerr.h"
#include "icvmsys.h"

#if defined(mklinux)|| defined(__FreeBSD__)
typedef int socklen_t;
#endif


// ICVM address family  definitions
// only these are supported   currently
#define	ICVM_AF_UNSPEC		0		/* unspecified */
#define	ICVM_AF_UNIX		1		/* local to host (pipes, portals) */
#define	ICVM_AF_INET		2		/* internetwork: UDP, TCP, etc. */



static int TranslateAddressFamilyFromICVM(int inprot){
	switch(inprot){
		case ICVM_AF_UNSPEC:
			return  AF_UNSPEC;
		case ICVM_AF_UNIX:
			return  AF_UNIX;
		case ICVM_AF_INET:
			return  AF_INET;

	}
	errno=EAFNOSUPPORT;
	return -1;
}

static int TranslateAddressFamilyToICVM(int inprot){
	switch(inprot){
		case AF_UNSPEC:
			return  ICVM_AF_UNSPEC;
		case AF_UNIX:
			return  ICVM_AF_UNIX;
		case AF_INET:
			return  ICVM_AF_INET;

	}
	errno=EAFNOSUPPORT;
	return -1;
}



// ICVM protocol family  definitions
// only these are supported

#define	ICVM_PF_UNSPEC		0		/* unspecified */
#define	ICVM_PF_UNIX		1		/* local to host (pipes, portals) */
#define	ICVM_PF_INET		2		/* internetwork: UDP, TCP, etc. */



static int TranslateProtocolFamilyFromICVM(int inprot){
	switch(inprot){
		case ICVM_PF_UNSPEC:
			return  PF_UNSPEC;
		case ICVM_PF_UNIX:
			return  PF_UNIX;
		case ICVM_PF_INET:
			return  PF_INET;

	}
	errno=EPFNOSUPPORT;
	return -1;
}


// ICVM socket types

#define	ICVM_SOCK_STREAM	1		/* stream socket */
#define	ICVM_SOCK_DGRAM		2		/* datagram socket */
#define	ICVM_SOCK_RAW		3		/* raw-protocol interface */
#define	ICVM_SOCK_RDM		4		/* reliably-delivered message */
#define	ICVM_SOCK_SEQPACKET	5		/* sequenced packet stream */

static int TranslateSocketTypeFromICVM(int inprot){
	switch(inprot){
		case ICVM_SOCK_STREAM:
			return  SOCK_STREAM;
		case ICVM_SOCK_DGRAM:
			return  SOCK_DGRAM;
		case ICVM_SOCK_RAW:
			return  SOCK_RAW;
		case ICVM_SOCK_SEQPACKET:
			return  SOCK_SEQPACKET;
	}
	errno=ESOCKTNOSUPPORT;
	return -1;
}


static int TranslateSocketTypeToICVM(int inprot){
	switch(inprot){
		case SOCK_STREAM:
			return  ICVM_SOCK_STREAM;
		case SOCK_DGRAM:
			return  ICVM_SOCK_DGRAM;
		case SOCK_RAW:
			return  ICVM_SOCK_RAW;
		case SOCK_SEQPACKET:
			return  ICVM_SOCK_SEQPACKET;
	}
	errno=ESOCKTNOSUPPORT;
	return -1;
}


// generic address structure offsets

#define ICVM_sa_len		0
#define ICVM_sa_family	1
#define ICVM_sa_data	2
	
#define ICVM_sa_sizeof	16


// unix address structure offsets
#define ICVM_sun_len		0
#define ICVM_sun_family		1
#define ICVM_sun_path		2

#define ICVM_sun_sizeof		106


// internet address structures offsets 

#define ICVM_sin_len		0
#define ICVM_sin_family		1
#define ICVM_sin_port		2
#define ICVM_sin_addr		4
#define ICVM_sin_zero		8
#define ICVM_sin_sizeof		16



typedef union _large_sockaddr{
	struct sockaddr g;
	struct sockaddr_un u;
	struct sockaddr_in i;
}large_sockaddr;



#define MAXTRACKSOCKETS	256
static large_sockaddr *tracksockets[MAXTRACKSOCKETS]; // to track socket parms

static large_sockaddr *GetTrackSocket(int fd){
	if(fd==-1)
		return 0;
	if(fd>=MAXTRACKSOCKETS){
		fprintf(stderr,"FATAL: GetTrackSocket - fd too large\n");
		exit(3);
	}
	return tracksockets[fd];
}


static void WriteTrackSocket(int fd,large_sockaddr *t){
	large_sockaddr *tp=GetTrackSocket(fd);
	if(fd<0)
		return;
//	if(tp && t->g.sa_family==AF_UNSPEC)
//		t->g.sa_family=tp->g.sa_family;
	if(tp==0)
		tp=tracksockets[fd]=malloc(sizeof(large_sockaddr));
	if(tp)
		*tp=*t;
}


// translate socket address structure from ICVM to native. 
// returns 1 if successful, 0 if not supported and sets errno

static int TranslateSockAddrFromICVM(
			char  *in,
			large_sockaddr *out,
			int fdsaved,
			int fdtosave){
	int family=RDu8(in+ICVM_sa_family,0);
	large_sockaddr *saved;
 	switch(family){
		case ICVM_AF_UNSPEC:
			saved=GetTrackSocket(fdsaved);
			if(saved==0)
				break;
			*out=*saved;
			return 1;
			
		case ICVM_AF_UNIX:
			out->u.sun_family=AF_UNIX;
			strcpy(out->u.sun_path,in+ICVM_sun_path);
			WriteTrackSocket(fdtosave,out);
			return 1;

		case ICVM_AF_INET:
			out->i.sin_family=AF_INET;
			out->i.sin_port=RDs16(in+ICVM_sin_port,0);
			out->i.sin_addr.s_addr=RDs32(in+ICVM_sin_addr,0);
			WriteTrackSocket(fdtosave,out);
			return 1;

	}


	errno=EAFNOSUPPORT;
	return 0;
};



static int TranslateSockAddrToICVM(
			large_sockaddr *in,
			s8 *out,
			int savedfd,
			int fdtosave
			){
	large_sockaddr *sv;
	if(in->g.sa_family==AF_UNSPEC){
		sv=GetTrackSocket(savedfd);
		if(sv)
			in=sv;
	}
 	switch(in->g.sa_family){
		case AF_UNIX:
			WDu8(out+ICVM_sun_len,ICVM_sun_sizeof,0);
			WDu8(out+ICVM_sun_family,ICVM_AF_UNIX,0);
			strcpy(out+ICVM_sun_path,in->u.sun_path);
			WriteTrackSocket(fdtosave,in);
			return 1;

		case AF_INET:
			WDu8(out+ICVM_sin_len,ICVM_sin_sizeof,0);
			WDu8(out+ICVM_sin_family,ICVM_AF_INET,0);
			WDs16(out+ICVM_sin_port,in->i.sin_port,0);
			WDu32(out+ICVM_sin_addr,in->i.sin_addr.s_addr,0);
			WriteTrackSocket(fdtosave,in);
			return 1;

	}
	errno=EAFNOSUPPORT;
	return 0;
};

// ICVM timeval offsets

#define ICVM_tv_sec		0
#define ICVM_tv_usec	4
#define ICVM_tv_sizeof	8

void TranslateTimeValFromICVM(s8 *from,struct timeval *to){
	to->tv_sec=RDs32(from+ICVM_tv_sec,0);
	to->tv_usec=RDs32(from+ICVM_tv_usec,0);
};

static void TranslateTimeValToICVM(struct timeval *from,s8 *to){
	WDs32(to+ICVM_tv_sec,from->tv_sec,0);
	WDs32(to+ICVM_tv_usec,from->tv_usec,0);
};


void ICVM_accept(void){
	s8 *iaddr=
		(s8 *)rSTKs32(1);
	s32 *p=(s32 *)rSTKs32(2);
	int fd=rSTKs32(0);
	large_sockaddr naddr;
	int len;
	int olen=RDs32(p,0);
	len=sizeof(naddr);
	if(olen==ICVM_sun_sizeof)
		len=sizeof(struct sockaddr_un);
	if(olen==ICVM_sin_sizeof)
		len=sizeof(struct sockaddr_in);
	R_R0=accept(
			fd,
			(struct sockaddr *)&naddr,
			&len
		);
	if(R_R0>=0){
		if(!TranslateSockAddrToICVM(&naddr,iaddr,-1,R_R0)){
			close(R_R0);
			R_R0=-1;
		}else
			WDu32(p,iaddr+ICVM_sa_len,0);
	}		
	icvm_SetErr();
}



void ICVM_getsockname(void){
	s8 *iaddr=
		(s8 *)rSTKs32(1);
	s32 *p=(s32 *)rSTKs32(2);
	int fd=rSTKs32(0);
	large_sockaddr naddr;
	int len;
	int olen=RDs32(p,0);
	len=sizeof(naddr);
	if(olen==ICVM_sun_sizeof)
		len=sizeof(struct sockaddr_un);
	if(olen==ICVM_sin_sizeof)
		len=sizeof(struct sockaddr_in);
	R_R0=getsockname(
			fd,
			(struct sockaddr *)&naddr,
			&len
		);
	if(R_R0>=0){
		if(!TranslateSockAddrToICVM(&naddr,iaddr,-1,R_R0)){
			close(R_R0);
			R_R0=-1;
		}else
			WDu32(p,iaddr+ICVM_sa_len,0);
	}		
	icvm_SetErr();
}



void ICVM_bind(void){
	s8 *iaddr=
		(s8 *)rSTKs32(1);
	large_sockaddr naddr;
	int fd=rSTKs32(0);
	R_R0=-1;
	if(TranslateSockAddrFromICVM(iaddr,&naddr,fd,fd))
		R_R0=bind(
			fd,
			(struct sockaddr *)&naddr,
			rSTKs32(2)
		);
	icvm_SetErr();
}

void ICVM_connect(void){
	s8 *iaddr=
		(s8 *)rSTKs32(1);
	int fd=rSTKs32(0);
	large_sockaddr naddr;
	R_R0=-1;
	if(TranslateSockAddrFromICVM(iaddr,&naddr,fd,fd))
		R_R0=connect(
			fd,
			(struct sockaddr *)&naddr,
			rSTKs32(2)
		);
	icvm_SetErr();
}

	
void ICVM_socketpair(void){
	int af=TranslateAddressFamilyFromICVM(rSTKs32(0));
	int at=TranslateSocketTypeFromICVM(rSTKs32(1));
	int proto=rSTKs32(2);
	s8 *ptr=(s8 *)rSTKs32(3);
	int pair[2];
	R_R0=socketpair(af,at,proto,pair);
	if(ptr){
		WDs32(ptr,pair[0],0);
		WDs32(ptr+4,pair[1],0);
	}
	icvm_SetErr();
}

void ICVM_socket(void){
	int af=TranslateAddressFamilyFromICVM(rSTKs32(0));
	int at=TranslateSocketTypeFromICVM(rSTKs32(1));
	int proto=rSTKs32(2);
	R_R0=-1;
	if(af>=0 && at>=0){
		R_R0=socket(af,at,proto);
	}
	icvm_SetErr();
}

void ICVM_listen(void){
	R_R0=listen(
		rSTKs32(0),
		rSTKs32(1)
	);
	icvm_SetErr();
}



// h_errno values

#define	ICVM_HOST_NOT_FOUND	1 /* Authoritative Answer Host not found */
#define	ICVM_TRY_AGAIN		2 /* Non-Authoritive Host not found, or SERVERFAIL */
#define	ICVM_NO_RECOVERY	3 /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define	ICVM_NO_DATA		4 /* Valid name, no data record of requested type */
#define	ICVM_NO_ADDRESS	NO_DATA		/* no address, look for MX record */


static void icvm_SetHErr(void){
	icvm_SetErr();
	switch(h_errno){
		case -1:
			R_ERRNO2=-1;break;
		case 0:
			R_ERRNO2=0;break;
		case HOST_NOT_FOUND:
			R_ERRNO2=ICVM_HOST_NOT_FOUND;break;
		case NO_RECOVERY:
			R_ERRNO2=ICVM_NO_RECOVERY;break;
		case NO_DATA:
			R_ERRNO2=ICVM_NO_DATA;break;
#if NO_ADDRESS != NO_DATA
		case NO_ADDRESS:
			R_ERRNO2=ICVM_NO_ADDRESS;break;
#endif
		default:
			R_ERRNO2=-1;
			R_ERRNO1=ICVM_EUNKNOWN;
	}
}


// ICVM message flags accessors			

#define	ICVM_MSG_OOB		0x1		/* process out-of-band data */
#define	ICVM_MSG_PEEK		0x2		/* peek at incoming message */
#define	ICVM_MSG_DONTROUTE	0x4		/* send without using routing tables */
#define	ICVM_MSG_EOR		0x8		/* data completes record */
#define	ICVM_MSG_TRUNC		0x10		/* data discarded before delivery */
#define	ICVM_MSG_CTRUNC		0x20		/* control data lost before delivery */
#define	ICVM_MSG_WAITALL	0x40		/* wait for full request or error */
#define	ICVM_MSG_DONTWAIT	0x80		/* this message should be nonblocking */
#define	ICVM_MSG_BCAST		0x100		/* this message was rcvd using link-level brdcst */
#define	ICVM_MSG_MCAST		0x200		/* this message was rcvd using link-level mcast */

int TranslateMFlagsFromICVM(int in){
	int out=0;
	if(in & ICVM_MSG_OOB) 		out|=MSG_OOB;	 
	if(in & ICVM_MSG_PEEK) 		out|=MSG_PEEK;	 
	if(in & ICVM_MSG_DONTROUTE) out|=MSG_DONTROUTE;	 
#ifdef MSG_EOR
	if(in & ICVM_MSG_EOR) 		out|=MSG_EOR;	 
#endif
#ifdef MSG_TRUNC
	if(in & ICVM_MSG_TRUNC) 	out|=MSG_TRUNC;	 
#endif
	if(in & ICVM_MSG_CTRUNC) 	out|=MSG_CTRUNC;	 
#ifdef MSG_WAITALL
	if(in & ICVM_MSG_WAITALL) 	out|=MSG_WAITALL;	 
#endif
#ifdef MSG_DONTWAIT
	if(in & ICVM_MSG_DONTWAIT) 	out|=MSG_DONTWAIT;	 
#endif
#ifdef MSG_BCAST
	if(in & ICVM_MSG_BCAST) 	out|=MSG_BCAST;	 
#endif
#ifdef MSG_MCAST
	if(in & ICVM_MSG_MCAST) 	out|=MSG_MCAST;	 
#endif
	return out;
};


void ICVM_sendto(void){
	s8 *iaddr=(s8 *)rSTKs32(4);
	s32 flags=TranslateMFlagsFromICVM(rSTKs32(3));
	s32 addrlen=rSTKs32(5);
	s32 sockfd=rSTKs32(0);
	s32 nbytes=rSTKs32(2);
	char *buf=(char *)rSTKs32(1);
	large_sockaddr naddr;
	R_R0=-1;
	if(iaddr==0) // force a send here
		R_R0=send(sockfd,buf,nbytes,flags);
	else if(TranslateSockAddrFromICVM(iaddr,&naddr,sockfd,sockfd)){
		R_R0=sendto(sockfd,buf,nbytes,flags,(struct sockaddr *)(&naddr),addrlen);
	}
	icvm_SetErr();
}

void ICVM_recvfrom(void){
	s8 *iaddr=(s8 *)rSTKs32(4);
	s32 flags=TranslateMFlagsFromICVM(rSTKs32(3));
	s32 *addrlenp=(s32 *)rSTKs32(5);
	s32 sockfd=rSTKs32(0);
	s32 nbytes=rSTKs32(2);
	char *buf=(char *)rSTKs32(1);
	large_sockaddr naddr;
	socklen_t addrlen=4;
	large_sockaddr *p;
	R_R0=-1;
	if(iaddr==0){ // force a send here
// need to put IPV6 support here on length ????
#ifdef NOT_NOW
		p=GetTrackSocket(sockfd);
		if(p)
			addrlen=p->g.sa_addrlen;
#endif 
		R_R0=recv(sockfd,buf,nbytes,flags);
	}else if(TranslateSockAddrFromICVM(iaddr,&naddr,sockfd,sockfd)){
		R_R0=recvfrom(sockfd,buf,nbytes,flags,(struct sockaddr *)(&naddr),&addrlen);
	}
	if(addrlenp)
		WDs32(addrlenp,addrlen,0);
	icvm_SetErr();
}


// icvm select accesss
#define	ICVM_FD_SETSIZE	256
#define	ICVM_FD_ISSET(n, p)	(RDs32( ((s32 *)p) +((n)/32),0 ) & (1 << ((n) & 31 )))
#define	ICVM_FD_SET(n, p)	\
	WDs32( ((s32 *)p) +((n)/32), \
	RDs32( ((s32 *)p) +((n)/32),0 ) | (1 << ((n) & 31 )),0)


static void TranslateFDSetFromICVM(s32 * from,fd_set *to, int maxbits){
	int cnt=0;
	FD_ZERO(to); // clear native set first;
	while(cnt<maxbits){
		if(ICVM_FD_ISSET(cnt,from)){
			FD_SET(cnt,to);
		}
		++cnt;
	}
}

static void TranslateFDSetToICVM(fd_set *from,s32 *to, int maxbits){
	int cnt=0;
	while(cnt<8){
		WDs32(to+cnt,0,0);
		++cnt;
	}
	cnt=0;
	while(cnt<maxbits){
		if(FD_ISSET(cnt,from)){
			ICVM_FD_SET(cnt,to);
		}
		++cnt;
	}
}


void ICVM_select(void){
	s32 maxfd=rSTKs32(0);
	s32 *i_rset=(s32 *)rSTKs32(1);
	s32 *i_wset=(s32 *)rSTKs32(2);
	s32 *i_xset=(s32 *)rSTKs32(3);
	s8 	*i_tv=(s8 *)rSTKs32(4);

	fd_set rset,wset,xset;
	fd_set *rsetp=0;
	fd_set *wsetp=0;
	fd_set *xsetp=0;
	struct timeval tv;
	struct timeval *tvp=0;
	if(i_rset){
		TranslateFDSetFromICVM(i_rset,&rset,maxfd);
		rsetp=&rset;
	}
	if(i_wset){
		TranslateFDSetFromICVM(i_wset,&wset,maxfd);
		wsetp=&wset;
	}
	if(i_xset){
		TranslateFDSetFromICVM(i_xset,&xset,maxfd);
		xsetp=&xset;
	}
	if(i_tv){
		TranslateTimeValFromICVM(i_tv,&tv);
		tvp=&tv;
	}
	R_R0=select(maxfd,rsetp,wsetp,xsetp,tvp);
	if(i_rset)
		TranslateFDSetToICVM(&rset,i_rset,maxfd);
	if(i_wset)
		TranslateFDSetToICVM(&wset,i_wset,maxfd);
	if(i_xset)
		TranslateFDSetToICVM(&xset,i_xset,maxfd);
	icvm_SetErr();
}


void ICVM_getdomainname(void){
	R_R0=getdomainname(
		(char *)rSTKs32(0),
		rSTKs32(1)
	);
	icvm_SetErr();
};

void ICVM_setdomainname(void){
	R_R0=setdomainname(
		(char *)rSTKs32(0),
		rSTKs32(1)
	);
	icvm_SetErr();
};


void ICVM_shutdown(void){
	R_R0=shutdown(
		rSTKs32(0),
		rSTKs32(1)
	);
	icvm_SetErr();
}



/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */

#define	ICVM_SOL_SOCKET	0xffff		/* options for socket level */
#define	ICVM_IPPROTO_IP		0		/* dummy for IP */
#define	ICVM_IPPROTO_ICMP		1		/* control message protocol */
#define	ICVM_IPPROTO_TCP		6		/* tcp */


#define	ICVM_SO_DEBUG	0x0001		/* turn on debugging info recording */
#define	ICVM_SO_ACCEPTCONN	0x0002		/* socket has had listen() */
#define	ICVM_SO_REUSEADDR	0x0004		/* allow local address reuse */
#define	ICVM_SO_KEEPALIVE	0x0008		/* keep connections alive */
#define	ICVM_SO_DONTROUTE	0x0010		/* just use interface addresses */
#define	ICVM_SO_BROADCAST	0x0020		/* permit sending of broadcast msgs */
#define	ICVM_SO_USELOOPBACK	0x0040		/* bypass hardware when possible */
#define	ICVM_SO_LINGER		0x0080		/* linger on close if data present */
#define	ICVM_SO_OOBINLINE	0x0100		/* leave received OOB data in line */
#define	ICVM_SO_REUSEPORT	0x0200		/* allow local address & port reuse */
#define	ICVM_SO_TIMESTAMP	0x0400		/* timestamp received dgram traffic */
#define ICVM_SO_SNDBUF		0x1001		/* send buffer size */
#define ICVM_SO_RCVBUF		0x1002		/* receive buffer size */
#define ICVM_SO_SNDLOWAT	0x1003		/* send low-water mark */
#define ICVM_SO_RCVLOWAT	0x1004		/* receive low-water mark */
#define ICVM_SO_SNDTIMEO	0x1005		/* send timeout */
#define ICVM_SO_RCVTIMEO	0x1006		/* receive timeout */
#define	ICVM_SO_ERROR		0x1007		/* get error status and clear */
#define	ICVM_SO_TYPE		0x1008		/* get socket type */


// ICVM offets into linger structure

#define ICVM_l_onoff		0
#define ICVM_l_linger		4
#define ICVM_l_lingersizeof	8

#ifdef NOT_NOW

struct	linger {
	int	l_onoff;		/* option on/off */
	int	l_linger;		/* linger time in seconds */
};

#endif


#define	ICVM_IP_OPTIONS		1    /* buf/ip_opts; set/get IP options */
#define	ICVM_IP_HDRINCL		2    /* int; header is included with data */
#define	ICVM_IP_TOS			3    /* int; IP type of service and preced. */
#define	ICVM_IP_TTL			4    /* int; IP time to live */
#define	ICVM_IP_RECVOPTS		5    /* bool; receive all IP opts w/dgram */
#define	ICVM_IP_RECVRETOPTS		6    /* bool; receive IP opts for response */
#define	ICVM_IP_RECVDSTADDR		7    /* bool; receive IP dst addr w/dgram */
#define	ICVM_IP_RETOPTS		8    /* ip_opts; set/get IP options */
#define	ICVM_IP_MULTICAST_IF		9    /* in_addr; set/get IP multicast i/f  */
#define	ICVM_IP_MULTICAST_TTL	10   /* u_char; set/get IP multicast ttl */
#define	ICVM_IP_MULTICAST_LOOP	11   /* u_char; set/get IP multicast loopback */
#define	ICVM_IP_ADD_MEMBERSHIP	12   /* ip_mreq; add an IP group membership */
#define	ICVM_IP_DROP_MEMBERSHIP	13   /* ip_mreq; drop an IP group membership */
#define ICVM_IP_PORTRANGE		19   /* int; range to use for ephemeral port */
#define	ICVM_IP_RECVIF		20   /* bool; receive reception if w/dgram */
#define	ICVM_IP_ERRORMTU		21   /* int; get MTU of last xmit = EMSGSIZE */

#define	ICVM_IP_DEFAULT_MULTICAST_TTL  1	/* normally limit m'casts to 1 hop  */
#define	ICVM_IP_DEFAULT_MULTICAST_LOOP 1	/* normally hear sends if a member  */
#define	ICVM_IP_MAX_MEMBERSHIPS	20	/* per socket; must fit in one mbuf */

#ifdef NOT_NOW
/*
 * Argument structure for IP_ADD_MEMBERSHIP and IP_DROP_MEMBERSHIP.
 */
struct ip_mreq {
	struct	in_addr imr_multiaddr;	/* IP multicast address of group */
	struct	in_addr imr_interface;	/* local IP address of interface */
};

#endif

/*
 * Argument for IP_PORTRANGE:
 * - which range to search when port is unspecified at bind() or connect()
 */
#define	ICVM_IP_PORTRANGE_DEFAULT	0	/* default range */
#define	ICVM_IP_PORTRANGE_HIGH	1	/* same as DEFAULT (FreeBSD compat) */
#define	ICVM_IP_PORTRANGE_LOW	2	/* use privileged range */


#define	ICVM_TCP_NODELAY	0x01	/* don't delay send to coalesce packets */
#define	ICVM_TCP_MAXSEG		0x02	/* set maximum segment size */

static int TranslateSockOptFromICVM(int level,int inopt,int *olevel,int *errptr){
	int opt;
	*errptr=0;
	switch(level){
		case ICVM_SOL_SOCKET:
			*olevel=SOL_SOCKET;
			switch(inopt){		
				case ICVM_SO_SNDBUF:
					return SO_SNDBUF;
				case ICVM_SO_RCVBUF:
					return SO_RCVBUF;
#ifdef SO_SNDLOWAT
				case ICVM_SO_SNDLOWAT:
					return SO_SNDLOWAT;
#endif
#ifdef SO_RCVLOWAT
				case ICVM_SO_RCVLOWAT:
					return SO_RCVLOWAT;
#endif
#ifdef SO_SNDTIME0
				case ICVM_SO_SNDTIMEO:
					return SO_SNDTIMEO;
#endif
#ifdef SO_RCVTIME0
				case ICVM_SO_RCVTIMEO:
					return SO_RCVTIMEO;
#endif
				case ICVM_SO_ERROR:
					return SO_ERROR;
				case ICVM_SO_TYPE:
					return SO_TYPE;
				case ICVM_SO_DEBUG:
					return  SO_DEBUG;
#ifdef SO_ACCEPTCONN
				case ICVM_SO_ACCEPTCONN:
					return  SO_ACCEPTCONN;
#endif
				case ICVM_SO_REUSEADDR:
					return  SO_REUSEADDR;
				case ICVM_SO_KEEPALIVE:
					return  SO_KEEPALIVE;
				case ICVM_SO_DONTROUTE:
					return  SO_DONTROUTE;
				case ICVM_SO_BROADCAST:
					return  SO_BROADCAST;
#ifdef SO_USELOOPBACK
				case ICVM_SO_USELOOPBACK:
					return  SO_USELOOPBACK;
#endif
				case ICVM_SO_LINGER:
					return  SO_LINGER;
				case ICVM_SO_OOBINLINE:
					return  SO_OOBINLINE;
#ifdef SO_REUSEPORT
				case ICVM_SO_REUSEPORT:
					return  SO_REUSEPORT;
#endif
#ifdef SO_TIMESTAMP
				case ICVM_SO_TIMESTAMP:
					return  SO_TIMESTAMP;
#endif
			}
			break; // error

		case ICVM_IPPROTO_IP:
			*olevel=IPPROTO_IP;
			switch(inopt){		

#ifdef NOT_NOW		// til we figure out what the args are
				case ICVM_IP_OPTIONS:
					return IP_OPTIONS;
#endif
				case ICVM_IP_HDRINCL:
					return IP_HDRINCL;
				case ICVM_IP_TOS:
					return IP_TOS;
				case ICVM_IP_TTL:
					return IP_TTL;
#ifdef IP_RECVOPTS
				case ICVM_IP_RECVOPTS:
					return IP_RECVOPTS;
#endif
#ifdef IP_RECVRETOPTS
				case ICVM_IP_RECVRETOPTS:
					return IP_RECVRETOPTS;
#endif
#ifdef IP_RECVDSTADDR
				case ICVM_IP_RECVDSTADDR:
					return IP_RECVDSTADDR;
#endif
#ifdef IP_RETOPTS
				case ICVM_IP_RETOPTS:
					return IP_RETOPTS;
#endif
#ifdef NOT_NOW		// for now do not support MULTICAST
				case ICVM_IP_MULTICAST_IF:
					return IP_MULTICAST_IF;
				case ICVM_IP_MULTICAST_TTL:
					return IP_MULTICAST_TTL;
				case ICVM_IP_MULTICAST_LOOP:
					return IP_MULTICAST_LOOP;
				case ICVM_IP_ADD_MEMBERSHIP:
					return IP_ADD_MEMBERSHIP;
				case ICVM_IP_DROP_MEMBERSHIP:
					return IP_DROP_MEMBERSHIP;
#endif

#ifdef IP_PORTRANGE
				case ICVM_IP_PORTRANGE:
					return IP_PORTRANGE;
#endif
#ifdef IP_RECVIF
				case ICVM_IP_RECVIF:
					return IP_RECVIF;
#endif
#ifdef IP_ERRORMTU
				case ICVM_IP_ERRORMTU:
					return IP_ERRORMTU;
#endif
				
			}
			break;

		case ICVM_IPPROTO_TCP:
			*olevel=IPPROTO_TCP;
			switch(inopt){		
				case ICVM_TCP_NODELAY:
					return TCP_NODELAY;
				case ICVM_TCP_MAXSEG:
					return TCP_MAXSEG;

			}
			break;
	}
	*errptr=1;
	return -1;
}


#define ICVM_ST_int		0
#define ICVM_ST_linger	1
#define ICVM_ST_timeval	2

static int GetOptType(int level,int inopt){
	int opttype=ICVM_ST_int;
	switch(level){
		case ICVM_SOL_SOCKET:
			switch(inopt){		
				case ICVM_SO_SNDTIMEO:
				case ICVM_SO_RCVTIMEO:
					return ICVM_ST_timeval;
				case ICVM_SO_LINGER:
					return ICVM_ST_linger;
			}
			break; // error

		case ICVM_IPPROTO_IP:
			switch(inopt){		

				case ICVM_IP_OPTIONS:
					return IP_OPTIONS;
#ifdef IP_RETOPTS
				case ICVM_IP_RETOPTS:
					return IP_RETOPTS;
#endif
#ifdef NOT_NOW		// for now do not support MULTICAST
				case ICVM_IP_MULTICAST_IF:
					return IP_MULTICAST_IF;
				case ICVM_IP_MULTICAST_TTL:
					return IP_MULTICAST_TTL;
				case ICVM_IP_MULTICAST_LOOP:
					return IP_MULTICAST_LOOP;
				case ICVM_IP_ADD_MEMBERSHIP:
					return IP_ADD_MEMBERSHIP;
				case ICVM_IP_DROP_MEMBERSHIP:
					return IP_DROP_MEMBERSHIP;
#endif
				
			}
			break;

		case ICVM_IPPROTO_TCP:
			break;
	}
	return opttype;
}


void ICVM_getsockopt(void){
	s32 fd=rSTKs32(0);
	s32 ilevel=rSTKs32(1);
	s32 ioptname=rSTKs32(2);
	s32 ioptval=rSTKs32(3);
	s32 ioptlen=rSTKs32(4);
	int level,optname;
	int err;
	int opttype;
	int optval;
	socklen_t optlen;
	struct linger optlinger;
	struct timeval opttimeval;

	optname=TranslateSockOptFromICVM(ilevel,ioptname,&level,&err);
	if(err){
		R_R0=-1;
		R_ERRNO1=ICVM_EINVAL;
		return;
	}

	opttype=GetOptType(ilevel,ioptname);


	switch(opttype){
		case ICVM_ST_int:
			optlen=sizeof(optval);
			R_R0=getsockopt(fd,level,optname,&optval,&optlen);
			WDs32(ioptval,optval,0);
			WDs32(ioptlen,4,0);
			break;

		case ICVM_ST_linger:
			optlen=sizeof(optlinger);
			R_R0=getsockopt(fd,level,optname,&optlinger,&optlen);
			WDs32(ioptval+ICVM_l_onoff,optlinger.l_onoff,0);
			WDs32(ioptval+ICVM_l_linger,optlinger.l_linger,0);
			WDs32(ioptlen,ICVM_l_lingersizeof,0);
			break;

		case ICVM_ST_timeval:
			optlen=sizeof(opttimeval);
			R_R0=getsockopt(fd,level,optname,&opttimeval,&optlen);
			TranslateTimeValToICVM(&opttimeval,(s8 *)ioptval);
			WDs32(ioptlen,ICVM_tv_sizeof,0);
			break;



	}

	icvm_SetErr();
}

void ICVM_setsockopt(void){
	s32 fd=rSTKs32(0);
	s32 ilevel=rSTKs32(1);
	s32 ioptname=rSTKs32(2);
	s32 ioptval=rSTKs32(3);
	s32 ioptlen=rSTKs32(4);
	int level,optname;
	int err;
	int opttype;
	int optval;
	struct linger optlinger;
	struct timeval opttimeval;

	optname=TranslateSockOptFromICVM(ilevel,ioptname,&level,&err);
	opttype=GetOptType(ilevel,ioptname);

	if(err){
		R_R0=-1;
		R_ERRNO1=ICVM_EINVAL;
		return;
	}




	switch(opttype){
		case ICVM_ST_int:
			optval=RDs32(ioptval,0);
			R_R0=setsockopt(fd,level,optname,&optval,sizeof(optval));
			break;

		case ICVM_ST_linger:
			optlinger.l_onoff=RDs32(ioptval+ICVM_l_onoff,0);
			optlinger.l_linger=RDs32(ioptval+ICVM_l_linger,0);
			R_R0=setsockopt(fd,level,optname,&optlinger,sizeof(optlinger));
			break;

		case ICVM_ST_timeval:
			TranslateTimeValFromICVM((s8 *)ioptval,&opttimeval);
			R_R0=setsockopt(fd,level,optname,&opttimeval,sizeof(opttimeval));
			break;



	}


	icvm_SetErr();
}



//*********** STUFF BELOW MAY, OR MAY NOT GET USED *****************




#define MAX_ALIASES		64
#define MAX_ADDRESSES	64


static u8	h_addresses[4*(MAX_ADDRESSES+1)];
static p32	h_addresspointers[MAX_ADDRESSES+1];
static p32 	h_aliases[MAX_ALIASES+1];
static p32 	n_aliases[MAX_ALIASES+1];
static p32 	s_aliases[MAX_ALIASES+1];
static p32 	p_aliases[MAX_ALIASES+1];


// ICVM hostent offsets 
#define ICVM_h_name			0
#define ICVM_h_aliases		4
#define ICVM_h_addrtype		8
#define ICVM_h_length		12
#define ICVM_h_addr_list	16
#define ICVM_h_sizeof		20

typedef	s8 ICVM_hostent[ICVM_h_sizeof];


// ICVM netent offsets 

#define ICVM_n_name			0
#define ICVM_n_aliases		4
#define ICVM_n_addrtype		8
#define ICVM_n_net			12
#define ICVM_n_sizeof		16

typedef	s8 ICVM_netent[ICVM_n_sizeof];

// ICVM servent offsets 

#define ICVM_s_name			0		
#define ICVM_s_aliases		4
#define ICVM_s_port			8
#define ICVM_s_proto		12
#define ICVM_s_sizeof		16

typedef	s8 ICVM_servent[ICVM_s_sizeof];

// ICVM protoent offsets 

#define ICVM_p_name			0
#define ICVM_p_aliases		4
#define ICVM_p_proto		8
#define ICVM_p_sizeof		12

typedef	s8 ICVM_protoent[ICVM_p_sizeof];



static s8 * BuildICVMHostEnt(struct hostent *in){
	static ICVM_hostent _h;
	s8 *h=(s8 *)&_h;
	char **s;
	int n;
	WDu32(h+ICVM_h_name,in->h_name,0);
	WDu32(h+ICVM_h_aliases,h_aliases,0);
	WDu32(h+ICVM_h_addr_list,h_addresspointers,0);
	WDu32(h+ICVM_h_addrtype,
		TranslateAddressFamilyToICVM(in->h_addrtype),0
	);
	WDu32(h+ICVM_h_length,4,0); // 4 byte addresses
	n=0;
	s=in->h_addr_list;
	while(*s && n<MAX_ADDRESSES){
		WDu32(&(h_addresspointers[n]),&(h_addresses[n*4]),0); 
		WDu8(&(h_addresses[n*4]),*(*s),0 ); 
		WDu8(&(h_addresses[n*4+1]),*((*s)+1),0 ); 
		WDu8(&(h_addresses[n*4+2]),*((*s)+2),0 ); 
		WDu8(&(h_addresses[n*4+3]),*((*s)+3),0 ); 
		++s;
		++n;
	}
	WDu32(&(h_addresspointers[n]),0,0); 

	n=0;
	s=in->h_aliases;
	while(*s && n<MAX_ALIASES){
		WDu32(&(h_aliases[n]),*s,0); 
		++s;
		++n;
	}
	WDu32(&(h_aliases[n]),0,0); 
	return h;
}; 


void ICVM_gethostbyname(void){
	s8 * ih=0;
	struct hostent *h=gethostbyname(
		(char *)rSTKs32(0)
	);
	if(h)
		ih=BuildICVMHostEnt(h);
	R_R0=(p32)ih;
	icvm_SetHErr();
};


void ICVM_gethostbyaddr(void){
	s8 * ih=0;
	p32 addrptr =rSTKu32(0);
	s32 addr=RDs32(addrptr,0);
	s32 sz=rSTKu32(1);
	s32 tp=rSTKu32(2);
	struct hostent *h=0;
	if(sz==4 && tp==ICVM_AF_INET){
		h=gethostbyaddr((char *)(&addr),sizeof(s32),AF_INET);
		if(h)
			ih=BuildICVMHostEnt(h);
	}else{
		R_R0=0;
		R_ERRNO1=ICVM_EINVAL;
		R_ERRNO2=-1;
		return;
	}
	R_R0=(p32)ih;
	icvm_SetHErr();
};

// NEW STUFF


void ICVM_gethostbyname2(void){
	s8 * ih=0;
	s32 tp=rSTKu32(1);
	struct hostent *h;
	if(tp==ICVM_AF_INET){
		h=gethostbyname(
			(char *)rSTKs32(0)
		);
	}else{
		R_R0=0;
		R_ERRNO1=ICVM_EINVAL;
		R_ERRNO2=-1;
		return;
	}
	if(h)
		ih=BuildICVMHostEnt(h);
	R_R0=(p32)ih;
	icvm_SetHErr();
};



void ICVM_gethostent(void){
#if defined(__NetBSD__) // ??? not here ???? 
   R_ERRNO1=ICVM_EOPNOTSUPP;
   R_R0=0;
#else	

	struct hostent *h=0;
	s8 * ih=0;
	h=gethostent();
	if(h)
		ih=BuildICVMHostEnt(h);
	R_R0=(p32)ih;
	icvm_SetHErr();
#endif
};



void ICVM_sethostent(void){
	sethostent(rSTKs32(0));
	icvm_SetErr();
};

void ICVM_endhostent(void){
	endhostent();
	icvm_SetErr();
};



static s8 * BuildICVMNetEnt(struct netent *in){
	static ICVM_netent _h;
	s8 *h=(s8 *)&_h;
	char **s;
	int n;
	WDu32(h+ICVM_n_name,in->n_name,0);
	WDu32(h+ICVM_n_aliases,n_aliases,0);
	WDu32(h+ICVM_n_addrtype,
		TranslateAddressFamilyToICVM(in->n_addrtype),0
	);
	WDu32(h+ICVM_n_net,RDu32(&in->n_net,0),0); // 4 byte addresses

	n=0;
	s=in->n_aliases;
	while(*s && n<MAX_ALIASES){
		WDu32(&(n_aliases[n]),*s,0); 
		++s;
		++n;
	}
	WDu32(&(n_aliases[n]),0,0); 
	return h;
}; 


void ICVM_getnetent(void){
	struct netent *h=0;
	s8 * ih=0;
	h=getnetent();
	if(h)
		ih=BuildICVMNetEnt(h);
	R_R0=(p32)ih;
	icvm_SetErr();
};


void ICVM_getnetbyname(void){
	struct netent *h=0;
	s8 * ih=0;
	h=getnetbyname(
		(char *)rSTKu32(0)
	);
	if(h)
		ih=BuildICVMNetEnt(h);
	R_R0=(p32)ih;
	icvm_SetErr();
};


void ICVM_getnetbyaddr(void){
	struct netent *h=0;
	s8 * ih=0;
	h=getnetbyaddr(
		rSTKu32(0),
		TranslateAddressFamilyFromICVM(rSTKu32(1))
	);
	if(h)
		ih=BuildICVMNetEnt(h);
	R_R0=(p32)ih;
	icvm_SetErr();
};



void ICVM_setnetent(void){
	setnetent(rSTKs32(0));
	icvm_SetErr();
};

void ICVM_endnetent(void){
	endnetent();
	icvm_SetErr();
};


static s8 * BuildICVMServEnt(struct servent *in){
	static ICVM_servent _h;
	s8 *h=(s8 *)&_h;
	char **s;
	int n;
	WDu32(h+ICVM_s_name,in->s_name,0);
	WDu32(h+ICVM_s_aliases,s_aliases,0);
	WDu32(h+ICVM_s_port,in->s_port,0);
	WDu32(h+ICVM_s_proto,in->s_proto,0);

	n=0;
	s=in->s_aliases;
	while(*s && n<MAX_ALIASES){
		WDu32(&(s_aliases[n]),*s,0); 
		++s;
		++n;
	}
	WDu32(&(s_aliases[n]),0,0); 
	return h;
}; 



void ICVM_setservent(void){
	setservent(rSTKs32(0));
	icvm_SetErr();
};

void ICVM_endservent(void){
	endservent();
	icvm_SetErr();
};



void ICVM_getservent(void){
	struct servent *h=0;
	s8 * ih=0;
	h=getservent();
	if(h)
		ih=BuildICVMServEnt(h);
	R_R0=(p32)ih;
	icvm_SetErr();
};

void ICVM_getservbyname(void){
	struct servent *h=0;
	s8 * ih=0;
	h=getservbyname(
		(char *)rSTKu32(0),
		(char *)rSTKu32(1)
	);
	if(h)
		ih=BuildICVMServEnt(h);
	R_R0=(p32)ih;
	icvm_SetErr();
};


void ICVM_getservbyport(void){
	struct servent *h=0;
	s8 * ih=0;
	h=getservbyport(
		rSTKu32(0),
		(char *)rSTKu32(1)
	);
	if(h)
		ih=BuildICVMServEnt(h);
	R_R0=(p32)ih;
	icvm_SetErr();
};


static s8 * BuildICVMProtoEnt(struct protoent *in){
	static ICVM_protoent _h;
	s8 *h=(s8 *)&_h;
	char **s;
	int n;
	WDu32(h+ICVM_p_name,in->p_name,0);
	WDu32(h+ICVM_p_aliases,p_aliases,0);
	WDu32(h+ICVM_p_proto,in->p_proto,0);

	n=0;
	s=in->p_aliases;
	while(*s && n<MAX_ALIASES){
		WDu32(&(p_aliases[n]),*s,0); 
		++s;
		++n;
	}
	WDu32(&(p_aliases[n]),0,0); 
	return h;
}; 



void ICVM_setprotoent(void){
	setprotoent(rSTKs32(0));
	icvm_SetErr();
};

void ICVM_endprotoent(void){
	endprotoent();
	icvm_SetErr();
};



void ICVM_getprotoent(void){
	struct protoent *h=0;
	s8 * ih=0;
	h=getprotoent();
	if(h)
		ih=BuildICVMProtoEnt(h);
	R_R0=(p32)ih;
	icvm_SetErr();
};

void ICVM_getprotobyname(void){
	struct protoent *h=0;
	s8 * ih=0;
	h=getprotobyname(
		(char *)rSTKu32(0)
	);
	if(h)
		ih=BuildICVMProtoEnt(h);
	R_R0=(p32)ih;
	icvm_SetErr();
};


void ICVM_getprotobynumber(void){
	struct protoent *h=0;
	s8 * ih=0;
	h=getprotobynumber(
		rSTKu32(0)
	);
	if(h)
		ih=BuildICVMProtoEnt(h);
	R_R0=(p32)ih;
	icvm_SetErr();
};


#endif // !__WIN32__



