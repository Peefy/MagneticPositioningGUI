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
using System.Windows.Threading;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

using MahApps.Metro.Controls;

using Microsoft.Research.DynamicDataDisplay;
using Microsoft.Research.DynamicDataDisplay.DataSources;
using Microsoft.Research.DynamicDataDisplay.Charts.Axes;

using MagneticPositioningGUI.Utils;
using MagneticPositioningGUI.ViewModels;

namespace MagneticPositioningGUI.Windows
{
    /// <summary>
    /// PlotWindow.xaml 的交互逻辑
    /// </summary>
    public partial class PlotWindow : MetroWindow
    {

        private double i = 0;
        private DispatcherTimer timer = new DispatcherTimer();

        public PlotWindowViewModel ViewModel { get; set; }

        public ObservableDataSource<Point> XDataSource { get; set; }
            = new ObservableDataSource<Point>();

        public ObservableDataSource<Point> YDataSource { get; set; }
            = new ObservableDataSource<Point>();

        public ObservableDataSource<Point> ZDataSource { get; set; }
            = new ObservableDataSource<Point>();

        public ObservableDataSource<Point> RollDataSource { get; set; }
            = new ObservableDataSource<Point>();

        public ObservableDataSource<Point> YawDataSource { get; set; }
            = new ObservableDataSource<Point>();

        public ObservableDataSource<Point> PitchDataSource { get; set; }
            = new ObservableDataSource<Point>();

        public bool IsAutoFit { get; set; } = true;

        public int AutoFitCount { get; set; } = 200;

        public double PlotTimerInterval { get; set; } = 0.05;

        public int LineWidth { get; set; } = 2;

        public PlotWindow(PlotWindowViewModel viewModel)
        {
            InitializeComponent();
            ViewModel = viewModel;
            DataContext = ViewModel;
            RenewPlotConfig();
        }

        private void RenewPlotConfig()
        {
            var config = JsonFileConfig.ReadFromFile().PlotConfig;
            AutoFitCount = config.AutoFitCount;
            IsAutoFit = config.IsAutoFit;
            PlotTimerInterval = config.PlotTimerInterval;
        }

        private void AnimatedPlot(object sender, EventArgs e)
        {
            XDataSource.AppendAsync(base.Dispatcher, new Point(i, x));
            YDataSource.AppendAsync(base.Dispatcher, new Point(i, y));
            ZDataSource.AppendAsync(base.Dispatcher, new Point(i, z));
            RollDataSource.AppendAsync(base.Dispatcher, new Point(i, roll));
            YawDataSource.AppendAsync(base.Dispatcher, new Point(i, yaw));
            PitchDataSource.AppendAsync(base.Dispatcher, new Point(i, pitch));
            var count = XDataSource.Collection.Count;
            i += PlotTimerInterval;
            if (count >= AutoFitCount && IsAutoFit == true)
            {
                XDataSource.Collection.RemoveAt(0);
                YDataSource.Collection.RemoveAt(0);
                ZDataSource.Collection.RemoveAt(0);
                RollDataSource.Collection.RemoveAt(0);
                YawDataSource.Collection.RemoveAt(0);
                PitchDataSource.Collection.RemoveAt(0);
                FitToView();
            }
                
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            xplotter.AddLineGraph(XDataSource, Colors.BlueViolet, LineWidth);
            yplotter.AddLineGraph(YDataSource, Colors.BlueViolet, LineWidth);
            zplotter.AddLineGraph(ZDataSource, Colors.BlueViolet, LineWidth);
            rollplotter.AddLineGraph(RollDataSource, Colors.BlueViolet, LineWidth);
            yawplotter.AddLineGraph(YawDataSource, Colors.BlueViolet, LineWidth);
            pitchplotter.AddLineGraph(PitchDataSource, Colors.BlueViolet, LineWidth);
            timer.Interval = TimeSpan.FromSeconds(PlotTimerInterval);
            timer.Tick += new EventHandler(AnimatedPlot);
            timer.IsEnabled = true;
            FitToView();
        }

        public void FitToView()
        {
            xplotter.Viewport.FitToView();
            yplotter.Viewport.FitToView();
            zplotter.Viewport.FitToView();
            rollplotter.Viewport.FitToView();
            yawplotter.Viewport.FitToView();
            pitchplotter.Viewport.FitToView();
        }

        private void MetroWindow_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.C)
            {
                ClearPlotData();
            }
            if(e.Key == Key.Q)
            {
                IsAutoFit = !IsAutoFit;
            }
        }

        private void ClearPlotData()
        {
            //xplotter.Children.Clear();
            XDataSource.Collection.Clear();

            //yplotter.Children.Clear();
            YDataSource.Collection.Clear();

            //zplotter.Children.Clear();
            ZDataSource.Collection.Clear();

            //rollplotter.Children.Clear();
            RollDataSource.Collection.Clear();

            //yawplotter.Children.Clear();
            YawDataSource.Collection.Clear();

            //pitchplotter.Children.Clear();
            PitchDataSource.Collection.Clear();

            FitToView();

        }

        private double x;
        private double y;
        private double z;
        private double roll;
        private double yaw;
        private double pitch;

        public void AppendPlotData(double x, double y, double z, 
            double roll, double yaw, double pitch)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.roll = roll;
            this.yaw = yaw;
            this.pitch = pitch;
        }
    }
}
