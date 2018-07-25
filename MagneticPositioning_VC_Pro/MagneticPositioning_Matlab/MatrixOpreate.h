#pragma once
#ifndef MATRIXOPREATE_H_  
#define MATRIXOPREATE_H_  
#define pi 3.1415926
#define constant 4.5594e05
using namespace std;
bool matrix_3_3_multiply_3_3(double matrix1[3][3], double matrix2[3][3], double(&result)[3][3]);
bool get_matrix_transpose_3_3(double matrix[3][3], double(&result)[3][3]);
double SumSquare(double a, double b, double c);
bool GetMatrixInverse(double src[3][3], int n, double des[3][3]);
void  getAStart(double arcs[3][3], int n, double ans[3][3]);
double getA(double arcs[3][3], int n);
#endif