/**
  This is mid-room-node with sensor:
    14-6: DHT22 on D5
          photonresistor on A0

          protocol: MQTT. 
          topic: out_topic_photon, out_topic_temperature, out_topic_humidity
    
*/

#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTPIN D5
#define DHTTYPE DHT22

DHT dht(DHTPIN,DHTTYPE,15);

const char* ssid = "pi-hi-hi";
const char* pass = "123456789";

const char* mqtt_server = "172.24.1.1";
const char* out_topic_photon = "room/light-sensing-mid-room";
const char* out_topic_temperature = "room/temperature-sensing-mid-room";
const char* out_topic_humidity= "room/humidity-sensing-mid-room";
const char* clientID = "mid-room-node";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to");
  Serial.println(ssid);

  WiFi.begin(ssid,pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to...");
  Serial.println(ssid);
  Serial.print(WiFi.localIP());
}

//this node just collecting environent info so doesn't need callback
void callback(char* topic, byte* payload, unsigned int length){

}

void reconnect(){
  while(!client.connected()){
    Serial.print("Atemptinh MQTT connection...");

    if(client.connect(clientID)){
      Serial.println("connected to MQTT broker");
      client.publish(out_topic_photon,clientID);
    }else{
      Serial.print("failed, rc = ");
      Serial.println(client.state());
      Serial.println(" try again in 1 sec");
      delay(1000);
    }
  }
}

void publish_value(){ 
   
//publish photon value
  dtostrf(analogRead(A0),2,2,msg);
  String msg_photon = msg;

  Serial.print("Photonresistor:");
  Serial.println(msg);
  client.publish(out_topic_photon,msg);

 //publish temperature value
  dtostrf(dht.readTemperature(DHTPIN),3,3,msg);//3 - width, 3 - precision
  String heat = msg;
  client.publish(out_topic_temperature,msg);
  Serial.println(heat);

 //publish humidity value
  dtostrf(dht.readHumidity(DHTPIN),3,3,msg);
  String humidity = msg;
  client.publish(out_topic_humidity,msg);
  Serial.println(humidity);
  delay(2000);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,1883);
  pinMode(D5,INPUT);
}

void loop() {
  if(!client.connected()){
    reconnect();
  }
  publish_value();
  client.loop();
}
