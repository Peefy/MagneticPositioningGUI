using System;
using System.IO;
using System.IO.Ports;
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
using MagneticPositioningGUI.Communications;
using System.Windows.Threading;
using System.Threading;

namespace MagneticPositioningGUI.Algorithms
{
    public class MagPosResultProvider : IMagPosResultProvider
    {

        public const int AxisCount = 3;

        public const int XAxisIndex = 0;
        public const int YAxisIndex = 1;
        public const int ZAxisIndex = 2;

        public int PackageLength { get; set; } = 13;

        public int DealBuffersDeley { get; set; } = 10;

        public Queue<byte> Buffers { get; set; }

        public bool IsRecievedData { get; set; } = false;

        public (float X, float Y, float Z, float Roll, float Yaw, float Pitch)
            UnrecievedData { get; set; } = (0, 0, 0, 0, 0, 0);

        public ReceiveCoil XCoil { get; set; }
        public ReceiveCoil YCoil { get; set; }
        public ReceiveCoil ZCoil { get; set; }

        public CMatrix.Matrix AAMatrix { get; set; }
        public CMatrix.Matrix XYZMatrix { get; set; }

        Filter filterPsi = new Filter();
        Filter filterPhi = new Filter();
        Filter filterTheta = new Filter();
        Filter filterx = new Filter();
        Filter filtery = new Filter();
        Filter filterz = new Filter();

        JsonFileConfig _config;
        SerialPort serialPort;
        //Dispatcher _dip;
        //SynchronizationContext _ds;

        double constant;
        double Px, Py, Pz;
        double sumpxpypz;
        double rho;

        double[] AA;

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

        public MagPosResultProvider()
        {
            _config = JsonFileConfig.Instance;
            AA = new double[9];
            Buffers = new Queue<byte>();
            DealBuffersDeley = _config.ComConfig.DealBuffersDeley;
            PackageLength = _config.ComConfig.PackageLength;
            XCoil = new ReceiveCoil();
            YCoil = new ReceiveCoil();
            ZCoil = new ReceiveCoil();
            XYZMatrix = CMatrix.Matrix.Zeros(AxisCount);
            AAMatrix = CMatrix.Matrix.Zeros(AxisCount);
            AAMatrix.Data = _config.AlgorithmPara.AA;
            constant = _config.AlgorithmPara.Constant;
            UnrecievedData = (_config.UiConfig.UnrecievedDataX, _config.UiConfig.UnrecievedDataY,
                _config.UiConfig.UnrecievedDataZ, _config.UiConfig.UnrecievedDataRoll,
                _config.UiConfig.UnrecievedDataYaw, _config.UiConfig.UnrecievedDataPicth);
            RenewAAInfo();
            Task.Run(() =>
            {
                try
                {
                    var flag = false;
                    var compara = _config.ComConfig;
                    while (true)
                    {
                        if (Buffers.Count >= PackageLength)
                        {
                            if (Buffers.Dequeue() == compara.StartRecieveFlag)
                            {
                                flag = true;
                            }
                            if (flag == true)
                            {
                                var coil = new ReceiveCoil();
                                var data = Buffers.Dequeue();
                                if (data == compara.XCoilFlag)
                                {
                                    coil = XCoil;
                                }
                                else if (data == compara.YCoilFlag)
                                {
                                    coil = YCoil;
                                }
                                else if (data == compara.ZCoilFlag)
                                {
                                    coil = ZCoil;
                                }
                                coil.X = NumberUtil.FourBytesToDoubleFromQueue(Buffers);
                                coil.Y = NumberUtil.FourBytesToDoubleFromQueue(Buffers);
                                coil.Z = NumberUtil.FourBytesToDoubleFromQueue(Buffers);
                                flag = false;
                            }
                        }
                        Thread.Sleep(DealBuffersDeley);
                    }
                }
                catch (Exception)
                {
                    throw;
                }
            });
        }

        private void RenewAAInfo()
        {
            var datas = _config.AlgorithmPara.AA;
            AA[0] = datas[0, 0];
            AA[1] = datas[0, 1];
            AA[2] = datas[0, 2];
            AA[3] = datas[1, 0];
            AA[4] = datas[1, 1];
            AA[5] = datas[1, 2];
            AA[6] = datas[2, 0];
            AA[7] = datas[2, 1];
            AA[8] = datas[2, 2];
        }

