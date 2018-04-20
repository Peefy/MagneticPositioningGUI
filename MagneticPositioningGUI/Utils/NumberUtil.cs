using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MagneticPositioningGUI.Utils
{
    public static class NumberUtil
    {
        public static double SumSquare(double a, double b, double c)
        {
            return a * a + b * b + c * c;
        }

        public static double SumSquare(double[] numbers)
        {
            double sum = 0;
            foreach(var number in numbers)
            {
                sum += number * number;
            }
            return sum;
        }

    }

}
