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

        public (double X, double Y, double Z, double Roll, double Yaw, double Pitch)
            UnrecievedData { get; set; } = (0, 0, 0, 0, 0, 0);

        public (double X, double Y, double Z, double Roll, double Yaw, double Pitch)
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

        int N93Rows = 4;
        int N93Columns = 3;

        double[,] N93;

        double constant;
        double Px, Py, Pz;
        double sumpxpypz;
        double rho;

        double[] AA;

        double Xx, Xy, Xz;
        double Yx, Yy, Yz;
        double Zx, Zy, Zz;

        double f13, f23, f33;
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

        int status = 0;
        byte[] buffers = new byte[50];
        private bool SerialDataDeal()
        {
            try
            {
                if (serialPort?.IsOpen == false)
                    return false;
                var flag = false;
                var bytesNum = serialPort.BytesToRead;
                if (bytesNum > 0)
                {
                    serialPort.Read(buffers, 0, 1);
                    switch (status)
                    {
                        case 0:
                            if (buffers[0] == compara.StartRecieveFlag)
                            {
                                status = 7;
                            }
                            break;
                        case 7:
                            if (buffers[0] == compara.N93Flag)
                            {
                                bytesNum = serialPort.BytesToRead;
                                if (bytesNum <= PackageLength - 2)
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
                        default: break;
                    }
                }
                return flag;
            }
            catch
            {
               return false;
            }
            
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
                serialPort.PortName = ports[0];
            }
<<<<<<< HEAD
            //serialPort.DataReceived -= SerialPortDataReceived;
            //serialPort.DataReceived += SerialPortDataReceived;
            serialPort.PortName = "COM12";
            serialPort.BaudRate = 115200;
=======
>>>>>>> 26b06bf9fe41db3015bf1236ae7aa6f3a45eaf63
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

        public (double X, double Y, double Z, double Roll, double Yaw, double Pitch) ProvideInfo()
        {
            var serialflag = SerialDataDeal();
            if (serialflag == false)
            {
                return LastData;
            }            
            var data = UnrecievedData;
            F4_matrix[0, 0] = N93[0, 0] * 6.892 / 4.228;
            F4_matrix[0, 1] = N93[0, 1] * 6.892 / 5.545;
            F4_matrix[0, 2] = N93[0, 2];
            F4_matrix[1, 0] = N93[1, 0] * 6.892 / 4.228;
            F4_matrix[1, 1] = N93[1, 1] * 6.892 / 5.545;
            F4_matrix[1, 2] = N93[1, 2];
            F4_matrix[2, 0] = N93[2, 0] * 6.892 / 4.228;
            F4_matrix[2, 1] = N93[2, 1] * 6.892 / 5.545;
            F4_matrix[2, 2] = N93[2, 2];
            if (IsRecievedData == true)
            {
                counter += 1;
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
                data.X = NumberUtil.MathRoundWithDigit(x1);
                data.Y = NumberUtil.MathRoundWithDigit(c1);
                data.Z = NumberUtil.MathRoundWithDigit(z1);
                data.Roll = NumberUtil.MathRoundWithDigit(IsNanAndReserve(psi, data.Roll));
                data.Yaw = NumberUtil.MathRoundWithDigit(IsNanAndReserve(phi, data.Yaw));
                data.Pitch = NumberUtil.MathRoundWithDigit(IsNanAndReserve(theta, data.Pitch));
                LastData = data;

                double IsNanAndReserve(double value, double last)
                {
                    if (double.IsNaN(value) == true)
                        return last;
                    return value;
                }

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
