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
#include <TimerOne.h>
#include <ClickEncoder.h>

// pins for rotary encoder 1
#define RE1_PIN1 A8
#define RE1_PIN2 A9
#define RE1_BTN_PIN A10


// pins for light strip 1
#define LIGHTSTRIP1_NUM_LEDS 150
#define LIGHTSTRIP1_PIN 6

// millisecond counter for the loop
unsigned long currentMillis = 0;
unsigned long prevMillis = 0;

// rotary encoder 1 setup
ClickEncoder *re1;
int16_t re1_last_value, re1_value;
void timerIsr() {
  re1->service();
}

// light strip 1 setup
CRGB lightstrip1[LIGHTSTRIP1_NUM_LEDS];
int current_lightstrip1_led = 0;
int speed = 10;


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  // rotary enocder setup code
  // call to this is encoderA, encoderB, button
  re1 = new ClickEncoder(RE1_PIN1, RE1_PIN2, RE1_BTN_PIN);
  Timer1.initialize();
  Timer1.attachInterrupt(timerIsr); 
  re1->setAccelerationEnabled("enabled");
  re1_last_value = -1;

  // light strip setup
  FastLED.addLeds<NEOPIXEL, LIGHTSTRIP1_PIN>(lightstrip1, LIGHTSTRIP1_NUM_LEDS);
  FastLED.clear();

  init_lightstrip(lightstrip1, LIGHTSTRIP1_NUM_LEDS);

}

void loop() {

  currentMillis = millis();

  //runningLights(0xff, 0xff, 0x00);


//  Serial.print("A8: ");
 // Serial.println(analogRead(A8));
 //   Serial.print("A9: ");
 // Serial.println(analogRead(A9));
 //   Serial.print("A10: ");
 // Serial.println(analogRead(A10));



//moveledsonlightstrip(lightstrip1, LIGHTSTRIP1_NUM_LEDS, 1);

rotatelightstrip(lightstrip1, LIGHTSTRIP1_NUM_LEDS, 1);



  re1_value += re1->getValue();


  if (re1_value != re1_last_value) {
    int directionn = 1;
    if(re1_value < re1_last_value) {
     directionn = -1;
    } 
     moveledsonlightstrip(lightstrip1, LIGHTSTRIP1_NUM_LEDS, directionn);
    re1_last_value = re1_value;
    Serial.print("Encoder Value: ");
    Serial.println(re1_value);
  }

  ClickEncoder::Button re1_btn = re1->getButton();
  if (re1_btn != ClickEncoder::Open) {
    Serial.print("Button: ");
#define VERBOSECASE(label) case label: Serial.println(#label); break;
    switch (re1_btn) {
        VERBOSECASE(ClickEncoder::Pressed);
        VERBOSECASE(ClickEncoder::Held)
        VERBOSECASE(ClickEncoder::Released)
        VERBOSECASE(ClickEncoder::Clicked)
        VERBOSECASE(ClickEncoder::DoubleClicked)
       
        break;
    }

  }
}


void init_lightstrip(CRGB* lightstrip, int num_leds) {
  int num_sections = 4;
  int leds_per_section = num_leds/num_sections;
  int currRed = random(0,256);
  int currGreen = random(0,256);
  int currBlue = random(0,256);
  Serial.println("here");
  Serial.print("num_leds: ");
  Serial.println(num_leds);
  for(int currled=0; currled<num_leds; currled++) {
    lightstrip[currled] = CRGB(currRed, currGreen, currBlue);

    if(currled % leds_per_section == 0) {
      currRed = random(0,256);
      currGreen = random(0,256);
      currBlue = random(0,256);
    }

  }
  FastLED.show();
  
}

void moveledsonlightstrip(CRGB* lightstrip, int num_leds, int directionn) {
  lightstrip[num_leds] = CRGB(lightstrip[0].red, lightstrip[0].green, lightstrip[0].blue);
  for(int currled = num_leds; currled > 1; currled--) {
    lightstrip[currled] = CRGB(lightstrip[currled-1].red, lightstrip[currled-1].green, lightstrip[currled-1].blue);
  }
  FastLED.show();
}


void rotatelightstrip(CRGB* lightstrip, int num_leds, int directionn) {
  if(directionn > 0) {
    // move forward
    CRGB temp = lightstrip[0];
    for(int currled=0; currled<num_leds-1; currled++) {
      lightstrip[currled] = CRGB(lightstrip[currled+1].red, lightstrip[currled+1].green, lightstrip[currled+1].blue);
      FastLED.show();      
    }
    lightstrip[num_leds-1] = temp;
    FastLED.show();
  } else {
    // move backward
    CRGB temp = lightstrip[num_leds-1];
    for(int currled=num_leds; currled>0; currled--) {
      lightstrip[currled] = CRGB(lightstrip[currled-1].red, lightstrip[currled-1].green, lightstrip[currled-1].blue);
      FastLED.show();
    }
    lightstrip[0] = temp;
    FastLED.show();
  }
  
}


void runningLights(byte red, byte green, byte blue) {
  if(currentMillis - prevMillis >= speed) {
    int position = 0;
    prevMillis = currentMillis;
    setPixel(current_lightstrip1_led, ((sin(current_lightstrip1_led+1)*127+128)/255)*red,
               ((sin(current_lightstrip1_led+2)*127+128)/255)*green,
               ((sin(current_lightstrip1_led+3)*127+128)/255)*blue);
    FastLED.show();
    current_lightstrip1_led++;
    if(current_lightstrip1_led+3>LIGHTSTRIP1_NUM_LEDS) {
      current_lightstrip1_led = 0;
      FastLED.clear();
    }
            
    
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
   lightstrip1[Pixel].r = red;
   lightstrip1[Pixel].g = green;
   lightstrip1[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < LIGHTSTRIP1_NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
