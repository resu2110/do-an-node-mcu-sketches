#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

const char* ssid = "FPT Telecom";
const char* pass = "PaPaH3t@123";

const char* mqtt_server = "192.168.1.51";

WiFiClient espClient;
PubSubClient client(espClient);

const int ledGPIO5 = 5;
const int ledGPIO4 = 4;

void setup_wifi(){
  delay(100);
  Serial.println(".");
  Serial.println("connecting to.....");
  Serial.println(ssid);
  WiFi.begin(ssid,pass);
  
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println(".");
  }

  Serial.println("Connected!!!");
  Serial.println(WiFi.localIP());
  
}


// this function executed when some dev pub a mess to a topic that this Node is subscribing to

void callback(String topic, byte* message, unsigned int length){
  Serial.print("message arrived on topic: ");
  Serial.print(topic);
  Serial.println();

  String messageTemp;
  for(int i = 0 ; i < length ; i++){
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  if(topic == "nodeMCU/4"){
    Serial.print("Changing GPIO 4 to... ");
    if(messageTemp == "1"){
      digitalWrite(ledGPIO4, HIGH);
      Serial.print("ONNN"); 
    }else if(messageTemp == "0"){
      digitalWrite(ledGPIO4, LOW);
      Serial.print("OFF"); 
    }
  }
    if(topic == "nodeMCU/5"){
    Serial.print("Changing GPIO 5 to... ");
    if(messageTemp == "1"){
      digitalWrite(ledGPIO5, HIGH);
      Serial.print("ONNN"); 
    }else if(messageTemp == "0"){
      digitalWrite(ledGPIO5, LOW);
      Serial.print("OFF"); 
    }
  
  }
  Serial.println();
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
     /*
     YOU  NEED TO CHANGE THIS NEXT LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a unique name to the ESP8266.
     Here's how it looks like now:
       if (client.connect("ESP8266Client")) {
     If you want more devices connected to the MQTT broker, you can do it like this:
       if (client.connect("ESPOffice")) {
     Then, for the other ESP:
       if (client.connect("ESPGarage")) {
      That should solve your MQTT multiple connections problem

     THE SECTION IN loop() function should match your device name
    */
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("nodeMCU/4");
      client.subscribe("nodeMCU/5");
      Serial.println("to 2 topic 4 and 5");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void setup() {
  // put your setup code here, to run once:
  pinMode(ledGPIO4, OUTPUT);
  pinMode(ledGPIO5, OUTPUT);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()){
    reconnect();
  }

  if(!client.loop()){
    client.connect("nodeMCUClient");
  }
}
