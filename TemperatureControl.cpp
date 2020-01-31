#include "TemperatureControl.h"

TemperatureControl::TemperatureControl() {
    oneWire = new OneWire(TEMP_PIN);
    sensor = new DallasTemperature(oneWire);

    sensor->begin();
    pinMode(BURNER_PIN, HIGH);
}

void TemperatureControl::controlTemperature(void* parameter) {
  while(1) {
    sensor->requestTemperatures(); 
    temperature = sensor->getTempCByIndex(0);
  
    if(temperature > 30.2) digitalWrite(BURNER_PIN, LOW);
    if(temperature < 29.8) digitalWrite(BURNER_PIN, HIGH);

    vTaskDelay(10 / portTICK_RATE_MS); //Measure temperature every 10ms
  }
}

void TemperatureControl::sendTemperature(void* parameter) {
  while(1) {
    if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status
      HTTPClient http;    //Declare object of class HTTPClient
      
      http.begin(INFLUX_DB);      //Specify request destination
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