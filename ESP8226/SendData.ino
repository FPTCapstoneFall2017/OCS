#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SHT1x.h>
#include <aREST.h>

#define ssid "Mon"
#define password "nguyentrongnghia261193"
#define mqtt_server "127.0.0.1"
#define mqtt_topic_pub_temp "temp"
//#define mqtt_server "m12.cloudmqtt.com"
//#define mqtt_topic_pub_temp "sensor/temp"
#define mqtt_topic_pub_humi "sensor/humidity"
//#define mqtt_topic_control "control"
#define mqtt_user "luiekkcp"
#define mqtt_pass  "r_jrVa1OZFL0"



#define dataPin 4
#define clockPin 5


const uint16_t mqtt_port = 1883;



aREST rest = aREST();
SHT1x sht1x(dataPin, clockPin);
WiFiServer server(80);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[1000];

/* Pub variable */
//String sPub_Temp;
//String sPub_Humi;

float humi = 0.0;
float temp = 0.0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}
//-------------------------------Setup _Wifi-------------------------------------------
void setup_wifi() {
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("");
  Serial.print("WiFi connected");
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
}
//--------------------------------Callback--------------------------------------------------
void callback ( char* topic, byte* payload, unsigned int length) {
  String message = "";
  int i = 0;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  while ( i < length) {
    message += (char)payload[i++];
  }

  Serial.print("Receive");
  Serial.println(message);

}


//-----------------------------------Reconnect to mqtt server-------------------------------------------------
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_topic_pub_temp, "ESP_reconnected temperature");
      client.publish(mqtt_topic_pub_humi, "ESP_reconnected humidity");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//---------------------------get tem---------------

///* Clean code */
//float getTemp()
//{
//  return sht1x.readTemperatureC();
//}
//----------------------get humi--------------------------

///* Clean code */
//float getHumi()
//{
//  return sht1x.readTemperatureC();
//}
//------------------------------------------------------------------------------------
void loop() {


  if (!client.connected()& WiFi.status() == WL_CONNECTED ) {
    reconnect();
  }


  client.loop();

  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;


    temp = sht1x.readTemperatureC();

    Serial.print("New temperature:");
    Serial.println(String(temp).c_str());
    client.publish(mqtt_topic_pub_temp, String(temp).c_str(), true);


    humi = sht1x.readHumidity();

    Serial.print("New humidity:");
    Serial.println(String(humi).c_str());
    client.publish(mqtt_topic_pub_humi, String(humi).c_str(), true);




       // client.subscribe(mqtt_topic_control);


  }
}

