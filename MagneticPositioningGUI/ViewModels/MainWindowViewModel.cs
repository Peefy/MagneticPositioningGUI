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

        private float _x;
        public float X
        {
            get => _x;
            set => SetProperty(ref _x, value);
        }

        private float _y;
        public float Y
        {
            get => _y;
            set => SetProperty(ref _y, value);
        }

        private float _z;
        public float Z
        {
            get => _z;
            set => SetProperty(ref _z, value);
        }

        private float _roll;
        public float Roll
        {
            get => _roll;
            set => SetProperty(ref _roll, value);
        }

        private float _yaw;
        public float Yaw
        {
            get => _yaw;
            set => SetProperty(ref _yaw, value);
        }

        private float _pitch;
        public float Pitch
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

        public IMagPosResultProvider ResultProvider { get; set; }

        public Task ProvideTask { get; set; }

        public Command ClickCommand { get; set; }

        public MainWindowViewModel()
        {
            _config = JsonFileConfig.Instance;
            _uiRefreshDeley = _config.UiConfig.UiRefreshDeley;
            Title = _config.UiConfig.Title;
            ResultProvider = ServiceLocator.Instance.Get<IMagPosResultProvider>();
            ClickCommand = new Command(ControlButtonEvent);
            UpdateScaleFactor();
            Task.Run(() =>
            {
                while (true)
                {
                    if (IsStart == true)
                    {
                        //(X, Y, Z, Roll, Yaw, Pitch) = ResultProvider.ProvideInfo();
                        X = -1.0f;
                        Y = -1.0f;
                        Z = -1.0f;
                        //++Roll;
                        if (++Yaw >= 180)
                            Yaw = -180;
                        Quaternion = Utils.NumberUtil.EulerAnglesToQuaternion(Roll, Yaw, Pitch);
                        StatusText = $"X:{X};Y:{Y};Z:{Z};Roll:{Roll};Yaw:{Yaw};Pitch:{Pitch}";
                        Thread.Sleep(_uiRefreshDeley);
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
            ScaleFactor = JsonFileConfig.Instance.UiConfig.ScaleFactor;
        }

        public void UpdateScaleFactor(float factor)
        {
            if (factor < 0.4)
                return;
            ScaleFactor = factor;
            JsonFileConfig.Instance.UiConfig.ScaleFactor = factor;
        }

    }
}
