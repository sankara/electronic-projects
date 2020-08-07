#define WIFI_COMPATIBLE_BOARD 1

#include <Arduino.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <SdsDustSensor.h>
#include <DHT.h>

#ifdef WIFI_COMPATIBLE_BOARD
#include "properties.h"
#endif



//R, G, B
int GREEN[] = {0x00, 0xf, 0x00};
int YELLOW[] = {0x00, 0x00, 0xf};//Yellow isn't very visible hence the choice of blue
int RED[] = {0xf, 0x00, 0x00};

//LED module pins
int pinLEDR = 6;
int pinLEDG = 5;
int pinLEDB = 3;

//DHT Sensor Pin
int pinDHT = 8;

SdsDustSensor sds(Serial1);//Serial1 is Hardware Rx/Tx on the nano
SSD1306AsciiWire screen;
DHT dht(pinDHT, DHT11);

char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}

int* getPmAcceptability(PmResult pm) {
  if (pm.pm25 > 0 && pm.pm25 < 12) {
    return GREEN;
  } else if (pm.pm25 >= 12 && pm.pm25 < 35) {
    return YELLOW;
  } else if (pm.pm25 >= 35) {
    return RED;
  }
}

void displayOnScreen(PmResult pm, float humidity, float temperature) {
  char pm25_s[12];
  dtostrf(pm.pm25, 7, 2, pm25_s);
  char pm10_s[12];
  dtostrf(pm.pm10, 7, 2, pm10_s);
  char h_s[12];
  dtostrf(humidity, 2, 0, h_s);
  char t_s[12];
  dtostrf(temperature, 7, 1, t_s);

  //Flickers
  screen.clear();

  screen.setCursor(0, 0);
  screen.print(h_s);
  screen.print(F("% "));
  screen.print(t_s);
  screen.print("  ");
  screen.print((char)248);
  screen.println("C");

  screen.println();

  screen.print(F("PM 2.5 : "));
  screen.println(pm25_s);
  screen.print(F("PM 10  : "));
  screen.println(pm10_s);
}

void showResults(int* status, PmResult pm, float humidity, float temperature) {
  //LED Output
  analogWrite(pinLEDR, status[0]);
  analogWrite(pinLEDG, status[1]);
  analogWrite(pinLEDB, status[2]);

  //OLED Output
  displayOnScreen(pm, humidity, temperature);
}

void initDisplay() {
  Wire.begin();
  Wire.setClock(400000L);

  screen.begin(&Adafruit128x64, 0x3C);

  screen.setFont(Callibri15);
  screen.clear();

  screen.println(F("Starting..."));
  delay(3000);
}

void initSDS() {
  sds.begin();
  Serial.println(sds.queryFirmwareVersion().toString()); // prints firmware version
  Serial.println(sds.setActiveReportingMode().toString()); // ensures sensor is in 'active' reporting mode
  //Serial.println(sds.setContinuousWorkingPeriod().toString());
}

void initIOT() {
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();  
}

void initDHT() {
  Serial.println("Initializing DHT");
  dht.begin();
}

void setup() {
  Serial.begin(9600);
  delay(1500);
  Serial.println("initializing");

  initDisplay();
  initSDS();
  initDHT();
#ifdef WIFI_COMPATIBLE_BOARD
  initIOT();
#endif
}

void loop() {
  delay(2000);

  PmResult pm = sds.readPm();
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  if (pm.isOk()) {
    Serial.print("PM2.5 = ");
    Serial.println(String(pm.pm25));
    Serial.print(", PM10 = ");
    Serial.println(String(pm.pm10));
    int *statusLight = getPmAcceptability(pm);
    showResults(statusLight, pm, humidity, temperature);

    pm10 = pm.pm10;
    pm25 = pm.pm25;
#ifdef WIFI_COMPATIBLE_BOARD
    ArduinoCloud.update();
#endif
  } else {
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
  }
}
