using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MagneticPositioningGUI.Algorithms
{
    public class Filter
    {
        public const int BufferCount = 30;

        double[] filterBuffer; 

        public Filter()
        {
            filterBuffer = new double[BufferCount + 1];
        }

        public double Run(double value)
        {
            var sum = 0.0;
            for(var i = 0; i < BufferCount; ++i)
            {
                filterBuffer[i] = filterBuffer[i + 1];
                sum += filterBuffer[i];
            }
            return sum / BufferCount;
        }

    }
}
