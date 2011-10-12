#include "cvm.h"
#include "icvmsys.h"
#include "icvmerr.h"

// Copyright (C) 2000,2001 Bob Dayley


// ICVM_matrix44mulfRC(src1,src2,dest)
// multipy a 4x4 floating point matrix where src1 and src 2 are 4x4 matrices
// set in order of c1r1,c1r2,c1r3,c1r4,c2r1,c2r2 .... 

void ICVM_matrix44mulfRC(void){
	float *a=(float *)Nptr(rSTKs32(0));
	float *b=(float *)Nptr(rSTKs32(1));
	p32 dst=rSTKs32(2);
	float d[16];
	int cnt=0;

// perform the multiply

	d[0]=a[0]*b[0]+a[4]*b[1]+a[8]*b[2]+a[12]*b[3];
	d[4]=a[0]*b[4]+a[4]*b[5]+a[8]*b[6]+a[12]*b[7];
	d[8]=a[0]*b[8]+a[4]*b[9]+a[8]*b[10]+a[12]*b[11];
	d[12]=a[0]*b[12]+a[4]*b[13]+a[8]*b[14]+a[12]*b[15];


	d[1]=a[1]*b[0]+a[5]*b[1]+a[9]*b[2]+a[13]*b[3];
	d[5]=a[1]*b[4]+a[5]*b[5]+a[9]*b[6]+a[13]*b[7];
	d[9]=a[1]*b[8]+a[5]*b[9]+a[9]*b[10]+a[13]*b[11];
	d[13]=a[1]*b[12]+a[5]*b[13]+a[9]*b[14]+a[13]*b[15];

	d[2]=a[2]*b[0]+a[6]*b[1]+a[10]*b[2]+a[14]*b[3];
	d[6]=a[2]*b[4]+a[6]*b[5]+a[10]*b[6]+a[14]*b[7];
	d[10]=a[2]*b[8]+a[6]*b[9]+a[10]*b[10]+a[14]*b[11];
	d[14]=a[2]*b[12]+a[6]*b[13]+a[10]*b[14]+a[14]*b[15];

	d[3]=a[3]*b[0]+a[7]*b[1]+a[11]*b[2]+a[15]*b[3];
	d[7]=a[3]*b[4]+a[7]*b[5]+a[11]*b[6]+a[15]*b[7];
	d[11]=a[3]*b[8]+a[7]*b[9]+a[11]*b[10]+a[15]*b[11];
	d[15]=a[3]*b[12]+a[7]*b[13]+a[11]*b[14]+a[15]*b[15];


// write results back
	cnt=0;
	do{
		WDf32(dst,d[cnt],cnt);
		++cnt;
	}while(cnt<16);

};

// ICVM_matrix34mulfRC(src1,src2,dest)
// multipy a 4x4 floating point matrix where src1 and src 2 are 4x4 matrices
// set in order of c1r1,c1r2,c1r3,c1r4,c2r1,c2r2 .... 
// bottom row of src2 is assumed to be empty 

void ICVM_matrix34mulfRC(void){
	float *a=(float *)Nptr(rSTKs32(0));
	float *b=(float *)Nptr(rSTKs32(1));
	p32 dst=rSTKs32(2);

	float d[16];
	int cnt=0;

// perform the multiply



	d[0]=a[0]*b[0]+a[4]*b[1]+a[8]*b[2];
	d[4]=a[0]*b[4]+a[4]*b[5]+a[8]*b[6];
	d[8]=a[0]*b[8]+a[4]*b[9]+a[8]*b[10];
	d[12]=a[0]*b[12]+a[4]*b[13]+a[8]*b[14]+a[12];


	d[1]=a[1]*b[0]+a[5]*b[1]+a[9]*b[2];
	d[5]=a[1]*b[4]+a[5]*b[5]+a[9]*b[6];
	d[9]=a[1]*b[8]+a[5]*b[9]+a[9]*b[10];
	d[13]=a[1]*b[12]+a[5]*b[13]+a[9]*b[14]+a[13];

	d[2]=a[2]*b[0]+a[6]*b[1]+a[10]*b[2];
	d[6]=a[2]*b[4]+a[6]*b[5]+a[10]*b[6];
	d[10]=a[2]*b[8]+a[6]*b[9]+a[10]*b[10];
	d[14]=a[2]*b[12]+a[6]*b[13]+a[10]*b[14]+a[14];

	d[3]=0;
	d[7]=0;
	d[11]=0;
	d[15]=1;


// write results back
	cnt=0;
	do{
		WDf32(dst,d[cnt],cnt);
		++cnt;
	}while(cnt<16);



};


