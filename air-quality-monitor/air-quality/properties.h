#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "secrets.h"

float pm10;
float pm25;
CloudTemperature temperature;
float humidity;

void initProperties(){

  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(pm10, READ, ON_CHANGE, NULL, 0.5);
  ArduinoCloud.addProperty(pm25, READ, ON_CHANGE, NULL, 0.5);
  ArduinoCloud.addProperty(temperature, READ, ON_CHANGE, NULL, 0.1);
  ArduinoCloud.addProperty(humidity, READ, ON_CHANGE, NULL, 1.0);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
