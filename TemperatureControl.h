#include <WiFi.h>
#include <HTTPClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef TEMPERATURE_CONRTOL_H_
#define TEMPERATURE_CONRTOL_H_

#define BURNER_PIN  2
#define TEMP_PIN    4

#define INFLUX_DB   "http://192.168.2.250:8086/write?db=brew"

class TemperatureControl
{
private:
    double temperature;

    OneWire* oneWire;
    DallasTemperature* sensor;
public:
    TemperatureControl();
    void controlTemperature(void* parameter);
    void sendTemperature(void* parameter);
};


#endif