void ICVM_matrix44mulfdRC(void){
	float *a=(float *)Nptr(rSTKs32(0));
	double *b=(double *)Nptr(rSTKs32(1));
	p32 dst=rSTKs32(2);
	float d[16];
	int cnt=0;

// perform the multiply

	d[0]=a[0]*b[0]+a[4]*b[1]+a[8]*b[2]+a[12]*b[3];
	d[4]=a[0]*b[4]+a[4]*b[5]+a[8]*b[6]+a[12]*b[7];
	d[8]=a[0]*b[8]+a[4]*b[9]+a[8]*b[10]+a[12]*b[11];
	d[12]=a[0]*b[12]+a[4]*b[13]+a[8]*b[14]+a[12]*b[15];


	d[1]=a[1]*b[0]+a[5]*b[1]+a[9]*b[2]+a[13]*b[3];
	d[5]=a[1]*b[4]+a[5]*b[5]+a[9]*b[6]+a[13]*b[7];
	d[9]=a[1]*b[8]+a[5]*b[9]+a[9]*b[10]+a[13]*b[11];
	d[13]=a[1]*b[12]+a[5]*b[13]+a[9]*b[14]+a[13]*b[15];

	d[2]=a[2]*b[0]+a[6]*b[1]+a[10]*b[2]+a[14]*b[3];
	d[6]=a[2]*b[4]+a[6]*b[5]+a[10]*b[6]+a[14]*b[7];
	d[10]=a[2]*b[8]+a[6]*b[9]+a[10]*b[10]+a[14]*b[11];
	d[14]=a[2]*b[12]+a[6]*b[13]+a[10]*b[14]+a[14]*b[15];

	d[3]=a[3]*b[0]+a[7]*b[1]+a[11]*b[2]+a[15]*b[3];
	d[7]=a[3]*b[4]+a[7]*b[5]+a[11]*b[6]+a[15]*b[7];
	d[11]=a[3]*b[8]+a[7]*b[9]+a[11]*b[10]+a[15]*b[11];
	d[15]=a[3]*b[12]+a[7]*b[13]+a[11]*b[14]+a[15]*b[15];


// write results back
	cnt=0;
	do{
		WDf32(dst,d[cnt],cnt);
		++cnt;
	}while(cnt<16);

};



// ICVM_matrix44mudfRC(src1,src2,dest)
// multipy a 4x4 double floating point matrix where src1 and src 2 are 4x4 matrices
// set in order of c1r1,c1r2,c1r3,c1r4,c2r1,c2r2 .... 

void ICVM_matrix44muldRC(void){
	double *a=(double *)Nptr(rSTKs32(0));
	double *b=(double *)Nptr(rSTKs32(1));
	p32 dst=rSTKs32(2);
	double d[16];
	int cnt=0;

// perform the multiply

	d[0]=a[0]*b[0]+a[4]*b[1]+a[8]*b[2]+a[12]*b[3];
	d[4]=a[0]*b[4]+a[4]*b[5]+a[8]*b[6]+a[12]*b[7];
	d[8]=a[0]*b[8]+a[4]*b[9]+a[8]*b[10]+a[12]*b[11];
	d[12]=a[0]*b[12]+a[4]*b[13]+a[8]*b[14]+a[12]*b[15];


	d[1]=a[1]*b[0]+a[5]*b[1]+a[9]*b[2]+a[13]*b[3];
	d[5]=a[1]*b[4]+a[5]*b[5]+a[9]*b[6]+a[13]*b[7];
	d[9]=a[1]*b[8]+a[5]*b[9]+a[9]*b[10]+a[13]*b[11];
	d[13]=a[1]*b[12]+a[5]*b[13]+a[9]*b[14]+a[13]*b[15];

	d[2]=a[2]*b[0]+a[6]*b[1]+a[10]*b[2]+a[14]*b[3];
	d[6]=a[2]*b[4]+a[6]*b[5]+a[10]*b[6]+a[14]*b[7];
	d[10]=a[2]*b[8]+a[6]*b[9]+a[10]*b[10]+a[14]*b[11];
	d[14]=a[2]*b[12]+a[6]*b[13]+a[10]*b[14]+a[14]*b[15];

	d[3]=a[3]*b[0]+a[7]*b[1]+a[11]*b[2]+a[15]*b[3];
	d[7]=a[3]*b[4]+a[7]*b[5]+a[11]*b[6]+a[15]*b[7];
	d[11]=a[3]*b[8]+a[7]*b[9]+a[11]*b[10]+a[15]*b[11];
	d[15]=a[3]*b[12]+a[7]*b[13]+a[11]*b[14]+a[15]*b[15];


// write results back
	cnt=0;
	do{
		WDf64(dst,d[cnt],cnt);
		++cnt;
	}while(cnt<16);

};



// base FAST system calls always hardwired into ICVM

SysCallFunction *FASTSysCalls[1024]={
		ICVM_matrix44mulfRC, // 8192
		ICVM_matrix34mulfRC, // 8193
		ICVM_matrix44mulfdRC, // 8194
		ICVM_matrix44muldRC, // 8195
};











