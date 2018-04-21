

using System;
using System.IO;
using System.Threading;

using ArkLight.Util;
using Newtonsoft.Json;

using MagneticPositioningGUI.Utils.JsonModels;

namespace MagneticPositioningGUI.Utils
{
    /// <summary>
    /// config.json 配置文件操作
    /// </summary>
    public class JsonFileConfig
    {
        [JsonIgnore]
        protected static Lazy<JsonFileConfig> _lazyInstance;

        [JsonIgnore]
        protected static Lazy<JsonFileConfig> LazyInstance =>
            _lazyInstance ?? (_lazyInstance = new Lazy<JsonFileConfig>(() => ReadFromFile(),
                LazyThreadSafetyMode.PublicationOnly));

        /// <summary>
        /// 配置文件静态实例
        /// </summary>
        [JsonIgnore]
        public static JsonFileConfig Instance => LazyInstance.Value;

        /// <summary>
        /// 配置文件路径和文件名称
        /// </summary>
        [JsonIgnore]
        public static string PathAndFileName { get; set; } =
            Path.Combine(Environment.CurrentDirectory, "config.json");

        /// <summary>
        /// 配置写入文件
        /// </summary>
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

        /// <summary>
        /// 从文件读取配置
        /// </summary>
        /// <returns></returns>
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

        public ComConfig ComConfig { get; set; }

        public AlgorithmPara AlgorithmPara { get; set; }

        public UiConfig UiConfig { get; set; }

        /// <summary>
        /// 无参构造函数
        /// </summary>
        public JsonFileConfig()
        {
            ComConfig = new ComConfig();
            AlgorithmPara = new AlgorithmPara();
            UiConfig = new UiConfig();
        }

        /// <summary>
        /// 用json字符串更改配置
        /// </summary>
        /// <param name="jsonString"></param>
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

        /// <summary>
        /// 返回配置文件的json字符串
        /// </summary>
        /// <returns></returns>
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
