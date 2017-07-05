
/**
  This is for mid-room-switch
    14-6: relay on D1 & D2
          D1: works with photon - a lamp
          D2: works with dht-temperature - a mist generator, 
              can be controlled manually

    29-6: remove converter from F to C cuz the publisher already did it
*/
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "pi-hi-hi";
const char* pass = "123456789";

const char* mqtt_server = "172.24.1.1";
const char* in_topic_photon = "room/light-sensing-mid-room";
const char* in_topic_temperature = "room/temperature-sensing-mid-room";
const char* in_topic_manual = "room/manual-control/mist-generator-1";
const char* clientID = "mid-room-switch";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to...");
  Serial.println(ssid);

  WiFi.begin(ssid,pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println(".");
  }

  Serial.println();
  Serial.print("connected to....");
  Serial.println(ssid);
  Serial.print(WiFi.localIP());
}

//mqt reconnect
void reconnect(){
  while(!client.connected()){
    Serial.println("Attempting MQTT connect4ion...");

    if(client.connect(clientID)){
      Serial.println("connected to MQTT broker");
      client.subscribe(in_topic_photon);
      client.subscribe(in_topic_temperature);
      client.subscribe(in_topic_manual);
      Serial.println("subscribed to intopics");
    }else{
      Serial.print("failed.. retryinggg rc = ");
      Serial.println(client.state());
      delay(1000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){

  String topic_name(topic);
  
  if(topic_name == in_topic_photon){    
    float photon_value = atof((char*)payload);    
    if(photon_value > 900){
      digitalWrite(D1,LOW);
    }else{
      digitalWrite(D1,HIGH);
    }
  }else if(topic_name == in_topic_temperature){    
    float temperature_value = atof((char*)payload);
//    temperature_value = (temperature_value-32)/1.8; //convert from F to Celsius
//    Serial.println(temperature_value);
    
     if(temperature_value > 31){ //launch the mist generator
      digitalWrite(D2,LOW);
    }else{
      digitalWrite(D2,HIGH);
    }
  }else if(topic_name == in_topic_manual){
    String command = String((char*)payload);
    Serial.println(command);

    if(command == "ON0-1"){  //lanuch switch of the mist generator manually      
      digitalWrite(D2,LOW);      
    }else{
      digitalWrite(D2,HIGH);
    }
  }else{
    Serial.println("shit");
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
}

void loop() {
  if(!client.connected()){
    reconnect();
  }  
  client.loop();
}
