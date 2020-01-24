#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define MOC3021_PIN 2
#define TEMP_PIN    4

WiFiMulti wifi;

float temperature = 0;

OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

void readSensor(void* parameter);
void sendTemperature(void* parameter);
 
void setup() {
 
  Serial.begin(115200);                                  //Serial connection
  sensors.begin();

  pinMode(MOC3021_PIN, OUTPUT);
  
  wifi.addAP("WM1", "WollemaN");   //WiFi connection

  Serial.println("Waiting for connection");
  while (wifi.run() != WL_CONNECTED) {  //Wait for the WiFI connection completion
    delay(500);
  }
  Serial.println("Connected");

  xTaskCreate(readSensor, "Read temp", 1024, NULL, 1, NULL);
  xTaskCreate(sendTemperature, "Send temp", 2048, NULL, 2, NULL);
  
}

void readSensor(void* parameter) {
  while(1) {
    sensors.requestTemperatures(); 
    temperature = sensors.getTempCByIndex(0);
  
    if(temperature > 30.2) digitalWrite(MOC3021_PIN, LOW);
    if(temperature < 29.8) digitalWrite(MOC3021_PIN, HIGH);

    vTaskDelay(10 / portTICK_RATE_MS); //Measure temperature every 10ms
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

}
