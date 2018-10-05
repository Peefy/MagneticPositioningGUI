

using System;
using System.IO;
using System.Threading;

using ArkLight.Util;
using Newtonsoft.Json;

using MagneticPositioningGUI.Utils.JsonModels;

namespace MagneticPositioningGUI.Utils
{
    public class JsonFileConfig
    {
        [JsonIgnore]
        protected static Lazy<JsonFileConfig> _lazyInstance;

        [JsonIgnore]
        protected static Lazy<JsonFileConfig> LazyInstance =>
            _lazyInstance ?? (_lazyInstance = new Lazy<JsonFileConfig>(() => ReadFromFile(),
                LazyThreadSafetyMode.PublicationOnly));

        [JsonIgnore]
        public static JsonFileConfig Instance => LazyInstance.Value;

        [JsonIgnore]
        public static string PathAndFileName { get; set; } =
            Path.Combine(Environment.CurrentDirectory, "config.json");

        public void WriteToFile()
        {
            try
            {
                var str = JsonConvert.SerializeObject(this, Formatting.Indented);
                File.WriteAllText(PathAndFileName, str);
            }
            catch (Exception ex)
            {
                throw (ex);
            }
        }

        public static JsonFileConfig ReadFromFile()
        {
            try
            {
                var str = File.ReadAllText(PathAndFileName);
                var config = JsonConvert.DeserializeObject<JsonFileConfig>(str);
                return config;
            }
            catch (Exception)
            {
                var config = new JsonFileConfig();
                config.WriteToFile();
                return new JsonFileConfig();
            }
        }

        [JsonProperty("comConfig")]
        public ComConfig ComConfig { get; set; }

        [JsonProperty("algorithmPara")]
        public AlgorithmPara AlgorithmPara { get; set; }

        [JsonProperty("uiConfig")]
        public UiConfig UiConfig { get; set; }

        [JsonProperty("plotConfig")]
        public PlotConfig PlotConfig { get; set; }

        public JsonFileConfig()
        {
            ComConfig = new ComConfig();
            AlgorithmPara = new AlgorithmPara();
            UiConfig = new UiConfig();
            PlotConfig = new PlotConfig();
        }

        public void SetConfig(string jsonString)
        {
            try
            {
                var config = JsonConvert.DeserializeObject<JsonFileConfig>(jsonString);
                _lazyInstance = new Lazy<JsonFileConfig>(() =>
                    ClassObjectDeepCloneUtil.DeepCopyUsingXmlSerialize(config),
                    LazyThreadSafetyMode.PublicationOnly);
                WriteToFile();
            }
            catch 
            {
                
            }
        }

        public override string ToString()
        {
            try
            {
                return JsonConvert.SerializeObject(this, Formatting.Indented);
            }
            catch (Exception ex)
            {
                return "toString() call error:" + ex.Message;
            }

        }

    }
}
