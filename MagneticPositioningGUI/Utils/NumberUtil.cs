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
            return (float)((bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + (bytes[3] << 0));
        }

        public static float FourBytesToDoubleFromQueue(byte[] queue, int startindex)
        {
            return (float)((queue[startindex] << 24) + (queue[startindex + 1] << 16) + 
                (queue[startindex + 2] << 8) + (queue[startindex + 3] << 0));
        }

        public static string EulerAnglesToQuaternion(double roll, double yaw, double pitch)
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
