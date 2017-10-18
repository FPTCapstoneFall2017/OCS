#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SHT1x.h>
#include <aREST.h>

#define ssid "P201"
#define password "p@55w0rd15h4rd"
#define mqtt_server "m13.cloudmqtt.com"
#define mqtt_topic_pub_temp "temp"
#define mqtt_topic_pub_humi "humidity"
#define mqtt_user "huynqse61525"
#define mqtt_pass  "123"



#define dataPin 4
#define clockPin 5


const uint16_t mqtt_port = 17373;



aREST rest = aREST();
SHT1x sht1x(dataPin, clockPin);
WiFiServer server(80);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[1000];
int value = 0;


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
int getTemp() {
  int temp_c;
  temp_c = sht1x.readTemperatureC();

  return temp_c;
}
//----------------------get humi--------------------------

int getHum() {
  int  humi;
  humi = sht1x.readHumidity();

  return humi;
}
//------------------------------------------------------------------------------------
void loop() {
  if (!client.connected()& WiFi.status() == WL_CONNECTED ) {
    reconnect();
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.print("WiFi connected");
    Serial.print("IP address: ");
  }

  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    //-------- decode temp-------
    String tempString = String(getTemp());
    char charTemp[tempString.length()];
    for (int i = 0; i < tempString.length(); i++) {
      charTemp[i] = tempString.charAt(i);
    }
    //---------decode humi--------
    String humString = String(getHum());
    char charHum[humString.length()];
//    for (int i = 0; i < humString.length(); i++) {
//      charHum[i] = humString.charAt(i);
//    }


    client.publish(mqtt_topic_pub_temp, charTemp);
    client.publish(mqtt_topic_pub_humi, charHum);
    Serial.println();
    client.subscribe(mqtt_topic_pub_temp);
    client.subscribe(mqtt_topic_pub_humi);


  }
}

