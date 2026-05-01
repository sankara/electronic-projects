/*
  Fade

  This example shows how to fade an LED on pin 9 using the analogWrite()
  function.

  The analogWrite() function uses PWM, so if you want to change the pin you're
  using, be sure to use another PWM capable pin. On most Arduino, the PWM pins
  are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fade
*/

int led_r = 6;           // the PWM pin the LED is attached to
int led_g = 5;
int led_b = 3;

// the setup routine runs once when you press reset:
void setup() {
}

// the loop routine runs over and over again forever:
void loop() {
  analogWrite(led_r, random(0,255));
  analogWrite(led_g, random(0,255));
  analogWrite(led_b, random(0,255));
  delay(1000);
}
