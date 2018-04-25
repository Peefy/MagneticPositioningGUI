using System;
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
        public int BuadRate { get; set; } = 256000;

        [JsonProperty("startRecieveFlag")]
        public byte StartRecieveFlag { get; set; } = 127;

        [JsonProperty("N93Flag")]
        public byte N93Flag { get; set; } = 146;

        [JsonProperty("packageLength")]
        public int PackageLength { get; set; } = 48;

        [JsonProperty("dealBuffersDeley")]
        public int DealBuffersDeley { get; set; } = 10;

        [JsonProperty("packetRecieveUpCount")]
        public int PacketRecieveUpCount { get; set; } = 20;

    }

}
