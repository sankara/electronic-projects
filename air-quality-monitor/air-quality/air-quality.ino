//Requires GFX, SSD1306 libs
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMono9pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool displayConnected = false;

//Nova sensor pins
int pinSDSRx = 11;
int pinSDSTx = 12;

#include <SDS011.h>
SDS011 sds;

//LED module pins
int pinLEDR = 6;
int pinLEDG = 5;
int pinLEDB = 3;


//R, G, B
int GREEN[] = {0x00, 0xff, 0x00};
int YELLOW[] = {0x00, 0x00, 0xff};//Yellow isn't very visible hence the choice of blue
int RED[] = {0xff, 0x00, 0x00};

struct PmResult {
  float pm25 = -1.0;
  float pm10 = -1.0;

  PmResult(const float _pm25, const float _pm10) {
    pm25 = _pm25;
    pm10 = _pm10;
  }

  String toString() {
    return "pm25: " + String(pm25) + ", pm10: " + String(pm10);
  }
};

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
  //LED Output
  analogWrite(pinLEDR, status[0]);
  analogWrite(pinLEDG, status[1]);
  analogWrite(pinLEDB, status[2]);

  //OLED Output
  Serial.println(displayConnected);
  if(displayConnected) {
    display.clearDisplay();
    display.setCursor(0, 13); 
    display.print(F("PM2.5: "));
    display.print(pm.pm25);
    display.setCursor(0, 50); 
    display.print(F("PM10: "));
    display.print(pm.pm10);
    display.display();
  }
}

bool checkDisplay() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      return false;
    }
  return true;
}

void initDisplay() {
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  //display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.setFont(&FreeMono9pt7b);
  display.println(F("Hello, welcome to PM monitor"));
  display.display();      // Show initial text
  delay(1000);
  display.clearDisplay();
  display.display();
}

void setup() {
  Serial.begin(9600);  

  if(displayConnected = checkDisplay()) {
    initDisplay();
  }

  sds.begin(pinSDSRx, pinSDSTx);
}

void loop() {
  delay(2000);

  float p25, p10;
  sds.read(&p25, &p10);
  PmResult pm(p25, p10);

  Serial.print("PM2.5 = ");
  Serial.println(String(pm.pm25));
  Serial.print(", PM10 = ");
  Serial.println(String(pm.pm10));

  int *status = getPmAcceptability(pm);
  showResults(status, pm);  
}
