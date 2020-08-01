//Requires GFX, SSD1306 libs
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>
#include <SdsDustSensor.h>

//R, G, B
int GREEN[] = {0x00, 0xff, 0x00};
int YELLOW[] = {0x00, 0x00, 0xff};//Yellow isn't very visible hence the choice of blue
int RED[] = {0xff, 0x00, 0x00};

//Nova sensor pins
int pinSDSRx = 11;
int pinSDSTx = 12;
//LED module pins
int pinLEDR = 6;
int pinLEDG = 5;
int pinLEDB = 3;

SdsDustSensor sds(pinSDSRx, pinSDSTx);
SSD1306AsciiAvrI2c screen;

int* getPmAcceptability(PmResult pm) {
  if(pm.pm25 > 0 && pm.pm25 < 12) {
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
  screen.setCursor(0,0);
  screen.print(F("PM 2.5 : "));
  screen.println(pm25_s);
  screen.println();
  screen.println();
  screen.print(F("PM 10  : "));
  screen.println(pm10_s);
}

void initDisplay() {
  screen.begin(&Adafruit128x64, 0x3C);

  screen.setFont(Callibri15);
  screen.clear();

  screen.println(F("Starting"));
  delay(3000);
  screen.clear();
}

void setup() {
  Serial.begin(9600);  

  initDisplay();
  sds.begin();
}

void loop() {
  delay(2000);

  PmResult pm = sds.readPm();

  Serial.print("PM2.5 = ");
  Serial.println(String(pm.pm25));
  Serial.print(", PM10 = ");
  Serial.println(String(pm.pm10));

  int *status = getPmAcceptability(pm);
  showResults(status, pm);  
}
