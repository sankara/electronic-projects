#include <Arduino.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <SdsDustSensor.h>
#include "properties.h"


//R, G, B
int GREEN[] = {0x00, 0xff, 0x00};
int YELLOW[] = {0x00, 0x00, 0xff};//Yellow isn't very visible hence the choice of blue
int RED[] = {0xff, 0x00, 0x00};

//LED module pins
int pinLEDR = 6;
int pinLEDG = 5;
int pinLEDB = 3;

SdsDustSensor sds(Serial1);//Serial1 is Hardware Rx/Tx on the nano
SSD1306AsciiWire screen;

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

void showResults(int* status, PmResult pm) {
  char pm25_s[12];
  dtostrf(pm.pm25, 7, 2, pm25_s);
  char pm10_s[12];
  dtostrf(pm.pm10, 7, 2, pm10_s);

  //LED Output
  analogWrite(pinLEDR, status[0]);
  analogWrite(pinLEDG, status[1]);
  analogWrite(pinLEDB, status[2]);

  //OLED Output
  screen.setCursor(0, 0);
  screen.print(F("PM 2.5 : "));
  screen.println(pm25_s);
  screen.println();
  screen.println();
  screen.print(F("PM 10  : "));
  screen.println(pm10_s);
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

void setup() {
  Serial.begin(9600);
  delay(1500);
  Serial.println("initializing");

  initDisplay();
  initSDS();
  initIOT();
}

void loop() {
  delay(2000);

  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    Serial.print("PM2.5 = ");
    Serial.println(String(pm.pm25));
    Serial.print(", PM10 = ");
    Serial.println(String(pm.pm10));
    int *statusLight = getPmAcceptability(pm);
    showResults(statusLight, pm);

    pm10 = pm.pm10;
    pm25 = pm.pm25;
    ArduinoCloud.update();
  } else {
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
  }
}
