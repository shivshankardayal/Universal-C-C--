
CC=igcc ${INCLUDES}  -O2  -c 
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
__dn_comp.o \
__res_close.o \
__res_send.o \
base64.o \
gethnamaddr.o \
getnetnamadr.o \
getnetent.o \
getproto.o \
getprotoent.o \
getprotoname.o \
getservbyname.o \
getservbyport.o \
getservent.o \
herror.o \
hesiod.o \
inet_lnaof.o \
inet_makeaddr.o \
inet_net_ntop.o \
inet_net_pton.o \
inet_neta.o \
inet_netof.o \
inet_network.o \
inet_ntoa.o \
inet_ntop.o \
inet_pton.o \
iso_addr.o \
linkaddr.o \
ns_addr.o \
ns_ntoa.o \
nsap_addr.o \
nsdispatch.o \
poll.o \
rcmd.o \
recv.o \
res_comp.o \
res_data.o \
res_debug.o \
res_init.o \
res_mkquery.o \
res_query.o \
res_send.o \
send.o \
sethostent.o \
nsparser.o \
nslexer.o 

all: ${OBJ} net.a

net.a: ${OBJ}
	touch net.a
	rm net.a
	${AR} net.a ${OBJ}


clean:
	rm *.o *.a
	rm nslexer.c nsparser.c nsparser.h

nsparser.o: nsparser.y
	yacc -p_nsyy -bnsparser -d nsparser.y
	mv nsparser.tab.c nsparser.c
	mv nsparser.tab.h nsparser.h
	${CC} $*.c nsparser.c


nslexer.o: nslexer.l nsparser.h
	lex -P_nsyy -otmp.lex  nslexer.l 
	cat tmp.lex | sed -e '/YY_BUF_SIZE/s/16384/1024/' >nslexer.c
	rm tmp.lex
	${CC} $*.c nslexer.c

 