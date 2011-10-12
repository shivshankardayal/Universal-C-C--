// ICVM x-system calls

#ifndef XSYSCALL_H
#define XSYSCALL_H
#include <sys/syscall.h>

#define SYSXCALL(x) (SYS_XCALLS +(x))

#define SYS_X11TransGetConnectionNumber		4096		// 4096
#define SYS_X11TransBytesReadable			4097		// 4097
#define SYS_X11TransRead					4098		// 4098
#define SYS_X11TransWrite					4099		// 4099
#define SYS_X11TransReadv					4100		// 4100
#define SYS_X11TransWritev					4101		// 4101
#define SYS_X11TransGetPeerAddr				4102		// 4102
#define SYS_X11TransConvertAddress			4103		// 4103
#define SYS_X11TransClose					4104		// 4104
#define SYS_X11TransConnect					4105		// 4105
#define SYS_X11TransOpenCOTSClient			4106		// 4106
#define SYS_X11TransSetOption				4107		// 4107
#define SYS_X11TransDisconnect				4108		// 4108
#define SYS_X11TransFreeConnInfo			4109		// 4109
		// 4110
	 //  4111

#define SYS_X11TransOpenCLTSClient			4112		// 4112
#define SYS_X11TransCloseForCloning			4113		// 4113
#define SYS_X11TransIsLocal					4114		// 4114
#define SYS_X11TransGetMyAddr				4115		// 4115
#define SYS_X11TransGetHostname				4116		// 4116



#define SYS_IceTransGetConnectionNumber		4136		// 4136
#define SYS_IceTransBytesReadable			4137		// 4137
#define SYS_IceTransRead					4138		// 4138
#define SYS_IceTransWrite					4139		// 4139
#define SYS_IceTransReadv					4140		// 4140
#define SYS_IceTransWritev					4141		// 4141
#define SYS_IceTransGetPeerAddr				4142		// 4142
// 4143
#define SYS_IceTransClose					4144		// 4144
#define SYS_IceTransConnect					4145		// 4145
#define SYS_IceTransOpenCOTSClient			4146		// 4146
#define SYS_IceTransSetOption				4147		// 4147
#define SYS_IceTransDisconnect				4148		// 4148
#define SYS_IceTransFreeConnInfo			4149		// 4149
#define SYS_IceTransGetPeerNetworkId		4150		// 4150
#define SYS_IceTransGetMyNetworkId			4151		// 4151
#define SYS_IceTransOpenCLTSClient			4152		// 4152
#define SYS_IceTransCloseForCloning			4153		// 4153
#define SYS_IceTransIsLocal					4154		// 4154
#define SYS_IceTransGetMyAddr				4155		// 4155
#define SYS_IceTransGetHostname				4156		// 4156

#define SYS_FSTransGetConnectionNumber		4176		// 4176
#define SYS_FSTransBytesReadable			4177		// 4177
#define SYS_FSTransRead						4178		// 4178
#define SYS_FSTransWrite					4179		// 4179
#define SYS_FSTransReadv					4180		// 4180
#define SYS_FSTransWritev					4181		// 4181
#define SYS_FSTransGetPeerAddr				4182		// 4182
// 4183
#define SYS_FSTransClose					4184		// 4184
#define SYS_FSTransConnect					4185		// 4185
#define SYS_FSTransOpenCOTSClient			4186		// 4186
#define SYS_FSTransSetOption				4187		// 4187
#define SYS_FSTransDisconnect				4188		// 4188
#define SYS_FSTransFreeConnInfo				4189		// 4189

#define SYS_BltTranslateCM8To8				4190		// 4190
#define SYS_BltTranslateScaleCM8To32		4191		// 4191


#define SYS_FSTransOpenCLTSClient			4192		// 4192
#define SYS_FSTransCloseForCloning			4193		// 4193
#define SYS_FSTransIsLocal					4194		// 4194
#define SYS_FSTransGetMyAddr				4195		// 4195
#define SYS_FSTransGetHostname				4196		// 4196

#define SYS_BltTranslateCM8To16				4212		// 4212
#define SYS_BltTranslateCM8To32				4213		// 4213
#define SYS_BltTranslateScaleCM8To8			4214		// 4214
#define SYS_BltTranslateScaleCM8To16		4215		// 4215


#define SYS_XimXTransGetConnectionNumber	4216		// 4216
#define SYS_XimXTransBytesReadable			4217		// 4217
#define SYS_XimXTransRead					4218		// 4218
#define SYS_XimXTransWrite					4219		// 4219
#define SYS_XimXTransReadv					4220		// 4220
#define SYS_XimXTransWritev					4221		// 4221
#define SYS_XimXTransGetPeerAddr			4222		// 4222
// 4223
#define SYS_XimXTransClose					4224		// 4224
#define SYS_XimXTransConnect				4225		// 4225
#define SYS_XimXTransOpenCOTSClient			4226		// 4226
#define SYS_XimXTransSetOption				4227		// 4227
#define SYS_XimXTransDisconnect				4228		// 4228
#define SYS_XimXTransFreeConnInfo			4229		// 4229
// 4230
// 4231
#define SYS_XimXTransOpenCLTSClient			4232		// 4232
#define SYS_XimXTransCloseForCloning		4233		// 4233
#define SYS_XimXTransIsLocal				4234		// 4234
#define SYS_XimXTransGetMyAddr				4235		// 4235
#define SYS_XimXTransGetHostname			4236		// 4236


#endif


