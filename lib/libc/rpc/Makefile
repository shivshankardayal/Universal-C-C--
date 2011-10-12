
D2= 
D3=-D_BSD_VA_LIST_="char *"
														

INC1=-I../../../include
INC2=-I../../../include/sys
INCLUDES= ${INC1} ${INC2}
										
																																															
CC=igcc  ${D2} ${D3}  ${INCLUDES} -O2  -c  -nostdinc
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
auth_none.o \
auth_unix.o \
authunix_prot.o \
bindresvport.o \
clnt_generic.o \
clnt_perror.o \
clnt_raw.o \
clnt_simple.o \
clnt_tcp.o \
clnt_udp.o \
get_myaddress.o \
getrpcent.o \
getrpcport.o \
pmap_clnt.o \
pmap_getmaps.o \
pmap_getport.o \
pmap_prot.o \
pmap_prot2.o \
pmap_rmt.o \
rpc_callmsg.o \
rpc_commondata.o \
rpc_dtablesize.o \
rpc_prot.o \
svc.o \
svc_auth.o \
svc_auth_unix.o \
svc_raw.o \
svc_run.o \
svc_simple.o \
svc_tcp.o \
svc_udp.o \
xdr.o \
xdr_array.o \
xdr_float.o \
xdr_mem.o \
xdr_rec.o \
xdr_reference.o \
xdr_stdio.o 


	
all: ${OBJ} rpc.a

rpc.a: ${OBJ}
	touch rpc.a
	rm rpc.a
	${AR} rpc.a ${OBJ}


clean:
	rm *.o *.a

ctime.o : ctime.c
	${CC} -fno-strength-reduce ctime.c
	 