#include <WiFi.h>

const char* ssid = "WM1";
const char* password =  "WollemaN";

void setup() {
 
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);   //WiFi connection

  Serial.print("Waiting for connection");
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Connected");
}

void loop() {

}