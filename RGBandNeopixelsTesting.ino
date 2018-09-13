#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

#define SINGLE_LED_PIN 23
#define SINGLE_BTN_PIN 34

#define RGB_RED_PIN 12
#define RGB_GRN_PIN 11
#define RGB_BLU_PIN 10

#define POT_PIN A15

#define NUM_LEDS 150
#define LS_PIN 6


CRGB leds [NUM_LEDS];

void setup() {
  // put your setup code here, to run once:

  pinMode(SINGLE_LED_PIN, OUTPUT);
  pinMode(SINGLE_BTN_PIN, INPUT_PULLUP);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GRN_PIN, OUTPUT);
  pinMode(RGB_BLU_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);
  Serial.begin(9600);
  Serial.println("reset");
  digitalWrite(SINGLE_LED_PIN, LOW);

  randomSeed(analogRead(0));

  FastLED.addLeds<NEOPIXEL, LS_PIN>(leds, NUM_LEDS);

}

void loop() {
  // put your main code here, to run repeatedly:


  int buttonState = 0;





/*
  setRGBColor(255, 0, 0);
  delay(5000);
  setRGBColor(0, 255, 0);
  delay(5000);
  setRGBColor(0, 0, 255);
  delay(5000);
  

  for(int i=0; i<10; i++) {
   long randNumberR = random(1,254);
   long randNumberG = random(1,254);
   long randNumberB = random(1,254);

    setRGBColor(randNumberR, randNumberG, randNumberB);
    delay(1000);
  }
  */


  /*
int val = analogRead(POT_PIN);
        int numLedsToLight = map(val, 0, 1023, 0, NUM_LEDS);

        // First, clear the existing led values
        FastLED.clear();
        for(int led = 0; led < numLedsToLight; led++) { 
            leds[led] = CRGB::Blue; 
        }
        FastLED.show();

  */


/*
  int potSense = analogRead(POT_PIN)/4;

  setRGBColor(0, 0, potSense);
  delay(500);
  setRGBColor(0, potSense, 0);
  delay(500);
  setRGBColor(potSense, 0, 0);
  delay(500);

  */
  

  /*
  buttonState = digitalRead(SINGLE_BTN_PIN);
  if(buttonState == HIGH) {
     digitalWrite(SINGLE_LED_PIN, LOW);
     Serial.println("READHIGH");
  }
  if(buttonState == LOW) {
    digitalWrite(SINGLE_LED_PIN, HIGH);
    Serial.println("READLOW");
  }
  */


  theaterChaseRainbow(1, 50);
  FastLED.clear();
  FastLED.show();
  delay(5000);

}

void setRGBColor(int red, int green, int blue) {
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GRN_PIN, green);
  analogWrite(RGB_BLU_PIN, blue);
}

void theaterChaseRainbow(int cycles, int speed){ // TODO direction, duration
  for (int j=0; j < 256 * cycles; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < NUM_LEDS; i=i+3) {
        int pos = i+q;
        leds[pos] = Wheel( (i+j) % 255);    //turn every third pixel on
      }
      FastLED.show();

      delay(speed);

      for (int i=0; i < NUM_LEDS; i=i+3) {
        leds[i+q] = CRGB::Black;  //turn every third pixel off
      }
    }
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
CRGB Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
