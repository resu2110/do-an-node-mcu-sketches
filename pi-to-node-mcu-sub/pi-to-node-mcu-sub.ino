#include <DHT.h>
#include <DHT_U.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Thông tin Wifi
const char* ssid = "pi-hi-hi";
const char* password = "123456789";

const char* mqtt_server = "172.24.1.1"; 

// Thông tin tin clientID, publishing, subcribing topic. ClientID nên là duy nhất trong tất cả các ứng dụng
// Thay đổi thông tin ở đây tương ứng với thông tin các bạn muốn
const char* clientID = "light-relay-node";

//topic.....
const char* inTopic = "light-heat-sensing-1";
const char* inTopic2 = "light-heat-sensing-2";
const char* homeassistant_topic = "ha_light";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
String switch1;


// Hàm kết nối Wifi
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(D2, OUTPUT);
  pinMode(D6,OUTPUT);
}

// Hàm call back được gọi khi nhận được thông tin từ subcribing topic
void callback(char* topic, byte* payload, unsigned int length) {
 Serial.println("callbakinggggg");
 
 String topicName(topic);
 
 if(topicName == "ha_light"){
  Serial.println("true topic");
  switch1 = String((char*)payload);
  if(switch1 == "OFF"){
    digitalWrite(5,LOW);
    Serial.println("LO");
  }else{
    digitalWrite(5,HIGH);
    Serial.println("HI");
  }
 }else{
  Serial.println("different topic!!!");
 }

  

//  String topicName(topic);
//  if(topicName == "light-heat-sensing-1"){
//    char* message = (char*) payload;
//    int value = atoi(message);
//    Serial.println("photonresistor: ");
//    Serial.println(value);
//    if (value > threshold) {
//      digitalWrite(D2, LOW);
//      Serial.println("Low");
//    } else {
//      digitalWrite(D2, HIGH);
//      Serial.println("LOW");
//    }  
//    
//  }else if(topicName == "light-heat-sensing-2"){
//    char* message = (char*) payload;
//    double value = atof(message);
//    Serial.println("temperature: ");
//    Serial.println(value);
//    
//    if (value > 85.0) {
//      digitalWrite(D6, LOW);
//      Serial.println("Low");
//    } else {
//      digitalWrite(D6, HIGH);
//      Serial.println("Hii");
//    }  
//    
//  }
  
}

// Reconnect đến MQTT Broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID)) {
      Serial.println("connected");
      client.subscribe(homeassistant_topic);
      Serial.print("subscribed to...");
      Serial.println(homeassistant_topic);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  Serial.println();
  delay(1000);
  client.loop();
}
