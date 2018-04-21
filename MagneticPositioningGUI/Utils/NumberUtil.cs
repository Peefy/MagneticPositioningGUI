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

        public static float MathRoundWithDigit(double value, int digit = 2)
        {
            return (float)Math.Round(value, digit);
        }

        public static float FourBytesToDoubleFromQueue(Queue<byte> queue)
        {
            byte[] bytes =
            {
                queue.Dequeue(),
                queue.Dequeue(),
                queue.Dequeue(),
                queue.Dequeue(),
            };
            return BitConverter.ToSingle(bytes, 0);
        }

        public static string EulerAnglesToQuaternion(float roll, float yaw, float pitch)
        {
            var cosRoll = Math.Cos(roll * 0.017453 * 0.5);
            var sinRoll = Math.Sin(roll * 0.017453 * 0.5);

            var cosPitch = Math.Cos(pitch * 0.017453 * 0.5);
            var sinPitch = Math.Sin(pitch * 0.017453 * 0.5f);

            var cosHeading = Math.Cos(yaw * 0.017453 * 0.5f);
            var sinHeading = Math.Sin(yaw * 0.017453 * 0.5f);

            var q0 = cosRoll * cosPitch * cosHeading + sinRoll * sinPitch * sinHeading;
            var q1 = sinRoll * cosPitch * cosHeading - cosRoll * sinPitch * sinHeading;
            var q2 = cosRoll * sinPitch * cosHeading + sinRoll * cosPitch * sinHeading;
            var q3 = cosRoll * cosPitch * sinHeading - sinRoll * sinPitch * cosHeading;
            return $"{q0} {q1} {q2} {q3}";
        }

    }

}
