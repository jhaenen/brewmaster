#include <WiFi.h>
#include <HTTPClient.h>

#include "TemperatureControl.h"

const char* ssid = "WM1";
const char* password =  "WollemaN";
 
void setup() {
 
  Serial.begin(115200);                                  //Serial connection
  
  WiFi.begin(ssid, password);   //WiFi connection

  TemperatureControl tc;

  Serial.print("Waiting for connection");
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Connected");

  xTaskCreate(tc.controlTemperature, "Control temp", 1024, NULL, 1, NULL);
  xTaskCreate(tc.sendTemperature, "Send temp", 2048, NULL, 0, NULL);

  vTaskStartScheduler(); 
}
 
void loop() {

}
