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

#define NUM_LEDS 150
#define LS_PIN 6

CRGB leds [NUM_LEDS];

unsigned long currentMillis = 0;
int speed = 10;

int prevMillis = 0;
int currLed = 0;



void setup() {
  // put your setup code here, to run once:

    FastLED.addLeds<NEOPIXEL, LS_PIN>(leds, NUM_LEDS);
    FastLED.clear();

}

void loop() {
  // put your main code here, to run repeatedly:

  currentMillis = millis();

 /*for(int dot = 0; dot < NUM_LEDS; dot++) { 
            leds[dot] = CRGB::Blue;
            FastLED.show();
            // clear this led for the next time around the loop
            leds[dot] = CRGB::Black;
            delay(300);            
        }*/


/*
        if(currentMillis - prevMillis >= speed) {

          prevMillis = currentMillis;
          leds[currLed] = CRGB::Blue;
          FastLED.show();
          leds[currLed] = CRGB::Black;
          currLed++;
          if(currLed>NUM_LEDS) {
            currLed=0;            
          }
        }
  */
  
//RunningLights(0xff, 0xff, 0x00, 100);

runningLightsNoDelay(0xff, 0xff, 0x00);

}

void runningLightsNoDelay(byte red, byte green, byte blue) {
  if(currentMillis - prevMillis >= speed) {
    int position = 0;
    prevMillis = currentMillis;
    setPixel(currLed, ((sin(currLed+1)*127+128)/255)*red,
               ((sin(currLed+2)*127+128)/255)*green,
               ((sin(currLed+3)*127+128)/255)*blue);
    FastLED.show();
    currLed++;
    if(currLed+3>NUM_LEDS) {
      currLed = 0;
      FastLED.clear();
    }
            
    
  }

  
}
void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position=0;
  
  for(int j=0; j<NUM_LEDS*2; j++)
  {
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<NUM_LEDS; i++) {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue);
      }
      
      showStrip();
      delay(WaveDelay);
  }
}



void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
