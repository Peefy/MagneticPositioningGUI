using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using MagneticPositioningGUI.Utils;

namespace MagneticPositioningGUI.Communications
{
    public class SerialPortBuilder
    {

        public SerialPort Default
        {
            get
            {
                return new SerialPort()
                {
                    BaudRate = BaudRate,
                    Parity = Parity.None,
                    DataBits = 8,
                    StopBits = StopBits.One,
                    PortName = ComText,
                };
            }
        }

        public int BaudRate { get; set; } = 256000;

        public string ComText { get; set; } = "COM10";

        public SerialPort FromJsonFile()
        {
            var config = JsonFileConfig.ReadFromFile().ComConfig;
            var baudrate = config.BuadRate;
            var portname = config.PortName;
            var serialport = Default;
            serialport.BaudRate = baudrate;
            serialport.PortName = portname;
            serialport.ReadBufferSize = 400;
            return serialport;
        }
    }
}
