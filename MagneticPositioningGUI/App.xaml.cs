using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

using ArkLight.Service;

using MagneticPositioningGUI.Utils;
using MagneticPositioningGUI.Algorithms;

namespace MagneticPositioningGUI
{
    /// <summary>
    /// App.xaml 的交互逻辑
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            AppInit();
        }

        void AppInit()
        {
            ServiceInit();
            this.Exit += App_Exit;
        }

        void ServiceInit()
        {
            ServiceLocator.Instance.Register<IMagPosResultProvider, MagPosResultProvider>();
        }

        private void App_Exit(object sender, ExitEventArgs e)
        {
            JsonFileConfig.Instance.WriteToFile();
        }

    }
}
