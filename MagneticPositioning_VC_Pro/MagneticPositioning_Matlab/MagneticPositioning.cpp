// MagneticPositioning.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "MatrixOpreate.h"
#include "MagneticPositioning.h"

double *six_Dof= new double[6];
double * pose(double F4_matrix[3][3])
{
        double constant_matrix[3][3] = { { 5,-1,-1 },{ -1, 5, -1 },{ -1,-1,5 } };
        double PxPyPz[3][1] = { { 0 },{ 0 },{ 0 } };
        double A1_matrix[3][1] = { { 0 },{ 0 },{ 0 } };
        double X; double Y; double Z; double rho;
        double bx[3][1]; double by[3][1]; double bz[3][1];
        double fu12; double fu23; double fu31;
        double x1, c1, z1, x2, y2, z2;
        double F[3][3];
        double F4_martix_transform[3][3];
        double F4_multi_F4_martix_transform[3][3];
        double f33, f13, f23;
        double beta, alpha, beta1, alpha1;

        double Talpha_1[3][3];
        double Tbeta_1[3][3];
        double Talpha_inv_1[3][3];
        double Tbeta_inv_1[3][3];
        double S_1[3][3];
        double f3_1[3][3];
        double temp_martix[3][3], temp_martix2[3][3], temp_martix3[3][3];
        double A_matrix[3][3];
        double f3_1_inv[3][3];
        double psi, theta, phi;

        int flag = 0;
        double  x_pre, y_pre, z_pre;
        double distance1, distance2;
        double xfinal, yfinal, zfinal;
        double Xx; double Yx; double Zx;
        double Xy; double Yy; double Zy;
        double Xz; double Yz; double Zz;
    
        double Px, Py, Pz, sumpxpypz;

      
        Xx = F4_matrix[0][0]; Yx = F4_matrix[0][1]; Zx = F4_matrix[0][2];
        Xy = F4_matrix[1][0]; Yy = F4_matrix[1][1]; Zy = F4_matrix[1][2];
        Xz = F4_matrix[2][0]; Yz = F4_matrix[2][1]; Zz = F4_matrix[2][2];

        Px = SumSquare(Xx, Xy, Xz);
        Py = SumSquare(Yx, Yy, Yz);
        Pz = SumSquare(Zx, Zy, Zz);
        sumpxpypz = Px + Py + Pz;
        PxPyPz[0][0] = Px; PxPyPz[1][0] = Py; PxPyPz[2][0] = Pz;
        // 计算距离
        rho = 1.5*constant*constant / sumpxpypz;
        rho = pow(rho, 0.166667);

        //求矩阵A1
        for (int i = 0; i < 3; i++) {
            A1_matrix[i][0] = 0.0;
            for (int j = 0; j < 3; j++) {
                A1_matrix[i][0] = A1_matrix[i][0] + constant_matrix[i][j] * PxPyPz[j][0];
            }
        }
        for (int i = 0; i < 3; i++)
        {
            A1_matrix[i][0] = (2.0 / 9.0) * A1_matrix[i][0];
        }

        X = (pow(rho, 4) / constant)*sqrt(abs(A1_matrix[0][0])); Y = (pow(rho, 4) / constant)*sqrt(abs(A1_matrix[1][0])); Z = (pow(rho, 4) / constant)*sqrt(abs(A1_matrix[2][0]));
        //bx,by,bz 赋值
        for (int i = 0; i < 3; i++)
        {
            bx[i][0] = F4_matrix[i][0];
            by[i][0] = F4_matrix[i][1];
            bz[i][0] = F4_matrix[i][2];

        }
        // 求内积
        fu12 = bx[0][0] * by[0][0] + bx[1][0] * by[1][0] + bx[2][0] * by[2][0];
        fu23 = by[0][0] * bz[0][0] + by[1][0] * bz[1][0] + by[2][0] * bz[2][0];
        fu31 = bz[0][0] * bx[0][0] + bz[1][0] * bx[1][0] + bz[2][0] * bx[2][0];
        //象限判断
        if ((fu12 > 0) && (fu23 < 0) && (fu31 < 0))
        {
            x1 = X; c1 = Y; z1 = -Z;
            x2 = -X; y2 = -Y; z2 = Z;
        }
        if ((fu12 < 0) && (fu23 < 0) && (fu31 > 0))
        {
            x1 = -X; c1 = Y; z1 = -Z;
            x2 = X; y2 = -Y; z2 = Z;
        }
        if ((fu12 > 0) && (fu23 > 0) && (fu31 > 0))
        {
            x1 = -X; c1 = -Y; z1 = -Z;
            x2 = X; y2 = Y; z2 = Z;
        }
        if ((fu12 < 0) && (fu23 > 0) && (fu31 < 0))
        {
            x1 = X; c1 = -Y; z1 = -Z;
            x2 = -X; y2 = Y; z2 = Z;
        }
        //alpha ,beta 解算
        get_matrix_transpose_3_3(F4_matrix, F4_martix_transform);
        matrix_3_3_multiply_3_3(F4_martix_transform, F4_matrix, F4_multi_F4_martix_transform);

        //计算F矩阵
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                F[i][j] = (4 * pow(rho, 6) / (constant*constant))*F4_multi_F4_martix_transform[i][j];
            }
        }

        // 计算 f33 f23 f13
        f33 = F[2][2]; f23 = F[1][2]; f13 = F[0][2];
        beta = asin(pow((abs(f33 - 1) / 3), 0.5)) / pi * 180;
        alpha = atan2(-f23, -f13) / pi * 180;
        if (alpha < 0)
        {
            alpha = alpha + 360;
        }
        //求解姿态角
        alpha1 = alpha / 180 * pi;
        beta1 = beta / 180 * pi;

        Talpha_1[0][0] = cos(alpha1);  Talpha_1[0][1] = sin(alpha1); Talpha_1[0][2] = 0;
        Talpha_1[1][0] = -sin(alpha1); Talpha_1[1][1] = cos(alpha1); Talpha_1[1][2] = 0;
        Talpha_1[2][0] = 0;            Talpha_1[2][1] = 0;           Talpha_1[2][2] = 1;

        Tbeta_1[0][0] = cos(beta1);  Tbeta_1[0][1] = 0; Tbeta_1[0][2] = -sin(beta1);
        Tbeta_1[1][0] = 0;           Tbeta_1[1][1] = 1; Tbeta_1[1][2] = 0;
        Tbeta_1[2][0] = sin(beta1);  Tbeta_1[2][1] = 0; Tbeta_1[2][2] = cos(beta1);


        Talpha_inv_1[0][0] = cos(-alpha1);  Talpha_inv_1[0][1] = sin(-alpha1); Talpha_inv_1[0][2] = 0;
        Talpha_inv_1[1][0] = -sin(-alpha1); Talpha_inv_1[1][1] = cos(-alpha1); Talpha_inv_1[1][2] = 0;
        Talpha_inv_1[2][0] = 0;            Talpha_inv_1[2][1] = 0;           Talpha_inv_1[2][2] = 1;

        Tbeta_inv_1[0][0] = cos(-beta1);  Tbeta_inv_1[0][1] = 0; Tbeta_inv_1[0][2] = -sin(-beta1);
        Tbeta_inv_1[1][0] = 0;           Tbeta_inv_1[1][1] = 1; Tbeta_inv_1[1][2] = 0;
        Tbeta_inv_1[2][0] = sin(-beta1);  Tbeta_inv_1[2][1] = 0; Tbeta_inv_1[2][2] = cos(-beta1);

        S_1[0][0] = 1;  S_1[0][1] = 0;    S_1[0][2] = 0;
        S_1[1][0] = 0;  S_1[1][1] = -0.5; S_1[1][2] = 0;
        S_1[2][0] = 0;  S_1[2][1] = 0;    S_1[2][2] = -0.5;

        matrix_3_3_multiply_3_3(Talpha_inv_1, Tbeta_inv_1, temp_martix);
        matrix_3_3_multiply_3_3(temp_martix, S_1, temp_martix2);
        matrix_3_3_multiply_3_3(temp_martix2, Tbeta_1, temp_martix3);
        matrix_3_3_multiply_3_3(temp_martix3, Talpha_1, f3_1);


        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                f3_1[i][j] = (constant / pow(rho, 3)) *f3_1[i][j];
            }
        }

        GetMatrixInverse(f3_1, 3, f3_1_inv);
        // 得到A矩阵
        matrix_3_3_multiply_3_3(F4_matrix, f3_1_inv, A_matrix);
        psi = atan2(A_matrix[0][1], A_matrix[0][0]) / pi * 180;
        if (psi < 0)
        {
            psi = psi + 360;
        }
        phi = atan2(A_matrix[1][2], A_matrix[2][2]) / pi * 180;
        if (phi < 0)
        {
            phi = phi + 360;
        }
        theta = asin(-A_matrix[0][2]) / pi * 180;

        if (flag2 == 1)
        {
            x_pre = x1;
            y_pre = c1;
            z_pre = z1;
        }

        distance1 = sqrt((x1 - x_pre)*(x1 - x_pre) + (c1 - y_pre)*(c1 - y_pre) + (z1 - z_pre)*(z1 - z_pre));
        distance2 = sqrt((x2 - x_pre)*(x2 - x_pre) + (y2 - y_pre)*(y2 - y_pre) + (z2 - z_pre)*(z2 - z_pre));

        if (distance1 < distance2)
        {
            xfinal = x1; yfinal = c1; zfinal = z1;
        }
        else
        {
            xfinal = x2; yfinal = y2; zfinal = z2;
        }
        x_pre = xfinal; y_pre = yfinal; z_pre = zfinal;
  
        six_Dof[0] = xfinal; six_Dof[1] = yfinal; six_Dof[2] = zfinal;
        six_Dof[3] = psi; six_Dof[4] = theta; six_Dof[5] = phi;
        return six_Dof;
}


