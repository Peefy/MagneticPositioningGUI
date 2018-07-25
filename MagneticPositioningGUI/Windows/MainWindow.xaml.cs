using System;
using System.Windows.Media.Media3D;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
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

using ArkLight.Util;

using MagneticPositioningGUI.Utils;
using MagneticPositioningGUI.ViewModels;
using System.Windows.Threading;
using System.Timers;

namespace MagneticPositioningGUI.Windows
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : MetroWindow
    {

        public MainWindowViewModel ViewModel { get; set; }

        Dispatcher _dip;
        SynchronizationContext _ds;

        GeometryModel3D lines;

        public MainWindow()
        {
            InitializeComponent();
            _dip = Dispatcher.CurrentDispatcher;
            _ds = new DispatcherSynchronizationContext();
            ViewModel = new MainWindowViewModel();
            DataContext = ViewModel;
            lines = (group.Children[12] as GeometryModel3D);
            var timerAddPoint = new System.Timers.Timer();
            //timerAddPoint.Elapsed += new ElapsedEventHandler(AddPoint);
            //timerAddPoint.Interval = 50;
            //timerAddPoint.AutoReset = true;
            //timerAddPoint.Enabled = true;

        }

        private void AddPoint(object sender, ElapsedEventArgs e)
        {
            try
            {
                _dip.Invoke(new Action(() =>
                {
                    var line = lines.Clone();
                    var x = ViewModel.X;
                    var y = ViewModel.Z;
                    var z = ViewModel.Y;
                    line.Transform = new TranslateTransform3D(x, y, z);
                    group.Children.Add(line);
                }));
            }
            catch 
            {

            }

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
            if(e.Key == Key.D)
            {
                ViewModel.ChangeIsDemo();
            }
        }
    }
}
