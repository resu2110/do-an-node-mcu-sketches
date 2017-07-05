#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/*
 * Code for relay that controlled by 
 * photonresistor value through MQTT./
 * 
 * Each connected device will bragging 
 * about its presence right after connected
 * to MQTT broker into a topic called
 * bragging./
 * 
 * This device will subscribe to 
 * environment topics./
 */
 
const char* ssid = "pi-hi-hi";
const char* pass = "123456789";

const char* mqtt_server = "172.24.1.1";
const char* clientID = "photon_switch";
const char* in_topic_photon = "environment/photon";
const char* out_topic_bragging = "bragging/relay1";
const char* in_topic_manual = "manual_control/photon";

String relay1;
int photonValue;

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

unsigned int threshold = 600;

void setup_wifi(){
  Serial.print("Connecting to..");
  Serial.println(ssid);
  
  WiFi.begin(ssid,pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("successfully connected to ");
  Serial.println(ssid);  
}

//mqtt reconnect
void reconnect(){
  while(!client.connected()){
    Serial.print("Atempting MQTT connection...");

    if(client.connect(clientID)){
      Serial.println("connected to MQTT broker");
      client.publish(out_topic_bragging,clientID);  
//      client.subscribe(in_topic_photon); 
      client.subscribe(in_topic_manual); 
      Serial.println("subscribed to in_topic!");
         
    }else{
      Serial.print("failed, rc = ");
      Serial.println(client.state());
      Serial.println("try again in 1 sec...");
      delay(1000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){

  String topicName(topic);
  
//  if(topicName == "environment/photon"){    
//    photonValue = atoi((char*)payload);
//    Serial.println(photonValue);
//    if(photonValue > 600){
//      digitalWrite(D5,LOW);
//    }else{
//      digitalWrite(D5,HIGH);
//    }
//  }
  
  if(topicName == "manual_control/photon"){
    relay1 = String((char*)payload);
    Serial.print("manual..");
    Serial.println(relay1);
    if(relay1 == "ONFon"){
      digitalWrite(D5,HIGH); 
    }else if(relay1 == "OFFon"){
      digitalWrite(D5,LOW);
    }
  }else{
    Serial.println("shit...");
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
  pinMode(D5,OUTPUT);
}

void loop() {
  if(!client.connected()){
    reconnect();
  }
  Serial.println();
  delay(1000);
  client.loop();
}
