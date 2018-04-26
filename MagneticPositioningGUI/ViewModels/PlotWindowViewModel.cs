using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

using Microsoft.Research.DynamicDataDisplay.DataSources;

using ArkLight.Mvvm;

namespace MagneticPositioningGUI.ViewModels
{
    public class PlotWindowViewModel : BaseViewModel
    {
        public PlotWindowViewModel()
        {
            Title = "曲线";
        }
    }
}
