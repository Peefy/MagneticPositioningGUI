using System;
using System.IO;
using System.Windows.Threading;
using System.Threading;
using System.IO.Ports;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;
using System.Windows.Media.Media3D;

using static System.Math;

using MagneticPositioningGUI.Utils;
using MagneticPositioningGUI.Utils.JsonModels;
using MagneticPositioningGUI.Models;
using MagneticPositioningGUI.Extensions;
using MagneticPositioningGUI.Communications;

namespace MagneticPositioningGUI.Algorithms
{
    public class MagPosResultProvider : IMagPosResultProvider
    {

        public const int AxisCount = 3;

        public const int XAxisIndex = 0;
        public const int YAxisIndex = 1;
        public const int ZAxisIndex = 2;

        public int PackageLength { get; set; } = 50;

        public int DealBuffersDeley { get; set; } = 1;

        public Queue<byte> Buffers { get; set; }

        public bool IsRecievedData { get; set; } = false;

        public (float X, float Y, float Z, float Roll, float Yaw, float Pitch)
            UnrecievedData { get; set; } = (0, 0, 0, 0, 0, 0);

        public (float X, float Y, float Z, float Roll, float Yaw, float Pitch)
            LastData { get; set; } = (0, 0, 0, 0, 0, 0);

        public int PacketRecieveUpCount { get; set; } = 20;

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
        ComConfig compara;
        SerialPort serialPort;
        //Dispatcher _dip;
        //SynchronizationContext _ds;

        int N93Rows = 4;
        int N93Columns = 3;

        double[,] N93;

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
        private int count = 0;
        private int flag = 0;
        private int flag2 = 0;
        private int[,] F4set2 = new int[3, 3]
        {
            {1,1,-1 },
            {1,1,-1 },
            {-1,-1,-1 },
        };

