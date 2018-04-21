﻿using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;

using ArkLight.Model;

namespace MagneticPositioningGUI.Utils.JsonModels
{
    public class ComConfig : BaseModel
    {
        [JsonProperty("portName")]
        public string PortName { get; set; } = "COM4";

        [JsonProperty("buadRate")]
        public int BuadRate { get; set; } = 9600;

        [JsonProperty("startRecieveFlag")]
        public byte StartRecieveFlag { get; set; } = 127;

        [JsonProperty("xCoilFlag")]
        public byte XCoilFlag { get; set; } = 146;

        [JsonProperty("yCoilFlag")]
        public byte YCoilFlag { get; set; } = 147;

        [JsonProperty("zCoilFlag")]
        public byte ZCoilFlag { get; set; } = 148;

        [JsonProperty("packageLength")]
        public int PackageLength { get; set; } = 13;

        [JsonProperty("dealBuffersDeley")]
        public int DealBuffersDeley { get; set; } = 10;

    }

}