        public (float X, float Y, float Z, float Roll, float Yaw, float Pitch) ProvideInfo()
        {
            if (IsRecievedData == true)
            {
                Xx = XCoil.X;
                Xy = YCoil.X;
                Xz = ZCoil.X;
                Yx = XCoil.Y;
                Yy = YCoil.Y;
                Yz = ZCoil.Z;
                Zx = XCoil.Z;
                Zy = YCoil.Z;
                Zz = ZCoil.Z;

                Xx = Xx * AA[0] + Xy * AA[1] + Xz * AA[2];
                Yx = Yx * AA[0] + Yy * AA[1] + Yz * AA[2];
                Zx = Zx * AA[0] + Zy * AA[1] + Zz * AA[2];

                Xy = Xx * AA[3] + Xy * AA[4] + Xz * AA[5];
                Yy = Yx * AA[3] + Yy * AA[4] + Yz * AA[5];
                Zy = Zx * AA[3] + Zy * AA[4] + Zz * AA[5];

                Xz = Xx * AA[6] + Xy * AA[7] + Xz * AA[8];
                Yz = Yx * AA[6] + Yy * AA[7] + Yz * AA[8];
                Zz = Zx * AA[6] + Zy * AA[7] + Zz * AA[8];

                Px = NumberUtil.SumSquare(XYZMatrix.GetColumnElements(XAxisIndex));
                Py = NumberUtil.SumSquare(XYZMatrix.GetColumnElements(YAxisIndex));
                Pz = NumberUtil.SumSquare(XYZMatrix.GetColumnElements(ZAxisIndex));
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
                ff = Pow(ff, 0.50);

                betia = Asin(ff) / PI * 180;
                f23 = 6 * (Zx * Yx + Zy * Yy + Zz * Yz);
                f23 = f23 / sumpxpypz;
                f13 = 6 * (Zx * Xx + Zy * Xy + Zz * Xz);
                f13 = f13 / sumpxpypz;
                sn_arfa = -f23 / (3 * Sin(betia / 180.0 * PI) * Cos(betia / 180.0 * PI));
                cn_arfa = -f13 / (3 * Sin(betia / 180.0 * PI) * Cos(betia / 180.0 * PI));

                rho1 = X1 * X1 + Y1 * Y1;
                rho1 = Pow(rho1, 0.5);
                rho2 = X1 * X1 + Y1 * Y1 + Z1 * Z1;
                rho2 = Pow(rho2, 0.5);
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

                X1 = X1 * 1000;
                Y1 = Y1 * 1000;
                Z1 = Z1 * 1000;
                X1 = filterx.Run(X1);
                Y1 = filtery.Run(Y1);
                Z1 = filterz.Run(Z1);
                var x = NumberUtil.MathRoundWithDigit(X1);
                var y = NumberUtil.MathRoundWithDigit(Y1);
                var z = NumberUtil.MathRoundWithDigit(Z1);
                var roll = NumberUtil.MathRoundWithDigit(psi_1);
                var yaw = NumberUtil.MathRoundWithDigit(phi_1);
                var pitch = NumberUtil.MathRoundWithDigit(theta_2);
                return (x, y, z, roll, yaw, pitch);
            }
            return UnrecievedData;
        }

        public bool StartProvide()
        {
            //_dip = Dispatcher.CurrentDispatcher;
            //_ds = new DispatcherSynchronizationContext();
            serialPort = new SerialPortBuilder().FromJsonFile();
            Buffers = new Queue<byte>();
            string[] ports = SerialPort.GetPortNames();
            Array.Sort(ports);
            if (ports.Length >= 2)
            {
                serialPort.PortName = ports[1];
            }
            else
            {
                serialPort.PortName = ports.FirstOrDefault();
            }
            serialPort.DataReceived -= SerialPortDataReceived;
            serialPort.DataReceived += SerialPortDataReceived;
            try
            {
                OpenPort();
                return true;
            }
            catch
            {
                return false;
            }
        }

        private void SerialPortDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            var bytesNum = serialPort.BytesToRead;
            byte[] buffers = new byte[bytesNum];
            serialPort.Read(buffers, 0, bytesNum);
            for (var i = 0; i < bytesNum; ++i)
                Buffers.Enqueue(buffers[i]);
            IsRecievedData = true;
        }

        public void OpenPort()
        {
            serialPort?.Open();
        }

        public bool StopProvide()
        {
            try
            {
                IsRecievedData = false;
                if (serialPort != null)
                {
                    serialPort.Close();
                    serialPort.Dispose();
                    serialPort = null;           
                }               
                return true;
            }
            catch
            {
                return false;
            }

        }
    }
}
