#pragma once
#ifndef MAGNETICPOSITIONING_H_  
#define MAGNETICPOSITIONING_H_ 
extern int flag2;
extern double *six_Dof;
//extern double Yx,Yy,Yz;
///extern double Zx,Zy,Zz;
//extern double xfinal,yfinal,zfinal;
//extern double phi, psi, theta;
double * pose(double F4_matrix[3][3]);
#endif
