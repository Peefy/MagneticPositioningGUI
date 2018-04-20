using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;
using System.Windows.Media.Media3D;

using static System.Math;

using MagneticPositioningGUI.Utils;
using MagneticPositioningGUI.Models;
using MagneticPositioningGUI.Extensions;

namespace MagneticPositioningGUI.Algorithms
{
    public class MagPosResultProvider : IMagPosResultProvider
    {

        public const int AxisCount = 3;

        public const int XAxisIndex = 0;
        public const int YAxisIndex = 1;
        public const int ZAxisIndex = 2;

        public Queue<byte> SerialBuffers { get; set; }

        public bool IsRecievedData { get; set; } = false;

        public (float X, float Y, float Z, float Roll, float Yaw, float Pitch)
            UnrecievedData => (0, 0, 0, 0, 0, 0);

        public ReceiveCoil XCoil { get; set; }
        public ReceiveCoil YCoil { get; set; }
        public ReceiveCoil ZCoil { get; set; }

        public CMatrix.Matrix AA { get; set; }
        public CMatrix.Matrix XYZ { get; set; }

        Filter filterPsi = new Filter();
        Filter filterPhi = new Filter();
        Filter filterTheta = new Filter();

        JsonFileConfig _config;
        double constant;
        double Px, Py, Pz;
        double sumpxpypz;
        double rho;

        double X1, Y1, Z1;
        private double arfa;
        double A1, A2, A3;

        double Xx, Xy, Xz;
        double Yx, Yy, Yz;
        double Zx, Zy, Zz;

        double ff, f13, f23, f33;

        private double sn_arfa;
        private double cn_arfa;
        private double rho1;
        private double rho2;
        private double arfa1;
        private double A11a;
        private double A11b;
        private double A11c;
        private double A11;
        private double A12a;
        private double A12b;

        private double A12c; 

        private double A12;
        private double A23a;
        private double A23b;
        private double A23c;
        private double A23;
        private double A33a;
        private double A33b;
        private double A33c;

        private double A33;
        private double A13a;
        private double A13b;
        private double A13c;
        private double A13; 

        private double psi_1;
        private double phi_1;
        private double theta_2;
        private double respsi;
        private double resphi;
        private double restheta2;
        private double respsi_2;
        private double resphi_2;
        private double restheta2_2;
        private double betia1;
        private double betia;
        private int flag3;

        public MagPosResultProvider()
        {
            _config = JsonFileConfig.Instance;
            SerialBuffers = new Queue<byte>();
            XCoil = new ReceiveCoil();
            YCoil = new ReceiveCoil();
            ZCoil = new ReceiveCoil();
            XYZ = CMatrix.Matrix.Zeros(AxisCount);
            AA = CMatrix.Matrix.Zeros(AxisCount);
            AA.Data = _config.AlgorithmPara.AA;
            constant = _config.AlgorithmPara.Constant;
        }

