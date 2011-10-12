#ifndef X_ICVM_H
#define X_ICVM_H

#include <X11/sys/xsyscall.h>


// ICVM custom function

// translate an 8 bit colormap to 8 bits given an input color palette table
// colormap is 256 entries of 8 bits each to translate with

void ICVM_BltTranslateCM8To8(unsigned char  *dst,
	unsigned char *src,int cnt,unsigned char *colormap)
		ICVMCALL(SYS_BltTranslateCM8To8);



// translate an 8 bit colormap to 16 bits given an input color palette table
// colormap is 256 entries of 16 bits each to translate with

void ICVM_BltTranslateCM8To16(unsigned short  *dst,
	unsigned char *src,int cnt,unsigned short *colormap)
		ICVMCALL(SYS_BltTranslateCM8To16);

// ICVM custom function
// translate an 8 bit colormap to 32 bits given an input color palette table
// colormap is 256 entries of 32 bits each to translate with

void ICVM_BltTranslateCM8To32(unsigned long *dst,
	unsigned char *src,int cnt,unsigned long *colormap) 
	ICVMCALL(SYS_BltTranslateCM8To32);


// ******** needs doc
void ICVM_BltTranslateScaleCM8To8(unsigned char *dst,
		unsigned char *src,unsigned char *colormap,
		int swidth,int sheight,int dwidth,int dheight,
		int sxdx,int sxdy,
		int sydx,int sydy,
		int soffsx,int soffsy,int screenwidth
	) 
	ICVMCALL(SYS_BltTranslateScaleCM8To8);

// ******** needs doc
void ICVM_BltTranslateScaleCM8To16(unsigned short *dst,
		unsigned char *src,unsigned short *colormap,
		int swidth,int sheight,int dwidth,int dheight,
		int sxdx,int sxdy,
		int sydx,int sydy,
		int soffsx,int soffsy,int screenwidth
	) 
	ICVMCALL(SYS_BltTranslateScaleCM8To16);


// ******** needs doc
void ICVM_BltTranslateScaleCM8To32(unsigned int *dst,
		unsigned char *src,unsigned int *colormap,
		int swidth,int sheight,int dwidth,int dheight,
		int sxdx,int sxdy,
		int sydx,int sydy,
		int soffsx,int soffsy,int screenwidth
	) 
	ICVMCALL(SYS_BltTranslateScaleCM8To32);


#endif

