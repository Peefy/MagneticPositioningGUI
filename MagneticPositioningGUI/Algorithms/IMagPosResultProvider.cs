using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MagneticPositioningGUI.Algorithms
{
    public interface IMagPosResultProvider
    {
        (float X, float Y, float Z, float Roll, float Yaw, float Pitch) ProvideInfo();
        bool StartProvide();
        bool StopProvide();
    }
}
