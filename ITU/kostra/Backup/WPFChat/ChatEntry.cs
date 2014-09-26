using Newtonsoft.Json.Serialization;
using Newtonsoft.Json;

namespace WPFChat
{
    [JsonObject(MemberSerialization.OptIn)]
    public class ChatEntry
    {
        [JsonProperty(PropertyName = "id")]
        public int ID { get; set; }

        [JsonProperty(PropertyName = "login")]
        public string Login { get; set; }

        [JsonProperty(PropertyName = "cnt")]
        public string Text { get; set; }

        [JsonProperty(PropertyName = "dts")]
        public string Time { get; set; }
    }
}
