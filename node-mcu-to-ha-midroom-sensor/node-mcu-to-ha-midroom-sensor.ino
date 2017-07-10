/**
  This is mid-room-node with sensor, place on Master Viet's desk:
    14-6: DHT22 on D5
          photonresistor on A0

          protocol: MQTT.
          topic: out_topic_photon, out_topic_temperature, out_topic_humidity

    16-6: relay for desk lamp on D0
          topic: out_topic_photon

    22-6: dht22 on D5
    using new photon resisor

    29-6: gonna add  intopics:

    

*/

#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTPIN D5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE, 15);

const char* ssid = "pi-hi-hi";
const char* pass = "123456789";

const char* mqtt_server = "172.24.1.1";
const char* out_topic_photon = "room/light-sensing-mid-room";
const char* out_topic_temperature = "room/temperature-sensing-mid-room";
const char* out_topic_humidity = "room/humidity-sensing-mid-room";
const char* clientID = "mid-room-node";

float temperature;
float humidity;
float photon;

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to...");
  Serial.println(ssid);
  Serial.print(WiFi.localIP());
}

//this node just collecting environent info so doesn't need callback
void callback(char* topic, byte* payload, unsigned int length) {

}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Atemptinh MQTT connection...");

    if (client.connect(clientID)) {
      Serial.println("connected to MQTT broker");
      client.publish(out_topic_photon, clientID);
    } else {
      Serial.print("failed, rc = ");
      Serial.println(client.state());
      Serial.println(" try again in 1 sec");
      delay(1000);
    }
  }
}

void publish_value() {

  //publish photon value
  dtostrf(photon, 2, 2, msg);
  Serial.print("Photonresistor:");
  Serial.println(msg);
  client.publish(out_topic_photon, msg);

  //convert F to C
  temperature = dht.convertFtoC(temperature);
  dtostrf(temperature,3,3,msg);
  client.publish(out_topic_temperature, msg);
  Serial.println(msg);

  //publish humidity value
  dtostrf(humidity, 3, 3, msg);
  client.publish(out_topic_humidity, msg);
  Serial.println(msg);
  delay(2000);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  pinMode(D5, INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  photon = analogRead(A0);
  temperature = dht.readTemperature(DHTPIN);
  humidity = dht.readHumidity(DHTPIN);
  
  publish_value();
  delay(1000);
//  client.loop();
}
