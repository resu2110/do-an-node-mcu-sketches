#include <DHT.h>
#include <DHT_U.h>

#include <ESP8266WiFi.h>


#define DHTTYPE DHT22
#define DHTPIN D5

// API key thingspeak
String apiKey = "XKU2HWEXKKCWWSM7"; //api thingspeak
const char* ssid = "FPT Telecom"; 
const char* password = "PaPaH3t@123";
const char* server = "api.thingspeak.com";
int OutPin = A0; // chân kết nối cảm biến độ ẩm 

DHT dht(DHTPIN,DHTTYPE, 15);
WiFiClient client;
char msg[50];
  
void setup() {                
  Serial.begin(115200);
   
  WiFi.begin(ssid, password);
 
  Serial.println();
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

  pinMode(D5,INPUT);
}

void loop() {  
  dtostrf(dht.readTemperature(DHTPIN),3,3,msg);
  String heat = msg;  
  dtostrf(dht.readHumidity(DHTPIN),3,3,msg);
  String humidity = msg;
  
  //Đẩy dữ lên thingspeak
  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
           postStr +="&field1=";
           postStr += heat;
           postStr += "\r\n\r\n";
           postStr +="&field2=";
           postStr += humidity;
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n"); 
     client.print("Host: api.thingspeak.com\n"); 
     client.print("Connection: close\n"); 
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n"); 
     client.print("Content-Type: application/x-www-form-urlencoded\n"); 
     client.print("Content-Length: "); 
     client.print(postStr.length()); 
     client.print("\n\n"); 
     client.print(postStr);

     
           
     Serial.print(" heat =  ");
     Serial.print(heat);
     Serial.print(" hum =  ");
     Serial.print(humidity);
  }

  client.stop();
   
  Serial.println("Waiting...");    
  
  delay(2000);
}
