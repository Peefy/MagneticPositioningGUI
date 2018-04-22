using System;
using System.Threading;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using ArkLight.Mvvm;
using ArkLight.Service;
using ArkLight.Util;

using MagneticPositioningGUI.Utils;
using MagneticPositioningGUI.Algorithms;

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

        private string _statusText = "无数据";
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

        public IMagPosResultProvider ResultProvider { get; set; }

        public Task ProvideTask { get; set; }

        public Command ClickCommand { get; set; }

        public MainWindowViewModel()
        {
            _config = JsonFileConfig.Instance;
            _uiRefreshDeley = _config.UiConfig.UiRefreshDeley;
            IsDemo = _config.UiConfig.IsDemo;
            Title = _config.UiConfig.Title;
            ResultProvider = ServiceLocator.Instance.Get<IMagPosResultProvider>();
            ClickCommand = new Command(ControlButtonEvent);
            UpdateScaleFactor();
            Task.Run(() =>
            {
                var i = 0;
                while (true)
                {
                    
                    if (IsStart == true)
                    {
                        if(IsDemo == false)
                        {
                            (X, Y, Z, Roll, Yaw, Pitch) = ResultProvider.ProvideInfo();
                        }
                        else
                        {
                            i++;
                            X = Math.Sin(i / 10.0);
                            Y = Math.Cos(i / 10.0);
                            Z = 0;
                            if (++Yaw >= 180)
                                Yaw = -180;
                            Thread.Sleep(_uiRefreshDeley);
                        }
                        Quaternion = Utils.NumberUtil.EulerAnglesToQuaternion(Roll, Yaw, Pitch);
                        StatusText = $"X:{X};Y:{Y};Z:{Z};Roll:{Roll};Yaw:{Yaw};Pitch:{Pitch}";
                        
                    }
                }
            });
        }

        public void ControlButtonEvent()
        {
            if (IsStart == true)
            {
                ResultProvider.StopProvide();
                IsStart = false;
            }
            else
            {
                ResultProvider.StartProvide();
                IsStart = true;
            }
            ControlButtonText = IsStart == true ? "停止" : "开始";
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

    }
}
