#define WIFI_COMPATIBLE_BOARD 1
#define AQM_DEBUG 0

#include <Arduino.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <SdsDustSensor.h>
#include <DHT.h>
#include <fonts/Callibri15.h>
#include <fonts/System5x7.h>
#include <ezTime.h>


#if WIFI_COMPATIBLE_BOARD

#include "properties.h"

#endif

Timezone tz;

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
DHT dht;

char *dtostrf(double val, signed char width, unsigned char prec, char *sout) {
    char fmt[20];
    sprintf(fmt, "%%%d.%df", width, prec);
    sprintf(sout, fmt, val);
    return sout;
}

int *getPmAcceptability(PmResult pm) {
    if (pm.pm25 > 0 && pm.pm25 < 12) {
        return GREEN;
    } else if (pm.pm25 >= 12 && pm.pm25 < 35) {
        return YELLOW;
    } else if (pm.pm25 >= 35) {
        return RED;
    }
}

void displayOnScreen(PmResult pm, float humd, float temp) {
    char pm25_s[12];
    dtostrf(pm.pm25, 7, 2, pm25_s);
    char pm10_s[12];
    dtostrf(pm.pm10, 7, 2, pm10_s);
    char h_s[12];
    dtostrf(humd, 2, 0, h_s);
    char t_s[12];
    dtostrf(temp, 7, 1, t_s);

    //Flickers
    screen.clear();

    screen.setFont(System5x7);
    screen.setCursor(0, 0);
    screen.print(tz.dateTime("H:i"));
    screen.print(F("   "));
    screen.print(h_s);
    screen.print(F("% "));
    screen.print(t_s);
    screen.print((char) 128);
    screen.println(F("C"));

    screen.println();

    screen.setFont(Callibri15);
    screen.print(F("  PM 2.5 : "));
    screen.println(pm25_s);
    screen.print(F("  PM 10  : "));
    screen.println(pm10_s);
}

void showResults(int *status, PmResult pm, float humd, float temp) {
    //LED Output
    analogWrite(pinLEDR, status[0]);
    analogWrite(pinLEDG, status[1]);
    analogWrite(pinLEDB, status[2]);

    //OLED Output
    displayOnScreen(pm, humd, temp);
}

void initDisplay() {
    Wire.begin();
    Wire.setClock(400000L);

    screen.begin(&Adafruit128x64, 0x3C);

    screen.setFont(Callibri15);
    screen.clear();

    screen.println(F("Initializing..."));
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
    Serial.println(F("Initializing DHT"));
    dht.setup(pinDHT);
}

void setup() {
    Serial.begin(9600);
    delay(1500);
    Serial.println(F("Initializing"));

    initDisplay();
    initSDS();
    initDHT();
#if WIFI_COMPATIBLE_BOARD
    WiFi.begin(SSID, PASS);
    initIOT();
    waitForSync();
#endif
    tz.setLocation("America/Los_Angeles");
}

void loop() {
    delay(2000);

    PmResult pm = sds.readPm();
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();

    if (pm.isOk()) {
#if AQM_DEBUG
        Serial.print("PM2.5 = ");
        Serial.println(String(pm.pm25));
        Serial.print(", PM10 = ");
        Serial.println(String(pm.pm10));
#endif
        int *statusLight = getPmAcceptability(pm);
        showResults(statusLight, pm, humidity, temperature);

        pm10 = pm.pm10;
        pm25 = pm.pm25;
#if WIFI_COMPATIBLE_BOARD
        ArduinoCloud.update();
#endif
    } else {
#if AQM_DEBUG
        Serial.print(F("Could not read values from sensor, reason: "));
        Serial.println(pm.statusToString());
#endif
    }
    //Run NTP events.
    events();
}
