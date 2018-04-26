using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MagneticPositioningGUI.Algorithms
{
    public interface IMagPosResultProvider
    {
        (double X, double Y, double Z, double Roll, double Yaw, double Pitch) ProvideInfo();
        (double X, double Y, double Z, double Roll, double Yaw, double Pitch) ProvideInfoV2();
        bool StartProvide();
        bool StopProvide();
    }
}
