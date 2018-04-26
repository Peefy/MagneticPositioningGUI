using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

using MahApps.Metro.Controls;

using MagneticPositioningGUI.Utils;
using MagneticPositioningGUI.ViewModels;

namespace MagneticPositioningGUI.Windows
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : MetroWindow
    {

        public MainWindowViewModel ViewModel { get; set; }

        public MainWindow()
        {
            InitializeComponent();
            ViewModel = new MainWindowViewModel();
            DataContext = ViewModel;
        }

        private void MetroWindow_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            var factor = JsonFileConfig.Instance.UiConfig.ScaleFactor;
            factor = factor - e.Delta / 120 * 0.1f;
            ViewModel.UpdateScaleFactor(factor);
        }

        private void MetroWindow_Closed(object sender, EventArgs e)
        {
            ViewModel.SaveCameraData();
        }

        private void MetroWindow_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.V)
            {
                ViewModel.OpenPlotWindowEvent();
            }
        }
    }
}
