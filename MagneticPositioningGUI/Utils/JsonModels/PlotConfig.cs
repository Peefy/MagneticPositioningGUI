
using Newtonsoft.Json;

namespace MagneticPositioningGUI.Utils.JsonModels
{
    public class PlotConfig
    {
        [JsonProperty("isAutoFit")]
        public bool IsAutoFit { get; set; } = true;

        [JsonProperty("autoFitCount")]
        public int AutoFitCount { get; set; } = 200;

        [JsonProperty("plotTimerInterval")]
        public double PlotTimerInterval { get; set; } = 0.05;
    }

}
