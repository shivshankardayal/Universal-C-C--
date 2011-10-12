#ifndef ICVMERR_H
#define ICVMERR_H

// Copyright (C) 2000,2001 Bob Dayley

// ICVM error codes

#define ICVM_EPERM		1
#define ICVM_ENOENT		2
#define ICVM_ESRCH		3
#define ICVM_EINTR		4
#define ICVM_EIO		5
#define ICVM_ENXIO		6
#define ICVM_E2BIG		7
#define ICVM_ENOEXEC	8
#define ICVM_EBADF		9
#define ICVM_ECHILD		10
#define ICVM_EDEADLK	11
#define ICVM_ENOMEM		12
#define ICVM_EACCES		13
#define ICVM_EFAULT		14
#define ICVM_ENOTBLK	15
#define ICVM_EBUSY		16
#define ICVM_EEXIST		17
#define ICVM_EXDEV		18
#define ICVM_ENODEV		19
#define ICVM_ENOTDIR	20
#define ICVM_EISDIR		21
#define ICVM_EINVAL		22
#define ICVM_ENFILE		23
#define ICVM_EMFILE		24
#define ICVM_ENOTTY		25
#define ICVM_ETXTBSY	26	
#define ICVM_EFBIG		27
#define ICVM_ENOSPC		28
#define ICVM_ESPIPE		29
#define ICVM_EROFS		30
#define ICVM_EMLINK		31
#define ICVM_EPIPE		32
#define ICVM_EDOM		33
#define ICVM_ERANGE		34
#define ICVM_EAGAIN		35
#define ICVM_EWOULDBLOCK EAGAIN
#define ICVM_EINPROGRESS 36
#define ICVM_EALREADY	37
#define ICVM_ENOTSOCK	38
#define ICVM_EDESTADDRREQ	39
#define ICVM_EMSGSIZE	40
#define ICVM_EPROTOTYPE	41
#define ICVM_ENOPROTOOPT	42
#define ICVM_EPROTONOSUPPORT	43	
#define ICVM_ESOCKTNOSUPPORT	44	
#define ICVM_EOPNOTSUPP		45	
#define ICVM_EPFNOSUPPORT	46
#define ICVM_EAFNOSUPPORT	47
#define ICVM_EADDRINUSE		48
#define ICVM_EADDRNOTAVAIL	49
#define ICVM_ENETDOWN		50
#define ICVM_ENETUNREACH	51
#define ICVM_ENETRESET		52
#define ICVM_ECONNABORTED	53
#define ICVM_ECONNRESET		54
#define ICVM_ENOBUFS		55
#define ICVM_EISCONN		56
#define ICVM_ENOTCONN		57
#define ICVM_ESHUTDOWN		58	
#define ICVM_ETOOMANYREFS	59
#define ICVM_ETIMEDOUT		60
#define ICVM_ECONNREFUSED	61
#define ICVM_ELOOP			62
#define ICVM_ENAMETOOLONG	63
#define ICVM_EHOSTDOWN		64
#define ICVM_EHOSTUNREACH	65
#define ICVM_ENOTEMPTY		66
#define ICVM_EPROCLIM		67
#define ICVM_EUSERS			68
#define ICVM_EDQUOT			69
#define ICVM_ESTALE			70
#define ICVM_EREMOTE		71
#define ICVM_EBADRPC		72
#define ICVM_ERPCMISMATCH	73
#define ICVM_EPROGUNAVAIL	74
#define ICVM_EPROGMISMATCH	75
#define ICVM_EPROCUNAVAIL	76
#define ICVM_ENOLCK			77
#define ICVM_ENOSYS			78
#define ICVM_EFTYPE			79

#define ICVM_EUNKNOWN		1000

#ifdef __cplusplus
extern "C"{
#endif

extern int i_errno;


int TranslateErrToICVM(int err);

#define i_seterr(err) i_errno=err
#define i_geterr() i_errno

#ifdef __WIN32__
#define i_settranslatederr() i_errno=TranslateErrToICVM(GetLastError())
#else
#define i_settranslatederr() i_errno=TranslateErrToICVM(errno)
#endif

#define icvm_returnerr() R_ERRNO1=i_geterr()


// to be obsolet
void icvm_SetErr(void);

#ifdef __cplusplus
};
#endif





#endif

