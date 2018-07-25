#include "stdafx.h"
#include"MatrixOpreate.h"
// �������
bool matrix_3_3_multiply_3_3(double matrix1[3][3], double matrix2[3][3], double(&result)[3][3]) {
    //������4*4����ĳ˻�
    for (int k = 0; k<3; k++) {
        for (int i = 0; i<3; i++) {
            result[i][k] = 0.0;
            for (int j = 0; j<3; j++) {
                result[i][k] = result[i][k] + matrix1[i][j] * matrix2[j][k];
            }
        }
    }
    return true;
}

// ����ת��
bool get_matrix_transpose_3_3(double matrix[3][3], double(&result)[3][3]) {
    for (int i = 0; i<3; i++) {
        for (int j = 0; j<3; j++) {
            result[j][i] = matrix[i][j];
        }
    }
    return true;
}


double SumSquare(double a, double b, double c)//��ƽ����
{
    double sumsquare;
    sumsquare = a*a + b*b + c*c;
    return sumsquare;
}

#define N 3    //���Ծ���ά������
//����һ��չ������|A|
double getA(double arcs[3][3], int n)
{
    if (n == 1)
    {
        return arcs[0][0];
    }
    double ans = 0;
    double temp[3][3] = { 0.0 };
    int i, j, k;
    for (i = 0; i<n; i++)
    {
        for (j = 0; j<n - 1; j++)
        {
            for (k = 0; k<n - 1; k++)
            {
                temp[j][k] = arcs[j + 1][(k >= i) ? k + 1 : k];

            }
        }
        double t = getA(temp, n - 1);
        if (i % 2 == 0)
        {
            ans += arcs[0][i] * t;
        }
        else
        {
            ans -= arcs[0][i] * t;
        }
    }
    return ans;
}
//����ÿһ��ÿһ�е�ÿ��Ԫ������Ӧ������ʽ�����A*
void  getAStart(double arcs[3][3], int n, double ans[3][3])
{
    if (n == 1)
    {
        ans[0][0] = 1;
        return;
    }
    int i, j, k, t;
    double temp[3][3];
    for (i = 0; i<n; i++)
    {
        for (j = 0; j<n; j++)
        {
            for (k = 0; k<n - 1; k++)
            {
                for (t = 0; t<n - 1; t++)
                {
                    temp[k][t] = arcs[k >= i ? k + 1 : k][t >= j ? t + 1 : t];
                }
            }


            ans[j][i] = getA(temp, n - 1);  //�˴�˳�������ת��
            if ((i + j) % 2 == 1)
            {
                ans[j][i] = -ans[j][i];
            }
        }
    }
}
//�õ���������src������󱣴浽des�С�
bool GetMatrixInverse(double src[3][3], int n, double des[3][3])
{
    double flag = getA(src, n);
    double t[3][3];
    if (0 == flag)
    {
        // cout << "ԭ��������ʽΪ0���޷����档����������" << endl;
        return false;//���������������ʽΪ0�������½���
    }

    {
        getAStart(src, n, t);
        for (int i = 0; i<n; i++)
        {
            for (int j = 0; j<n; j++)
            {
                des[i][j] = t[i][j] / flag;
            }

        }
    }

    return true;
}