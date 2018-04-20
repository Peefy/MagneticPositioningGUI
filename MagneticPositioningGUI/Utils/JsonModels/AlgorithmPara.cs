using System.Collections.Generic;

using Newtonsoft.Json;

namespace MagneticPositioningGUI.Utils.JsonModels
{
    public class AlgorithmPara
    {
        [JsonProperty("AA")]
        public double[,] AA { get; set; } = new double[3, 3]
        {
            { 1.0391, -0.0763,  0.1064 },
            { 0.1638,  1.0822, -0.2057 },
            {-0.0612, -0.0599,  1.3279 }
        };

        [JsonProperty("constant")]
        public double Constant { get; set; } = 4433.2738356897353;

    }
}
