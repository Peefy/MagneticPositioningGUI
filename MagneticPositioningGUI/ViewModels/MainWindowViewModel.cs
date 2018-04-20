using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using ArkLight.Mvvm;
using ArkLight.Service;
using ArkLight.Util;

using MagneticPositioningGUI.Algorithms;

namespace MagneticPositioningGUI.ViewModels
{
    public class MainWindowViewModel : BaseViewModel
    {

        private int _count;
        public int Count
        {
            get => _count;
            set => SetProperty(ref _count, value);
        }

        public Command ClickCommand { get; set; }

        public IMagPosResultProvider ResultProvider { get; set; }

        public MainWindowViewModel()
        {
            ResultProvider = ServiceLocator.Instance.Get<IMagPosResultProvider>();
            Title = "全空间磁定位";
            ClickCommand = new Command(() =>
            {
                Count++;
            });
        }        
    }
}
