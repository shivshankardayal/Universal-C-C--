
#define MAKESYSCALL(name,callno)\
	  asm("\n_"name":");\
	  asm("movel (%sp)+,(-112,%fp)");\
	  asm("syscalll %0": : "i" (callno) );\
	  asm(".globl _"name);\
	  asm("movel (-112,%fp),-(%sp)");\
	  asm("rts");

#define MAKEERRSYSCALL(name,callno)\
	  asm("\n_"name":");\
	  asm("movel (%sp)+,(-112,%fp)");\
	  asm("syscalll %0": : "i" (callno) );\
	  asm(".globl _"name);\
	  asm("movel (-116,%fp),_errno"); \
	  asm("movel (-112,%fp),-(%sp)");\
	  asm("rts");



#include <sys/xsyscall.h>
#include <unistd.h>

/* surrounded with a dummy call so we can access system header files.
   these actually turn to jumps into the Host handlers 
   so we just need to compile
   the jump vectors.   */


/* basic operating level system calls. 
*/

static void dummy(void){
	MAKEERRSYSCALL("_X11TransGetConnectionNumber",SYS_X11TransGetConnectionNumber);
	MAKEERRSYSCALL("_X11TransBytesReadable",SYS_X11TransBytesReadable);
	MAKEERRSYSCALL("_X11TransRead",SYS_X11TransRead);
	MAKEERRSYSCALL("_X11TransWrite",SYS_X11TransWrite);
	MAKEERRSYSCALL("_X11TransReadv",SYS_X11TransReadv);
	MAKEERRSYSCALL("_X11TransWritev",SYS_X11TransWritev);
	MAKEERRSYSCALL("_X11TransGetPeerAddr",SYS_X11TransGetPeerAddr);
	MAKEERRSYSCALL("_X11TransGetMyAddr",SYS_X11TransGetMyAddr);
	MAKEERRSYSCALL("_X11TransConvertAddress",SYS_X11TransConvertAddress);
	MAKEERRSYSCALL("_X11TransClose",SYS_X11TransClose);
	MAKEERRSYSCALL("_X11TransIsLocal",SYS_X11TransIsLocal);
	MAKEERRSYSCALL("_X11TransCloseForCloning",SYS_X11TransCloseForCloning);
	MAKEERRSYSCALL("_X11TransConnect",SYS_X11TransConnect);
	MAKEERRSYSCALL("_X11TransOpenCOTSClient",SYS_X11TransOpenCOTSClient);
	MAKEERRSYSCALL("_X11TransOpenCLTSClient",SYS_X11TransOpenCLTSClient);
	MAKEERRSYSCALL("_X11TransSetOption",SYS_X11TransSetOption);
	MAKEERRSYSCALL("_X11TransDisconnect",SYS_X11TransDisconnect);
	MAKEERRSYSCALL("_X11TransFreeConnInfo",SYS_X11TransFreeConnInfo);
	MAKEERRSYSCALL("_X11TransGetHostname",SYS_X11TransGetHostname);


	MAKEERRSYSCALL("_IceTransGetConnectionNumber",SYS_IceTransGetConnectionNumber);
	MAKEERRSYSCALL("_IceTransBytesReadable",SYS_IceTransBytesReadable);
	MAKEERRSYSCALL("_IceTransRead",SYS_IceTransRead);
	MAKEERRSYSCALL("_IceTransWrite",SYS_IceTransWrite);
	MAKEERRSYSCALL("_IceTransReadv",SYS_IceTransReadv);
	MAKEERRSYSCALL("_IceTransWritev",SYS_IceTransWritev);
	MAKEERRSYSCALL("_IceTransGetPeerAddr",SYS_IceTransGetPeerAddr);
	MAKEERRSYSCALL("_IceTransGetMyAddr",SYS_IceTransGetMyAddr);
	MAKEERRSYSCALL("_IceTransClose",SYS_IceTransClose);
	MAKEERRSYSCALL("_IceTransIsLocal",SYS_IceTransIsLocal);
	MAKEERRSYSCALL("_IceTransCloseForCloning",SYS_IceTransCloseForCloning);
	MAKEERRSYSCALL("_IceTransConnect",SYS_IceTransConnect);
	MAKEERRSYSCALL("_IceTransOpenCOTSClient",SYS_IceTransOpenCOTSClient);
	MAKEERRSYSCALL("_IceTransOpenCLTSClient",SYS_IceTransOpenCLTSClient);
	MAKEERRSYSCALL("_IceTransSetOption",SYS_IceTransSetOption);
	MAKEERRSYSCALL("_IceTransDisconnect",SYS_IceTransDisconnect);
	MAKEERRSYSCALL("_IceTransFreeConnInfo",SYS_IceTransFreeConnInfo);
	MAKEERRSYSCALL("_IceTransGetPeerNetworkId",SYS_IceTransGetPeerNetworkId);
	MAKEERRSYSCALL("_IceTransGetMyNetworkId",SYS_IceTransGetMyNetworkId);
	MAKEERRSYSCALL("_IceTransGetHostname",SYS_IceTransGetHostname);


	MAKEERRSYSCALL("_FSTransGetConnectionNumber",SYS_FSTransGetConnectionNumber);
	MAKEERRSYSCALL("_FSTransBytesReadable",SYS_FSTransBytesReadable);
	MAKEERRSYSCALL("_FSTransRead",SYS_FSTransRead);
	MAKEERRSYSCALL("_FSTransWrite",SYS_FSTransWrite);
	MAKEERRSYSCALL("_FSTransReadv",SYS_FSTransReadv);
	MAKEERRSYSCALL("_FSTransWritev",SYS_FSTransWritev);
	MAKEERRSYSCALL("_FSTransGetPeerAddr",SYS_FSTransGetPeerAddr);
	MAKEERRSYSCALL("_FSTransGetMyAddr",SYS_FSTransGetMyAddr);
	MAKEERRSYSCALL("_FSTransClose",SYS_FSTransClose);
	MAKEERRSYSCALL("_FSTransIsLocal",SYS_FSTransIsLocal);
	MAKEERRSYSCALL("_FSTransCloseForCloning",SYS_FSTransCloseForCloning);
	MAKEERRSYSCALL("_FSTransConnect",SYS_FSTransConnect);
	MAKEERRSYSCALL("_FSTransOpenCOTSClient",SYS_FSTransOpenCOTSClient);
	MAKEERRSYSCALL("_FSTransOpenCLTSClient",SYS_FSTransOpenCLTSClient);
	MAKEERRSYSCALL("_FSTransSetOption",SYS_FSTransSetOption);
	MAKEERRSYSCALL("_FSTransDisconnect",SYS_FSTransDisconnect);
	MAKEERRSYSCALL("_FSTransFreeConnInfo",SYS_FSTransFreeConnInfo);
	MAKEERRSYSCALL("_FSTransGetHostname",SYS_FSTransGetHostname);

	MAKEERRSYSCALL("_XimXTransGetConnectionNumber",SYS_XimXTransGetConnectionNumber);
	MAKEERRSYSCALL("_XimXTransBytesReadable",SYS_XimXTransBytesReadable);
	MAKEERRSYSCALL("_XimXTransRead",SYS_XimXTransRead);
	MAKEERRSYSCALL("_XimXTransWrite",SYS_XimXTransWrite);
	MAKEERRSYSCALL("_XimXTransReadv",SYS_XimXTransReadv);
	MAKEERRSYSCALL("_XimXTransWritev",SYS_XimXTransWritev);
	MAKEERRSYSCALL("_XimXTransGetPeerAddr",SYS_XimXTransGetPeerAddr);
	MAKEERRSYSCALL("_XimXTransGetMyAddr",SYS_XimXTransGetMyAddr);
	MAKEERRSYSCALL("_XimXTransClose",SYS_XimXTransClose);
	MAKEERRSYSCALL("_XimXTransIsLocal",SYS_XimXTransIsLocal);
	MAKEERRSYSCALL("_XimXTransCloseForCloning",SYS_XimXTransCloseForCloning);
	MAKEERRSYSCALL("_XimXTransConnect",SYS_XimXTransConnect);
	MAKEERRSYSCALL("_XimXTransOpenCOTSClient",SYS_XimXTransOpenCOTSClient);
	MAKEERRSYSCALL("_XimXTransOpenCLTSClient",SYS_XimXTransOpenCLTSClient);
	MAKEERRSYSCALL("_XimXTransSetOption",SYS_XimXTransSetOption);
	MAKEERRSYSCALL("_XimXTransDisconnect",SYS_XimXTransDisconnect);
	MAKEERRSYSCALL("_XimXTransFreeConnInfo",SYS_XimXTransFreeConnInfo);
	MAKEERRSYSCALL("_XimXTransGetHostname",SYS_XimXTransGetHostname);



}





