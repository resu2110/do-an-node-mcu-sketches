/*
  This applies for the outside node (yard)
  sensor: photon, DHT22
  
  29/6
  out_topic: "outside/light-sensing-yard"
              "outside/temperature-sensing-yard"
              "outside/humidity-sensing-yard"
   22/6: 
    DHT22: D0
    photon-resistor: A0

   5/7: changing outtopics  from outside/ to room/

   9/7: adding intopics
        adding toogle-automation
        D0: lamp
        D3: mist
   10/7: remove all control, just sensing
           
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
const char* clientID = "corner-node";

const char* out_topic_photon = "room/light-sensing-corner";
const char* out_topic_temperature = "room/temperature-sensing-corner";
const char* out_topic_humidity = "room/humidity-sensing-corner";

//const char* in_topic_lamp = "/room/desk-lamp-corner-control";
//const char* in_topic_mist_generator = "/room/mist-generator-corner-control";
//
//const char* in_topic_toggle_automatic = "room/toogle-automatic";

float temperature;
float humidity;
float photon;

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
//
//  String topicName(topic);
//  
//  if(topicName == in_topic_toggle_automatic){
//      String toggle_automation = String((char*)payload);
//      Serial.print(toggle_automation);
//      if(toggle_automation = "ONFon"){
//        client.subscribe(out_topic_photon);
//        client.subscribe(out_topic_temperature);
//        client.subscribe(out_topic_humidity);
//      }else if(toggle_automation = "OFFon"){
//        client.unsubscribe(out_topic_photon);
//        client.unsubscribe(out_topic_temperature);
//        client.unsubscribe(out_topic_humidity);
//      }
//  }
//
//  if(topicName == out_topic_photon){
//    photon = atof((char*)payload);
//    Serial.print("photon value from server:");
//    Serial.println(photon);
//    if(photon > 900){
//      Serial.println("too dark here!");
//      digitalWrite(D0,LOW);
//    }else{
//      digitalWrite(D0,HIGH);
//    }
//  }
//
//  if(topicName == out_topic_temperature){
//    temperature = atof((char*)payload);
//    Serial.print("temperature from server:");
//    Serial.println(temperature);
//    if(temperature > 30.0){
//      Serial.println("too hot at corner");
//      digitalWrite(D3, LOW);      
//    }else{
//      digitalWrite(D3,HIGH);
//    }
//  }
}

void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT connection...");

    if(client.connect(clientID)){
      Serial.println("connected to MQTT broker");      
      client.publish(out_topic_photon,clientID);      
      client.publish(out_topic_temperature,clientID);
      client.publish(out_topic_humidity,clientID);
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
  dtostrf(photon,2,2,msg);
  client.publish(out_topic_photon,msg);
  Serial.print("photon = ");
  Serial.println(msg);
  
  //convert F to C
  temperature = dht.convertFtoC(temperature);
  dtostrf(temperature,3,3,msg);
    
  //temperature
  client.publish(out_topic_temperature,msg);
  Serial.println(msg);  

  //humidity
  dtostrf(humidity,3,3,msg);
  client.publish(out_topic_humidity,msg);
  Serial.println(msg);

  delay(3000);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
  
  pinMode(D5,INPUT);
}

void loop() {
  if(!client.connected()){
    reconnect();
  }

  temperature = dht.readTemperature(DHTPIN);
  humidity = dht.readHumidity(DHTPIN);
  photon = analogRead(A0);
  
  publish_value();
  delay(2000);  
//  client.loop();
}
