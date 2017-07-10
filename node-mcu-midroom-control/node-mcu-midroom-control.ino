/*
 This is control node that subscribes to control topics in raspi

 9/7: adding D0: lamp
              D3: mist generator
           
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
const char* clientID = "corner-node-control-1";

const char* in_topic_temperature = "room/temperature-sensing-mid-room";
const char* in_topic_photon = "room/light-sensing-mid-room";
const char* in_topic_humidity = "room/humidity-sensing-mid-room";

const char* in_topic_lamp = "room/desk-lamp-midroom-control";
const char* in_topic_mist_generator = "room/mist-generator-midroom-control";

const char* out_topic_lamp = "room/desk-lamp-midroom-state";
const char* out_topic_mist_generator = "room/mist-generator-midroom-state";

const char* in_topic_toggle_automatic = "room/toogle-automatic";

float temperature;
float humidity;
float photon;

char automation_state = '1';
char switch_state;

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

void setup_wifi(){
  delay(1000);
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

  String topicName(topic);
  
  if(topicName == in_topic_toggle_automatic){
      automation_state = (char)payload[0];
      Serial.print("this is ...");
      Serial.println(automation_state);
      if(automation_state == '1'){
        client.subscribe(in_topic_photon);
        client.subscribe(in_topic_temperature);
        client.subscribe(in_topic_humidity);
        client.unsubscribe(in_topic_lamp);
        client.unsubscribe(in_topic_mist_generator);
        Serial.println("subscribed to frontdoor node");
      }
      if(automation_state == '0'){
        client.unsubscribe(in_topic_photon);
        client.unsubscribe(in_topic_temperature);
        client.unsubscribe(in_topic_humidity);
        client.subscribe(in_topic_lamp);
        client.subscribe(in_topic_mist_generator);
        Serial.println("bye bye frontdoor node");
      }
  }

  if(topicName == in_topic_photon){
    photon = atof((char*)payload);
    Serial.print("photon value from server:");
    Serial.println(photon);
    if(photon > 900){
      Serial.println("too dark here!");
      digitalWrite(D0,LOW);
    }else{
      digitalWrite(D0,HIGH);
    }
  }

  if(topicName == in_topic_temperature){
    temperature = atof((char*)payload);
    Serial.print("temperature from server:");
    Serial.println(temperature);
    if(temperature > 30.0){
      Serial.println("too hot at corner");
      digitalWrite(D1, LOW);      
    }else{
      digitalWrite(D1,HIGH);
    }
  }

  if(topicName == in_topic_lamp){
    switch_state = (char)payload[0];
    Serial.print("switch state = ");
    Serial.println(switch_state);
    if(switch_state == '1'){
      digitalWrite(D0,LOW);
      client.publish(out_topic_lamp,"1");    
    }else{
      digitalWrite(D0,HIGH);
      client.publish(out_topic_lamp,"0");          
    }
  }

  if(topicName == in_topic_mist_generator){
    Serial.println("hello mist generator");
    switch_state = (char)payload[0];
    if(switch_state == '1'){
      digitalWrite(D1,LOW);
      client.publish(out_topic_mist_generator,"1");
    }else{
      digitalWrite(D1,HIGH);
      client.publish(out_topic_mist_generator,"0");
    }
  }
}

void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT connection...");

    if(client.connect(clientID)){
      Serial.println("connected to mQTT broker");
      client.subscribe(in_topic_toggle_automatic);
      Serial.println("subscribed to 1 topic");
    }else{
      Serial.print("failed, rc=...");
      Serial.println(client.state());
      Serial.println("try again in 1 sec");
      delay(1000);
    }
  }
}

void publish_value(){
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
  
  pinMode(D0,OUTPUT);
  pinMode(D1,OUTPUT);
}

void loop() {
  if(!client.connected()){
    reconnect();
  }

//  delay(1000);  
  client.loop();
}
