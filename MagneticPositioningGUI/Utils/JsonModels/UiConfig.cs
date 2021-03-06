﻿
using Newtonsoft.Json;

namespace MagneticPositioningGUI.Utils.JsonModels
{
    public class UiConfig
    {
        [JsonProperty("title")]
        public string Title { get; set; } = "全空间磁定位";

        [JsonProperty("scaleFactor")]
        public float ScaleFactor { get; set; } = 1.5f;

        [JsonProperty("uiRefreshDeley")]
        public int UiRefreshDeley { get; set; } = 20;

        [JsonProperty("unrecievedDataX")]
        public float UnrecievedDataX { get; set; } = 0.1f;

        [JsonProperty("unrecievedDataY")]
        public float UnrecievedDataY { get; set; } = 0.1f;

        [JsonProperty("unrecievedDataZ")]
        public float UnrecievedDataZ { get; set; } = 0.1f;

        [JsonProperty("unrecievedDataRoll")]
        public float UnrecievedDataRoll { get; set; } = 0.1f;

        [JsonProperty("unrecievedDataYaw")]
        public float UnrecievedDataYaw { get; set; } = 0.1f;

        [JsonProperty("unrecievedDataPitch")]
        public float UnrecievedDataPicth { get; set; } = 0.1f;

        [JsonProperty("isDemo")]
        public bool IsDemo { get; set; } = false;

        [JsonProperty("cameraRoll")]
        public double CameraRoll { get; set; } = 0;

        [JsonProperty("cameraYaw")]
        public double CameraYaw { get; set; } = 0;

        [JsonProperty("cameraPitch")]
        public double CameraPitch { get; set; } = 0;

    }

}
