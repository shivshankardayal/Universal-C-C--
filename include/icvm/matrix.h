#ifndef ICVMMATRIX_H
#define ICVMMATRIX_H
#include <icvm/fastsyscall.h>


// ICVM_matrix44mulfRC(src1,src2,dest)
// multipy a 4x4 floating point matrix where src1 and src 2 are 4x4 matrices
// set in order of c1r1,c1r2,c1r3,c1r4,c2r1,c2r2 .... 


void ICVM_matrix44mulfRC(float *src1,float *src2,float *dest)
	ICVMCALL(SYS_matrix44mulfRC);




// ICVM_matrix34mulfRC(src1,src2,dest)
// multipy a 4x4 floating point matrix where src1 and src 2 are 4x4 matrices
// set in order of c1r1,c1r2,c1r3,c1r4,c2r1,c2r2 .... 
// bottom row of src2 is assumed to be empty (0,0,0,0)

void ICVM_matrix34mulfRC(float *src1,float *src2,float *dest)
	ICVMCALL(SYS_matrix34mulfRC);


// ICVM_matrix44mulfdRC(src1,src2,dest)
// multipy a 4x4 floating point matrix by a double precision matrix
//  where src1 and src 2 are 4x4 matrices - src1 is pointer to floats
//  src2 is pointer to doubles
// result is a floating point matrix
// set in order of c1r1,c1r2,c1r3,c1r4,c2r1,c2r2 .... 


void ICVM_matrix44mulfdRC(float *src1,double *src2,float *dest)
	ICVMCALL(SYS_matrix44mulfdRC);

// ICVM_matrix44mulfRC(src1,src2,dest)
// multipy a 4x4 double precision floating point matrix 
// where src1 and src 2 are 4x4 matrices
// set in order of c1r1,c1r2,c1r3,c1r4,c2r1,c2r2 .... 


void ICVM_matrix44muldRC(double *src1,double *src2,double *dest)
	ICVMCALL(SYS_matrix44mulfRC);




#endif // ICVMMATRIX_H
