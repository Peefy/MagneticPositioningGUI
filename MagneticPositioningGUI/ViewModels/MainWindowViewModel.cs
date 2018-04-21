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

        private int _count;
        public int Count
        {
            get => _count;
            set => SetProperty(ref _count, value);
        }

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

        public Command ClickCommand { get; set; }

        public IMagPosResultProvider ResultProvider { get; set; }

        public MainWindowViewModel()
        {
            _config = JsonFileConfig.Instance;
            _uiRefreshDeley = _config.UiConfig.UiRefreshDeley;
            Title = _config.UiConfig.Title;
            ResultProvider = ServiceLocator.Instance.Get<IMagPosResultProvider>();
            Task.Run(() =>
            {
                while(true)
                {
                    (X, Y, Z, Roll, Yaw, Pitch) = ResultProvider.ProvideInfo();
                    Thread.Sleep(_uiRefreshDeley);
                }
            });        
            ClickCommand = new Command(() =>
            {
                Count++;
            });
        }        
    }
}
