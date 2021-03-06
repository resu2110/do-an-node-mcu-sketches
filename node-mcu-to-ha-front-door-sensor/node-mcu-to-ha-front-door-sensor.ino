/*
  This applies for the outside node 
  sensor: photon, DHT22
  
  16-6:
  out_topic: "outside/light-sensing-front-door"
              "outside/temperature-sensing-front-door"

   22/6: 
    DHT22: D0
    photon-resistor: A0
           
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
const char* out_topic_photon = "outside/light-sensing-front-door";
const char* out_topic_temperature = "outside/temperature-sensing-front-door";
const char* out_topic_humidity = "outside/humidity-sensing-front-door";
const char* clientID = "front-door-node";

float tempCelsius;
float humidity;
float msg_photon;

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("connecting to..");
  Serial.println(ssid);

  WiFi.begin(ssid,pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to....");
  Serial.println(ssid);
  Serial.print(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length){
  
}

void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT connection...");

    if(client.connect(clientID)){
      Serial.println("connected to mQTT broker");
      client.publish(out_topic_photon,clientID);
//      client.publish(out_topic_temperature,clientID);
//      client.publish(out_topic_humidity,clientID);
    }else{
      Serial.print("failed, rc=...");
      Serial.println(client.state());
      Serial.println("try again in 1 sec");
      delay(1000);
    }
  }
}

void publish_value(){
  //photon
  dtostrf(msg_photon,2,2,msg);
  Serial.print("photon = ");
  Serial.println(msg);
  client.publish(out_topic_photon,msg);


  //convert F to C
  tempCelsius = dht.convertFtoC(tempCelsius);
  dtostrf(tempCelsius,3,3,msg);

  //temperature
  client.publish(out_topic_temperature,msg);
  Serial.println(msg);

  //humidity
//  dtostrf(dht.readHumidity(DHTPIN),3,3,msg);
  dtostrf(humidity,3,3,msg);
  client.publish(out_topic_humidity,msg);
  Serial.println(msg);
  delay(2000);
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server,1883);
  pinMode(D5,INPUT);
}
 
void loop() {
  if(!client.connected()){
    reconnect();
  }
  msg_photon = analogRead(A0);
  tempCelsius = dht.readTemperature(DHTPIN);
  humidity = dht.readHumidity(DHTPIN);
  
  publish_value();
  
  delay(1000);
}
