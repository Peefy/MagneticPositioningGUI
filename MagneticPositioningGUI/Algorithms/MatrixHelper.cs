using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MagneticPositioningGUI.Algorithms
{
    public static class MatrixHelper
    {
        public static double[,] Multiply(double[,] matrix1, double[,] matrix2)
        {
            var result = Zeros(3);
            for (int k = 0; k < 3; k++)
            {
                for (int i = 0; i < 3; i++)
                {
                    result[i, k] = 0.0;
                    for (int j = 0; j < 3; j++)
                    {
                        result[i, k] = result[i,k] + matrix1[i,j] * matrix2[j,k];
                    }
                }
            }
            return result;
        }

        public static double[,] Transpose(double[,] matrix)
        {
            var result = Zeros(3);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    result[j, i] = matrix[i, j];
                }
            }
            return result;
        }

        public static double GetA(double[,] arcs, int n = 3)
        {
            if (n == 1)
            {
                return arcs[0, 0];
            }
            double ans = 0;
            var temp = Zeros(3);
            int i, j, k;
            for (i = 0; i<n; i++)
            {
                for (j = 0; j < n - 1; j++)
                {
                    for (k = 0; k < n - 1; k++)
                    {
                        temp[j, k] = arcs[j + 1, (k >= i) ? k + 1 : k];

                    }
                }
                var t = GetA(temp, n - 1);
                if (i % 2 == 0)
                {
                     ans += arcs[0,i] * t;
                }
                else
                {
                     ans -= arcs[0,i] * t;
                }
            }
            return ans;
        }

        public static double[,] GetAStart(double[,] arcs, int n = 3)
        {
            var ans = Zeros(3);
            if (n == 1)
            {
                ans[0, 0] = 1;
                return ans;
            }
            int i, j, k, t;
            var temp = Zeros(3);
            for (i = 0; i < n; i++)
            {
                for (j = 0; j < n; j++)
                {
                    for (k = 0; k < n - 1; k++)
                    {
                        for (t = 0; t < n - 1; t++)
                        {
                            temp[k,t] = arcs[k >= i ? k + 1 : k,t >= j ? t + 1 : t];
                        }
                    }


                    ans[j,i] = GetA(temp, n - 1);  //�˴�˳�������ת��
                    if ((i + j) % 2 == 1)
                    {
                        ans[j,i] = -ans[j,i];
                    }
                }
            }
            return ans;
        }

        public static double[,] Inv(double[,] src, int n = 3)
        {
            double flag = GetA(src, n);
            if(flag == 0)
            {
                return null;
            }
            var t = GetAStart(src, n);
            var des = Zeros(3);
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    des[i, j] = t[i, j] / flag;
                }
            }
            return des;
        }

        public static double[,] Zeros(int num)
        {
            var result = new double[num, num];
            for(var i = 0;i < num ;++i)
            {
                for (var j = 0; j < num; ++j)
                {
                    result[i, j] = 0;
                }
            }
            return result;
        }

    }
}
