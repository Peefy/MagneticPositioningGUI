using System;
using System.Windows;
using System.Threading;
using System.Threading.Tasks;

using ArkLight.Mvvm;
using ArkLight.Service;

using MagneticPositioningGUI.Utils;
using MagneticPositioningGUI.Algorithms;
using MagneticPositioningGUI.Windows;

namespace MagneticPositioningGUI.ViewModels
{
    public class MainWindowViewModel : BaseViewModel
    {

        private int _uiRefreshDeley = 20;
        private JsonFileConfig _config;

        private double _cameraX;
        public double CameraX
        {
            get => _cameraX;
            set => SetProperty(ref _cameraX, value);
        }

        private double _cameraY;
        public double CameraY
        {
            get => _cameraY;
            set => SetProperty(ref _cameraY, value);
        }

        private double _cameraZ;
        public double CameraZ
        {
            get => _cameraZ;
            set => SetProperty(ref _cameraZ, value);
        }

        private double _cameraRoll;
        public double CameraRoll
        {
            get => _cameraRoll;
            set => SetProperty(ref _cameraRoll, value);
        }

        private double _cameraYaw;
        public double CameraYaw
        {
            get => _cameraYaw;
            set => SetProperty(ref _cameraYaw, value);
        }

        private double _cameraPitch;
        public double CameraPitch
        {
            get => _cameraPitch;
            set => SetProperty(ref _cameraPitch, value);
        }

        private double _x;
        public double X
        {
            get => _x;
            set => SetProperty(ref _x, value);
        }

        private double _y;
        public double Y
        {
            get => _y;
            set => SetProperty(ref _y, value);
        }

        private double _z;
        public double Z
        {
            get => _z;
            set => SetProperty(ref _z, value);
        }

        private double _roll;
        public double Roll
        {
            get => _roll;
            set => SetProperty(ref _roll, value);
        }

        private double _yaw;
        public double Yaw
        {
            get => _yaw;
            set => SetProperty(ref _yaw, value);
        }

        private double _pitch;
        public double Pitch
        {
            get => _pitch;
            set => SetProperty(ref _pitch, value);
        }

        private string _quaternion = "0,1,0,0";
        public string Quaternion
        {
            get => _quaternion;
            set => SetProperty(ref _quaternion, value);
        }

        private float _scaleFactor = 1.5f;
        public float ScaleFactor
        {
            get => _scaleFactor;
            set => SetProperty(ref _scaleFactor, value);
        }

        private string _statusText = "";
        public string StatusText
        {
            get => _statusText;
            set => SetProperty(ref _statusText, value);
        }

        private string _controlButtonText = "开始";
        public string ControlButtonText
        {
            get => _controlButtonText;
            set => SetProperty(ref _controlButtonText, value);
        }

        private bool _isStart = false;
        public bool IsStart
        {
            get => _isStart;
            set => SetProperty(ref _isStart, value);
        }

        public bool IsDemo { get; set; }

        public bool IsUpdateUi { get; set; } = true;

        public IMagPosResultProvider ResultProvider { get; set; }

        public Task ProvideTask { get; set; }

        public Command ClickCommand { get; set; }

        public Command OpenPlotWindowCommand { get; set; }

        public PlotWindow PlotWindow{ get; set; }

        public MainWindowViewModel()
        {
            _config = JsonFileConfig.Instance;
            _uiRefreshDeley = _config.UiConfig.UiRefreshDeley;
            IsDemo = _config.UiConfig.IsDemo;
            Title = _config.UiConfig.Title;
            ResultProvider = ServiceLocator.Instance.Get<IMagPosResultProvider>();
            ClickCommand = new Command(ControlButtonEvent);
            OpenPlotWindowCommand = new Command(OpenPlotWindowEvent);
            UpdateScaleFactor();
            LoadCameraData();
            Task.Run(() =>
            {
                var i = 0;
                while (true)
                {
                    if (IsStart == true)
                    {
                        if (IsDemo == false)
                        {
                            if (IsUpdateUi == true)
                            {
                                (X, Y, Z, Roll, Yaw, Pitch) = ResultProvider.ProvideInfoV2();
                                RenewStatusText(X, Y, Z, Roll, Yaw, Pitch);
                            }
                            else
                            {
                                var result = ResultProvider.ProvideInfoV2();
                                RenewStatusText(result.X, result.Y, result.Z,
                                    result.Roll, result.Yaw, result.Pitch);                       
                            }
                        }
                        else
                        {
                            i++;
                            X = Math.Round(Math.Sin(i / 10.0), 2);
                            Y = Math.Round(Math.Cos(i / 10.0), 2);
                            Z = 0;
                            if (++Yaw >= 180)
                                Yaw = -180;
                            Thread.Sleep(_uiRefreshDeley);
                            RenewStatusText(X, Y, Z, Roll, Yaw, Pitch);
                        }
                        if (IsUpdateUi == true)
                        {
                            Quaternion = NumberUtil.EulerAnglesToQuaternion(Roll, Pitch, Yaw);
                        }
                    }
                }
            });
        }

        public void RenewStatusText(double x, double y, double z, 
            double roll, double yaw, double pitch)
        {
            StatusText = $"X:{x}\r\nY:{y}\r\nZ:{z}\r\nRoll:{roll}\r\nYaw:{yaw}\r\nPitch:{pitch}\r\n";
            RenewPlotViewData(x, y, z, roll, yaw, pitch);
        }

        public void ChangeIsDemo()
        {
            IsDemo = !IsDemo;
            _config.UiConfig.IsDemo = IsDemo;
        }

        public void ControlButtonEvent()
        {
            if (IsStart == true)
            {
                if(IsDemo == false)
                    ResultProvider.StopProvide();
                IsStart = false;
            }
            else
            {
                if(ResultProvider.StartProvide() == false)
                {
                    if (IsDemo == false)
                        MessageBox.Show("串口打开失败！");
                    else
                        IsStart = true;
                    return;
                }
                IsStart = true;
            }
            ControlButtonText = IsStart == true ? "停止" : "开始";
        }

        public void OpenPlotWindowEvent()
        {
            PlotWindow = new PlotWindow(new PlotWindowViewModel());
            PlotWindow.Show();
        }

        public void RenewPlotViewData(double x, double y, double z, double roll, double yaw, double pitch)
        {
            if (PlotWindow == null)
                return;
            PlotWindow.AppendPlotData(x, y, z, roll, yaw, pitch);
        }

        public void UpdateScaleFactor()
        {
            ScaleFactor = _config.UiConfig.ScaleFactor;
        }

        public void UpdateScaleFactor(float factor)
        {
            if (factor < 0.4)
                return;
            ScaleFactor = factor;
            _config.UiConfig.ScaleFactor = factor;
        }

        public void SaveCameraData()
        {
            var uiconfig = JsonFileConfig.Instance.UiConfig;
            uiconfig.CameraPitch = CameraPitch;
            uiconfig.CameraRoll = CameraRoll;
            uiconfig.CameraYaw = CameraYaw;
        }

        public void LoadCameraData()
        {
            var uiconfig = JsonFileConfig.Instance.UiConfig;
            CameraPitch = uiconfig.CameraPitch;
            CameraRoll = uiconfig.CameraRoll;
            CameraYaw = uiconfig.CameraYaw;
        }

    }
}