        public (float X, float Y, float Z, float Roll, float Yaw, float Pitch) Provide()
        {
            if (IsRecievedData == true)
            {
                XYZ.Data = new double[AxisCount, AxisCount]
                {
                    { XCoil.X, XCoil.Y, XCoil.Z },
                    { YCoil.X, YCoil.Y, YCoil.Z },
                    { ZCoil.X, ZCoil.Y, ZCoil.Z },
                };
                XYZ = XYZ * AA;
                Xx = XCoil.X; Xy = XCoil.Y; Xz = XCoil.Z;
                Yx = YCoil.X; Yy = YCoil.Y; Yz = YCoil.Z;
                Zx = ZCoil.X; Zy = ZCoil.Y; Zz = ZCoil.Z;
                Px = NumberUtil.SumSquare(XYZ.GetColumnElements(XAxisIndex));
                Py = NumberUtil.SumSquare(XYZ.GetColumnElements(YAxisIndex));
                Pz = NumberUtil.SumSquare(XYZ.GetColumnElements(ZAxisIndex));
                sumpxpypz = Px + Py + Pz;
                rho = 1.5 * constant * constant / sumpxpypz;
                rho = Math.Pow(rho, 0.166667);

                A1 = Abs(1.1111111 * Px - 0.22222222 * Py - 0.222222222 * Pz);
                A2 = Abs(1.1111111 * Py - 0.22222222 * Px - 0.2222222 * Pz);
                A3 = Abs(1.1111111 * Pz - 0.22222222 * Px - 0.222222 * Py);

                X1 = (Math.Pow(rho, 4) / constant) * Math.Pow(A1, 0.5);
                Y1 = (Math.Pow(rho, 4) / constant) * Math.Pow(A2, 0.5);
                Z1 = (Math.Pow(rho, 4) / constant) * Math.Pow(A3, 0.5);

                f33 = 6 * (Zx * Zx + Zy * Zy + Zz * Zz);
                f33 = f33 / sumpxpypz;
                ff = f33 - 1.0;
                ff = Abs(ff) / 3.0;
                ff = Math.Pow(ff, 0.50);

                betia = Asin(ff) / PI * 180;
                f23 = 6 * (Zx * Yx + Zy * Yy + Zz * Yz);
                f23 = f23 / sumpxpypz;
                f13 = 6 * (Zx * Xx + Zy * Xy + Zz * Xz);
                f13 = f13 / sumpxpypz;
                sn_arfa = -f23 / (3 * Math.Sin(betia / 180.0 * PI) * Math.Cos(betia / 180.0 * PI));
                cn_arfa = -f13 / (3 * Math.Sin(betia / 180.0 * PI) * Math.Cos(betia / 180.0 * PI));

                rho1 = X1 * X1 + Y1 * Y1;
                rho1 = Math.Pow(rho1, 0.5);
                rho2 = X1 * X1 + Y1 * Y1 + Z1 * Z1;
                rho2 = Math.Pow(rho2, 0.5);
                arfa1 = Acos(X1 / rho1) / PI * 180;
                betia1 = Acos(rho1 / rho2);
                if ((sn_arfa > 0) && (cn_arfa > 0))
                {
                    //X1 = X1;
                    //Y1 = Y1;
                    Z1 = -Z1;
                    arfa = arfa1;
                }
                if ((sn_arfa > 0) && (cn_arfa < 0))
                { 
                    X1 = -X1;
                    //Y1 = Y1;
                    Z1 = -Z1;
                    arfa = 180.0 - arfa1;
                }
                if ((sn_arfa < 0) && (cn_arfa < 0))
                {
                    X1 = -X1;
                    Y1 = -Y1;
                    Z1 = -Z1;
                    arfa = arfa1 + 180.0;
                }
                if ((sn_arfa < 0) && (cn_arfa > 0))
                {
                    //X1 = X1;
                    Y1 = -Y1;
                    Z1 = -Z1;
                    arfa = 360.0 - arfa1;
                }
                arfa1 = arfa / 180 * PI;
                A11a = (Yx * Math.Pow(rho, 3) * (Math.Cos(arfa1) * Math.Pow(Math.Cos(betia1), 2) * Math.Sin(arfa1) + 2 * Math.Cos(arfa1) * Math.Pow(Math.Cos(betia1), 4) * Math.Sin(arfa1) - 2 * Math.Cos(arfa1) * Math.Sin(arfa1) * Math.Pow(Math.Sin(betia1), 2) + 2 * Math.Cos(arfa1) * Math.Sin(arfa1) * Math.Pow(Math.Sin(betia1), 4) + 4 * Math.Cos(arfa1) * Math.Pow(Math.Cos(betia1), 2) * Math.Sin(arfa1) * Math.Pow(Math.Sin(betia1), 2))) / (constant * ((Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 4) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 4) + Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + 2 * Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(arfa1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 2) + 4 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A11b = (Xx * Math.Pow(rho, 3) * (2 * Math.Pow(Math.Cos(arfa1), 2) * Math.Pow(Math.Sin(betia1), 2) - Math.Pow(Math.Cos(arfa1), 2) * Pow(Cos(betia1), 2) + 2 * Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + 2 * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 4 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))) / (constant * ((Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 4) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 4) + Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + 2 * Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(arfa1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 2) + 4 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A11c = (3 * Zx * Math.Pow(rho, 3) * Math.Cos(arfa1) * Math.Cos(betia1) * Math.Sin(betia1)) / (constant * ((Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 2) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A11 = A11a - A11b - A11c;
                A12a = (Xx * Pow(rho, 3) * (Cos(arfa1) * Pow(Cos(betia1), 2) * Sin(arfa1) + 2 * Cos(arfa1) * Pow(Cos(betia1), 4) * Sin(arfa1) - 2 * Cos(arfa1) * Sin(arfa1) * Pow(Sin(betia1), 2) + 2 * Cos(arfa1) * Sin(arfa1) * Pow(Sin(betia1), 4) + 4 * Cos(arfa1) * Pow(Cos(betia1), 2) * Sin(arfa1) * Pow(Sin(betia1), 2))) / (constant * ((Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 4) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 4) + Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + 2 * Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(arfa1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 2) + 4 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A12b = (Yx * Pow(rho, 3) * (2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) - Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) + 2 * Pow(Cos(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2) + 4 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2))) / (constant * ((Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 4) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 4) + Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + 2 * Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(arfa1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 2) + 4 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A12c = (3 * Zx * Pow(rho, 3) * Cos(betia1) * Sin(arfa1) * Sin(betia1)) / (constant * ((Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 2) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A12 = A12a - A12b - A12c;
                A23a = -(Zy * Pow(rho, 3) * (2 * Pow(Cos(betia1), 2) - Pow(Sin(betia1), 2))) / (constant * ((Pow(Cos(betia1), 4) + Pow(Sin(betia1), 4) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2))));
                A23b = (3 * Xy * Pow(rho, 3) * Cos(arfa1) * Cos(betia1) * Sin(betia1)) / (constant * ((Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 2) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A23c = (3 * Yy * Pow(rho, 3) * Cos(betia1) * Sin(arfa1) * Sin(betia1)) / (constant * ((Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 2) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A23 = A23a - A23b - A23c;
                A33a = -(Zz * Pow(rho, 3) * (2 * Pow(Cos(betia1), 2) - Pow(Sin(betia1), 2))) / (constant * ((Pow(Cos(betia1), 4) + Pow(Sin(betia1), 4) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2))));
                A33b = (3 * Xz * Pow(rho, 3) * Cos(arfa1) * Cos(betia1) * Sin(betia1)) / (constant * ((Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 2) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A33c = (3 * Yz * Pow(rho, 3) * Cos(betia1) * Sin(arfa1) * Sin(betia1)) / (constant * ((Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 2) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A33 = A33a - A33b - A33c;
                A13a = -(Zx * Pow(rho, 3) * (2 * Pow(Cos(betia1), 2) - Pow(Sin(betia1), 2))) / (constant * ((Pow(Cos(betia1), 4) + Pow(Sin(betia1), 4) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2))));
                A13b = (3 * Xx * Pow(rho, 3) * Cos(arfa1) * Cos(betia1) * Sin(betia1)) / (constant * ((Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 2) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A13c = (3 * Yx * Pow(rho, 3) * Cos(betia1) * Sin(arfa1) * Sin(betia1)) / (constant * ((Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 2) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A13 = A13a - A13b - A13c;
                psi_1 = Atan(Abs(A12 / A11)) / PI * 180;
                phi_1 = Atan(Abs(A23 / A33)) / PI * 180;
                theta_2 = -Asin(A13) / PI * 180;
                flag3 = 0;
                if ((A12 > 0) && (A11 > 0))
                {
                    //psi_1 = psi_1;
                }
                if ((A12 > 0) && (A11 < 0))
                {
                    psi_1 = 180.0 - psi_1;
                }
                if ((A12 < 0) && (A11 < 0))
                {
                    psi_1 = psi_1 + 180.0;
                }
                if ((A12 < 0) && (A11 > 0))
                {
                    psi_1 = 360 - psi_1;
                }
                if ((A23 > 0) && (A33 > 0))
                {
                    //phi_1 = phi_1;
                }
                if ((A23 > 0) && (A33 < 0))
                {
                    phi_1 = 180.0 - phi_1;
                }
                if ((A23 < 0) && (A33 < 0))
                {
                    phi_1 = 180.0 + phi_1;
                }
                if ((A23 < 0) && (A33 > 0))
                {
                    phi_1 = 360 - phi_1;
                }
                psi_1 = filterPsi.Run(psi_1);
                phi_1 = filterPhi.Run(phi_1);
                theta_2 = filterTheta.Run(theta_2);
                respsi = psi_1;
                resphi = phi_1;
                restheta2 = theta_2;
                respsi_2 = psi_1;
                resphi_2 = phi_1;
                restheta2_2 = theta_2;
            }
            return UnrecievedData;
        }
    }
}
