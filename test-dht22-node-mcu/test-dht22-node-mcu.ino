#include <ESP8266WiFi.h>
#include <DHT.h>
#include <DHT_U.h>
#include <PubSubClient.h>

// DHT11 sensor pins
#define DHTPIN D5
#define DHTTYPE DHT22

// Create aREST instance
aREST rest = aREST();

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);

// WiFi parameters
const char* ssid = "FPT Telecom";
const char* password = "PaPaH3t@123";   

// The port to listen for incoming TCP connections 
#define LISTEN_PORT           80

const char* mqtt_server = "192.168.1.62"; // Thông tin MQTT Broker cua Pi

const char* clientID = "light-node";

const char* outTopic = "light-heat-sensing-1";
const char* outTopic2 = "light-heat-sensing-2";

// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 200;              // interval at which to read sensor

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

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
}

// Hàm call back được gọi khi nhận được thông tin từ subcribing topic
void callback(char* topic, byte* payload, unsigned int length) {

  char* message = (char*) payload;
  float value = atof(message);
  Serial.print("temperature = ");
  Serial.println(value);
  if(value > 30.0){
    digitalWrite(D2,HIGH);
    Serial.println("D2 is hi");
  }else{
    digitalWrite(D2,LOW);
    Serial.println("D2 is lo");
  }
  
}

// Reconnect đến MQTT Broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, clientID);
      // ... and resubscribe
//      client.subscribe(inTopic);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void publishValue() {
//  count++;
//  dtostrf(count, 2, 2, msg);
//  Serial.println(msg);
  dtostrf(analogRead(A0), 2, 2, msg);
  String msg_photonresistor = msg;
  Serial.print("photonresistor:");
  Serial.println(msg);
  client.publish(outTopic, msg);
  
  delay(1000);
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
  publishValue();
  client.loop();
}
