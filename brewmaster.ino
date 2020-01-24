#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <PubSubClient.h>

#define MOC3021_PIN 2
#define TEMP_PIN    4
#define mqtt_server "192.168.2.250"

WiFiMulti wifi;
WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "WM1";
const char* password = "WollemaN";

float temperature = 0;

OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

//Buffer to decode MQTT messages
char message_buff[100];

long lastMsg = 0;   
long lastRecu = 0;
bool debug = false;  //Display log message if True

void readSensor(void* parameter);
void sendTemperature(void* parameter);
 
void setup() {
 
  Serial.begin(115200);                                  //Serial connection
  sensors.begin();

  pinMode(MOC3021_PIN, OUTPUT);
  
  wifi.addAP(ssid, password);   //WiFi connection

  Serial.println("Waiting for connection");
  while (wifi.run() != WL_CONNECTED) {  //Wait for the WiFI connection completion
    delay(500);
  }
  Serial.println("Connected");

  WiFi.begin(ssid, password); // Connect to WiFi
 
  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");

   // Verbonden.
  Serial.println("OK!");
  
  // IP adres.
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
 
  xTaskCreate(readSensor, "Read temp", 1024, NULL, 1, NULL);
  xTaskCreate(sendTemperature, "Send temp", 2048, NULL, 2, NULL);
  
  }
} 

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void readSensor(void* parameter) {
  while(1) {
    sensors.requestTemperatures(); 
    temperature = sensors.getTempCByIndex(0);
  
    if(temperature > 30.2) { 
      digitalWrite(MOC3021_PIN, LOW);
      client.publish("domoticz/in", "{'command': 'switchlight', 'idx': 49, 'switchcmd': 'On' }");
    }
    if(temperature < 29.8) {
      digitalWrite(MOC3021_PIN, HIGH);
      client.publish("domoticz/in", "{'command': 'switchlight', 'idx': 49, 'switchcmd': 'Off' }");
    }

    vTaskDelay(10 / portTICK_RATE_MS); //Measure temperature every 10ms
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("wemos/out", "WEMOS MQTT actief");
      // ... and resubscribe
      client.subscribe("domoticz/out");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void sendTemperature(void* parameter) {
  while(1) {
    if(wifi.run()== WL_CONNECTED){   //Check WiFi connection status
      HTTPClient http;    //Declare object of class HTTPClient
      
      http.begin("http://192.168.2.250:8086/write?db=brew");      //Specify request destination
      http.addHeader("Content-Type", "text/plain");  //Specify content-type header
    
      char data[100];
      sprintf(data, "temperatuur,host=nodemcu,region=eu-nl value=%.2f", temperature);
      
      int httpCode = http.POST(data);   //Send the request
      String payload = http.getString();                  //Get the response payload
      
      http.end();  //Close connection
    } else {
      Serial.println("Error in WiFi connection");   
    }
    
    vTaskDelay(1000 / portTICK_RATE_MS);  //Send a request every second
  }
}
 
void loop() {
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