        public MagPosResultProvider()
        {
            _config = JsonFileConfig.Instance;
            compara = _config.ComConfig;
            N93 = new double[N93Rows, N93Columns];
            AA = new double[9];
            Buffers = new Queue<byte>();         
            XCoil = new ReceiveCoil();
            YCoil = new ReceiveCoil();
            ZCoil = new ReceiveCoil();
            DealBuffersDeley = _config.ComConfig.DealBuffersDeley;
            PackageLength = _config.ComConfig.PackageLength;
            PacketRecieveUpCount = _config.ComConfig.PacketRecieveUpCount;
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
                    var packetRecieveCount = 0;
                    while (true)
                    {
                        if (Buffers.Count == 0)
                            continue;
                        if (flag == false && Buffers.Dequeue() == compara.StartRecieveFlag)
                        {
                             flag = true;
                        }
                        if (Buffers.Count == 0)
                            continue;
                        if (flag == true && Buffers.Dequeue() == compara.N93Flag)
                        {
                            if (Buffers.Count >= PackageLength)
                            {
                                for (var i = 0; i < N93Rows; ++i)
                                {
                                    for (var j = 0; j < N93Columns; ++j)
                                    {
                                        var data = NumberUtil.FourBytesToDoubleFromQueue(Buffers) / 100.0f;
                                        N93[i, j] = data;
                                    }
                                }
                                flag = false;
                                if (++packetRecieveCount >= PacketRecieveUpCount)
                                {
                                    IsRecievedData = true;
                                }
                            }
                        }
                        Thread.Sleep(1);
                    }
                }
                catch
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
            var serialflag = SerialDataDeal();
            if (serialflag == false)
                return LastData;
            if (IsRecievedData == true)
            {
                Xx = N93[0, 0];
                Xy = N93[1, 0];
                Xz = N93[2, 0];
                Yx = N93[0, 1];
                Yy = N93[1, 1];
                Yz = N93[2, 1];
                Zx = N93[0, 2];
                Zy = N93[1, 2];
                Zz = N93[2, 2];
                //TX三轴标定
                Xx = Xx * 6.892 / 4.228; Xy = Xy * 6.892 / 4.228; Xz = Xz * 6.892 / 4.228;
                Yx = Yx * 6.892 / 5.545; Yy = Yy * 6.892 / 5.545; Yz = Yz * 6.892 / 5.545;
                var F4 = new double[3,3]{{Xx, Yx, Zx},{Xy, Yy, Zy},{ Xz,Yz,Zz}};
                Xx = Xx * Sign(F4set2[0, 0]); Yx = Yx * Sign(F4set2[0, 1]); Zx = Zx * Sign(F4set2[0, 2]);
                Xy = Xy * Sign(F4set2[1, 0]); Yy = Yy * Sign(F4set2[1, 1]); Zy = Zy * Sign(F4set2[1, 2]);
                Xz = Xz * Sign(F4set2[2, 0]); Yz = Yz * Sign(F4set2[2, 1]); Zz = Zz * Sign(F4set2[2, 2]);
                // 符号矩阵
                count++;
                if ((count >= 500) && (flag == 0))
                {
                    // 预置点 alpha=45°； beta =10°
                    for (int i = 0; i <= 2; i++)
                    {
                        for (int j = 0; j <= 2; j++)
                        {
                            if (F4[i, j] * F4set2[i, j] < 0)
                            { F4set2[i, j] = -F4set2[i, j]; }
                            else
                            { F4set2[i, j] = F4set2[i, j]; }
                        }
                    }
                    flag = 1;
                    flag2 = 1;


                }
//                 Xx = Xx * AA[0] + Xy * AA[1] + Xz * AA[2];
//                 Yx = Yx * AA[0] + Yy * AA[1] + Yz * AA[2];
//                 Zx = Zx * AA[0] + Zy * AA[1] + Zz * AA[2];
// 
//                 Xy = Xx * AA[3] + Xy * AA[4] + Xz * AA[5];
//                 Yy = Yx * AA[3] + Yy * AA[4] + Yz * AA[5];
//                 Zy = Zx * AA[3] + Zy * AA[4] + Zz * AA[5];
// 
//                 Xz = Xx * AA[6] + Xy * AA[7] + Xz * AA[8];
//                 Yz = Yx * AA[6] + Yy * AA[7] + Yz * AA[8];
//                 Zz = Zx * AA[6] + Zy * AA[7] + Zz * AA[8];

                Px = NumberUtil.SumSquare(Xx,Xy,Xz);
                Py = NumberUtil.SumSquare(Yx,Yy,Yz);
                Pz = NumberUtil.SumSquare(Zx,Zy,Zz);
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
                   // Y1 = Y1;
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
                  //  X1 = X1;
                    Y1 = -Y1;
                    Z1 = -Z1;
                    arfa = 360.0 - arfa1;
                }
                arfa1 = arfa / 180 * PI;
                A11a = (Yx * Pow(rho, 3) * (Cos(arfa1) * Pow(Math.Cos(betia1), 2) * Sin(arfa1) + 2 * Cos(arfa1) * Pow(Cos(betia1), 4) * Sin(arfa1) - 2 * Cos(arfa1) * Sin(arfa1) * Pow(Sin(betia1), 2) + 2 * Cos(arfa1) * Sin(arfa1) * Pow(Math.Sin(betia1), 4) + 4 * Math.Cos(arfa1) * Math.Pow(Math.Cos(betia1), 2) * Math.Sin(arfa1) * Math.Pow(Math.Sin(betia1), 2))) / (constant * ((Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 4) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 4) + Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + 2 * Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(arfa1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 2) + 4 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A11b = (Xx * Pow(rho, 3) * (2 * Pow(Cos(arfa1), 2) * Pow(Sin(betia1), 2) - Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) + 2 * Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + 2 * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 4 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))) / (constant * ((Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 4) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 4) + Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + 2 * Pow(Cos(arfa1), 4) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(arfa1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 4) * Pow(Sin(betia1), 2) + 4 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
                A11c = (3 * Zx * Pow(rho, 3) * Cos(arfa1) * Cos(betia1) * Sin(betia1)) / (constant * ((Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 4) + Pow(Cos(arfa1), 2) * Pow(Sin(betia1), 4) + Pow(Cos(betia1), 4) * Pow(Sin(arfa1), 2) + Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 4) + 2 * Pow(Cos(arfa1), 2) * Pow(Cos(betia1), 2) * Pow(Sin(betia1), 2) + 2 * Pow(Cos(betia1), 2) * Pow(Sin(arfa1), 2) * Pow(Sin(betia1), 2))));
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

                // X1 = filterx.Run(X1);
                // Y1 = filtery.Run(Y1);
                // Z1 = filterz.Run(Z1);
                X1 *= 5;
                Y1 *= 5;
                Z1 *= 5;
                var x = NumberUtil.MathRoundWithDigit(X1);
                var y = NumberUtil.MathRoundWithDigit(Y1);
                var z = NumberUtil.MathRoundWithDigit(Z1);
                var roll = NumberUtil.MathRoundWithDigit(psi_1);
                var yaw = NumberUtil.MathRoundWithDigit(phi_1);
                var pitch = NumberUtil.MathRoundWithDigit(theta_2);
                LastData = (x, y, z, roll, yaw, pitch);
                return LastData;
            }
            return UnrecievedData;
        }

        int status = 0;
        private bool SerialDataDeal()
        {
            if (serialPort?.IsOpen == false)
                return false;
            var flag = false;
            var bytesNum = serialPort.BytesToRead;
            if(bytesNum > 0)
            {
                byte[] buffers = new byte[PackageLength];
                serialPort.Read(buffers, 0, 1);
                switch(status)
                {
                    case 0:
                        if(buffers[0] == compara.StartRecieveFlag)
                        {
                            status = 7;
                        }
                        break;
                    case 7:
                        if(buffers[0] == compara.N93Flag)
                        {
                            bytesNum = serialPort.BytesToRead;
                            if(bytesNum <= PackageLength - 2)
                            {
                                status = 0;
                                break;
                            }
                            flag = true;
                            serialPort.Read(buffers, 0, 48);
                            var index = 0;
                            for (var i = 0; i < N93Rows; ++i)
                            {
                                for (var j = 0; j < N93Columns; ++j)
                                {
                                    var data = NumberUtil.FourBytesToDoubleFromQueue(buffers, index) / 100.0f;
                                    N93[i, j] = data;
                                    index += 4;
                                }
                            }
                            IsRecievedData = true;
                        }
                        break;
                    default:break;
                }
                          
            }
            return flag;
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
            else if(ports.Length == 1)
            {
                serialPort.PortName = ports.FirstOrDefault();
            }
            //serialPort.DataReceived -= SerialPortDataReceived;
            //serialPort.DataReceived += SerialPortDataReceived;
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

        double[,] F4_matrix = { { 0.105294561427287,0.292078314347514,-0.073687277495538 },{0.281440721479004,0.074010572495803,-0.069331405357788},{0.073149986764519, 0.069709672092147, 0.179587499495770} };
        double[,] constant_matrix =  { { 5, -1, -1 }, { -1, 5, -1 }, { -1, -1, 5 } };
        double[,] PxPyPz = { {0},{0},{0} };
        double[,] A1_matrix = { {0},{0},{0} };
        double X; double Y; double Z;
        //double rho;
        double[,] bx = { { 0 }, { 0 }, { 0 } };
        double[,] by = { { 0 }, { 0 }, { 0 } };
        double[,] bz = { { 0 }, { 0 }, { 0 } };
        double fu12; double fu23; double fu31;
        double x1, c1, z1, x2, y2, z2;
        double[,] F = { { 0, 0, 0},{ 0, 0, 0},{ 0, 0, 0} };
        double[,] F4_martix_transform = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        double[,] F4_multi_F4_martix_transform = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        //double f33, f13, f23;
        double beta, alpha, beta1, alpha1;
        double[,] Talpha_1 = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        double[,] Tbeta_1 = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        double[,] Talpha_inv_1 = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        double[,] Tbeta_inv_1 = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        double[,] S_1 = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        double[,] f3_1 = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        double[,] temp_martix = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        double[,] temp_martix2= { { 0, 0, 0},{ 0, 0, 0},{ 0, 0, 0} };
        double[,] temp_martix3 = { { 0, 0, 0},{ 0, 0, 0},{ 0, 0, 0} };
        double[,] A_matrix = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        double[,] f3_1_inv = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        double psi, theta, phi;
        int counter = 0;
        //int flag = 0;
        //double flag2;
        double x_pre, y_pre, z_pre;
        double distance1, distance2;
        double xfinal, yfinal, zfinal;
        //double Xx; double Yx; double Zx;
        //double Xy; double Yy; double Zy;
        //double Xz; double Yz; double Zz;
        double[,] F42 = { {1,1,1},{1,1,1},{1,1,1} };
        double[,] F4_set = { {1,1,-1},{ 1,1,-1 },{ -1,-1,-1 } };
        //double Px, Py, Pz, sumpxpypz;

        public (float X, float Y, float Z, float Roll, float Yaw, float Pitch) ProvideInfoV2()
        {
            var serialflag = SerialDataDeal();
            if (serialflag == false)
            {
                return LastData;
            }            
            var data = UnrecievedData;
            F4_matrix[0, 0] = N93[0, 0];
            F4_matrix[0, 1] = N93[0, 1];
            F4_matrix[0, 2] = N93[0, 2];
            F4_matrix[1, 0] = N93[1, 0];
            F4_matrix[1, 1] = N93[1, 1];
            F4_matrix[1, 2] = N93[1, 2];
            F4_matrix[2, 0] = N93[2, 0];
            F4_matrix[2, 1] = N93[2, 1];
            F4_matrix[2, 2] = N93[2, 2];
            if (IsRecievedData == true)
            {
                counter = counter++;
                if ((counter >= 500) && (flag == 0))
                {
                    for (var i = 0; i < 3; i++)
                    {
                        for (var j = 0; j < 3; j++)
                        {
                            if (F4_matrix[i, j] * F4_set[i, j] < 0)
                            {
                                F42[i, j] = -F42[i, j];
                            }
                            else
                            {
                                F42[i, j] = F42[i, j];
                            }
                        }
                    }
                    flag = 1;
                    flag2 = 1;
                }
                for (var i = 0; i < 3; ++i)
                {
                    for (var j = 0; j < 3; ++j)
                    {
                        F4_matrix[i, j] = F42[i, j] * F4_matrix[i, j];
                    }
                }
                Xx = F4_matrix[0, 0]; Yx = F4_matrix[0, 1]; Zx = F4_matrix[0, 2];
                Xy = F4_matrix[1, 0]; Yy = F4_matrix[1, 1]; Zy = F4_matrix[1, 2];
                Xz = F4_matrix[2, 0]; Yz = F4_matrix[2, 1]; Zz = F4_matrix[2, 2];
                Px = NumberUtil.SumSquare(Xx, Xy, Xz);
                Py = NumberUtil.SumSquare(Yx, Yy, Yz);
                Pz = NumberUtil.SumSquare(Zx, Zy, Zz);
                sumpxpypz = Px + Py + Pz;
                PxPyPz[0, 0] = Px; PxPyPz[1, 0] = Py; PxPyPz[2, 0] = Pz;
                rho = 1.5 * constant * constant / sumpxpypz;
                rho = Pow(rho, 0.166667);
                for (int i = 0; i < 3; i++)
                {
                    A1_matrix[i,0] = 0.0;
                    for (int j = 0; j < 3; j++)
                    {
                        A1_matrix[i, 0] = A1_matrix[i, 0] + constant_matrix[i, j] * PxPyPz[j, 0];
                    }
                }
                for (int i = 0; i < 3; i++)
                {
                    A1_matrix[i, 0] = (2.0 / 9.0) * A1_matrix[i, 0];
                }
                X = (Pow(rho, 4) / constant) * Sqrt(Abs(A1_matrix[0, 0]));
                Y = (Pow(rho, 4) / constant) * Sqrt(Abs(A1_matrix[1, 0]));
                Z = (Pow(rho, 4) / constant) * Sqrt(Abs(A1_matrix[2, 0]));
                for (int i = 0; i < 3; i++)
                {
                    bx[i, 0] = F4_matrix[i, 0];
                    by[i, 0] = F4_matrix[i, 1];
                    bz[i, 0] = F4_matrix[i, 2];
                }
                fu12 = bx[0,0] * by[0,0] + bx[1,0] * by[1,0] + bx[2,0] * by[2,0];
                fu23 = by[0,0] * bz[0,0] + by[1,0] * bz[1,0] + by[2,0] * bz[2,0];
                fu31 = bz[0,0] * bx[0,0] + bz[1,0] * bx[1,0] + bz[2,0] * bx[2,0];
                if ((fu12 > 0) && (fu23 < 0) && (fu31 < 0))
                {
                    x1 = X; c1 = Y; z1 = -Z;
                    x2 = -X; y2 = -Y; z2 = Z;
                }
                if ((fu12 < 0) && (fu23 < 0) && (fu31 > 0))
                {
                    x1 = -X; c1 = Y; z1 = -Z;
                    x2 = X; y2 = -Y; z2 = Z;
                }
                if ((fu12 > 0) && (fu23 > 0) && (fu31 > 0))
                {
                    x1 = -X; c1 = -Y; z1 = -Z;
                    x2 = X; y2 = Y; z2 = Z;
                }
                if ((fu12 < 0) && (fu23 > 0) && (fu31 < 0))
                {
                    x1 = X; c1 = -Y; z1 = -Z;
                    x2 = -X; y2 = Y; z2 = Z;
                }
                F4_martix_transform = MatrixHelper.Transpose(F4_matrix);
                F4_multi_F4_martix_transform = MatrixHelper.Multiply(F4_matrix, F4_martix_transform);
                for (var i = 0; i < 3; i++)
                {
                    for (var j = 0; j < 3; j++)
                    {
                        F[i,j] = (4 * Pow(rho, 6) / (constant * constant)) * 
                            F4_multi_F4_martix_transform[i, j];
                    }
                }
                f33 = F[2, 2]; f23 = F[1, 2]; f13 = F[0, 2];
                beta = Asin(Pow((Abs(f33 - 1) / 3), 0.5)) / PI * 180;
                alpha = Atan2(-f23, -f13) / PI * 180;
                if (alpha < 0)
                {
                    alpha = alpha + 360;
                }
                alpha1 = alpha / 180 * PI;
                beta1 = beta / 180 * PI;

                Talpha_1[0, 0] = cos(alpha1); Talpha_1[0, 1] = sin(alpha1); Talpha_1[0, 2] = 0;
                Talpha_1[1, 0] = -sin(alpha1); Talpha_1[1, 1] = cos(alpha1); Talpha_1[1, 2] = 0;
                Talpha_1[2, 0] = 0; Talpha_1[2, 1] = 0; Talpha_1[2, 2] = 1;

                Tbeta_1[0, 0] = cos(beta1); Tbeta_1[0, 1] = 0; Tbeta_1[0, 2] = -sin(beta1);
                Tbeta_1[1, 0] = 0; Tbeta_1[1, 1] = 1; Tbeta_1[1, 2] = 0;
                Tbeta_1[2, 0] = sin(beta1); Tbeta_1[2, 1] = 0; Tbeta_1[2, 2] = cos(beta1);

                Talpha_inv_1[0, 0] = cos(-alpha1); Talpha_inv_1[0, 1] = sin(-alpha1); Talpha_inv_1[0, 2] = 0;
                Talpha_inv_1[1, 0] = -sin(-alpha1); Talpha_inv_1[1, 1] = cos(-alpha1); Talpha_inv_1[1, 2] = 0;
                Talpha_inv_1[2, 0] = 0; Talpha_inv_1[2, 1] = 0; Talpha_inv_1[2, 2] = 1;

                Tbeta_inv_1[0, 0] = cos(-beta1); Tbeta_inv_1[0, 1] = 0; Tbeta_inv_1[0, 2] = -sin(-beta1);
                Tbeta_inv_1[1, 0] = 0; Tbeta_inv_1[1, 1] = 1; Tbeta_inv_1[1, 2] = 0;
                Tbeta_inv_1[2, 0] = sin(-beta1); Tbeta_inv_1[2, 1] = 0; Tbeta_inv_1[2, 2] = cos(-beta1);

                S_1[0, 0] = 1; S_1[0, 1] = 0; S_1[0, 2] = 0;
                S_1[1, 0] = 0; S_1[1, 1] = -0.5; S_1[1, 2] = 0;
                S_1[2, 0] = 0; S_1[2, 1] = 0; S_1[2, 2] = -0.5;

                temp_martix = MatrixHelper.Multiply(Talpha_inv_1, Tbeta_inv_1);
                temp_martix2 = MatrixHelper.Multiply(temp_martix, S_1);
                temp_martix3 = MatrixHelper.Multiply(temp_martix2, Tbeta_1);
                f3_1 = MatrixHelper.Multiply(temp_martix3, Talpha_1);

                for (int i = 0; i < 3; i++)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        f3_1[i, j] = (constant / Pow(rho, 3)) * f3_1[i, j];
                    }
                }
                f3_1_inv = MatrixHelper.Inv(f3_1);
                A_matrix = MatrixHelper.Multiply(F4_matrix, f3_1_inv);
                psi = Atan2(A_matrix[0, 1], A_matrix[0, 0]) / PI * 180;
                if (psi < 0)
                {
                    psi = psi + 360;
                }
                phi = Atan2(A_matrix[1, 2], A_matrix[2, 2]) / PI * 180;
                if (phi < 0)
                {
                    phi = phi + 360;
                }
                theta = Asin(-A_matrix[0, 2]) / PI * 180;
                if (flag2 == 1)
                {
                    x_pre = x1;
                    y_pre = c1;
                    z_pre = z1;
                }
                distance1 = Sqrt((x1 - x_pre) * (x1 - x_pre) + (c1 - y_pre) * (c1 - y_pre) + (z1 - z_pre) * (z1 - z_pre));
                distance2 = Sqrt((x2 - x_pre) * (x2 - x_pre) + (y2 - y_pre) * (y2 - y_pre) + (z2 - z_pre) * (z2 - z_pre));
                if (distance1 < distance2)
                {
                    xfinal = x1; yfinal = c1; zfinal = z1;
                }
                else
                {
                    xfinal = x2; yfinal = y2; zfinal = z2;
                }
                x_pre = xfinal; y_pre = yfinal; z_pre = zfinal;
                data.X = NumberUtil.MathRoundWithDigit(xfinal * 10);
                data.Y = NumberUtil.MathRoundWithDigit(yfinal * 10);
                data.Z = NumberUtil.MathRoundWithDigit(zfinal * 10);
                data.Roll = NumberUtil.MathRoundWithDigit(psi);
                data.Yaw = NumberUtil.MathRoundWithDigit(theta);
                data.Pitch = NumberUtil.MathRoundWithDigit(phi);
                LastData = data;
            }
            return data;
        }

        public double sin(double val)
        {
            return Math.Sin(val);
        }

        public double cos(double val)
        {
            return Math.Cos(val);
        }

    }
}
