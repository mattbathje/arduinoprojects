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


// pins for rotary encoder 1
#define RE1_PINA A4
#define RE1_PINB A5
#define RE1_BTN_PIN A6


// pins for light strip 1
#define LIGHTSTRIP1_NUM_LEDS 150
#define LIGHTSTRIP1_PIN 6

// light strip 1 setup
CRGB lightstrip1[LIGHTSTRIP1_NUM_LEDS];
int current_lightstrip1_led = 0;
int speed = 10;

// rotary encoder 1 setup
int lastEncoded = 0;
int encoderValue = 0;
int lastencoderValue = 0;

unsigned long currentMillis = 0;
unsigned long prevMillis = 0;


void setup() {
  // put your setup code here, to run once:

 // light strip setup
  FastLED.addLeds<NEOPIXEL, LIGHTSTRIP1_PIN>(lightstrip1, LIGHTSTRIP1_NUM_LEDS);
  FastLED.clear();
  init_lightstrip(lightstrip1, LIGHTSTRIP1_NUM_LEDS);


  // rotary encoder setup
  pinMode(RE1_PINA, INPUT_PULLUP);
  pinMode(RE1_PINB, INPUT_PULLUP);
  pinMode(RE1_BTN_PIN, INPUT_PULLUP);


  //get re1 starting position
  int lastMSB = digitalRead(RE1_PINA);
  int lastLSB = digitalRead(RE1_PINB);
  //let start be lastEncoded so will index on first click
  lastEncoded = (lastMSB << 1) | lastLSB;

  Serial.print("Starting Position AB  " );
  Serial.print(lastMSB);
  Serial.println(lastLSB);

}

void loop() {
  // put your main code here, to run repeatedly:

 currentMillis = millis();

  int MSB = digitalRead(RE1_PINA); //MSB = most significant bit
  int LSB = digitalRead(RE1_PINB); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time

  if (encoderValue != lastencoderValue) {
    Serial.print("Index:  ");
    Serial.println(encoderValue);

    Serial.print("Binary Sum:  ");
    for (int i = 3; i >= 0; i--) //routine for printing full 4 bits, leading zeros
    {
      Serial.print((sum >> i) & 0x01);
    }

    Serial.println();

    if (encoderValue > lastencoderValue) {
      rotatelightstrip(lightstrip1, LIGHTSTRIP1_NUM_LEDS, 1);
    } else {
      rotatelightstrip(lightstrip1, LIGHTSTRIP1_NUM_LEDS, -1);
    }


    lastencoderValue = encoderValue;
  }

  //reset index
  if (digitalRead(RE1_BTN_PIN) == LOW) {
    encoderValue = 0;
    init_lightstrip(lightstrip1, LIGHTSTRIP1_NUM_LEDS);
  }

}


void init_lightstrip(CRGB* lightstrip, int num_leds) {
  int num_sections = 4;
  int leds_per_section = num_leds / num_sections;
  int currRed = random(0, 256);
  int currGreen = random(0, 256);
  int currBlue = random(0, 256);
  Serial.println("here");
  Serial.print("num_leds: ");
  Serial.println(num_leds);
  for (int currled = 0; currled < num_leds; currled++) {
    lightstrip[currled] = CRGB(currRed, currGreen, currBlue);

    if (currled % leds_per_section == 0) {
      currRed = random(0, 256);
      currGreen = random(0, 256);
      currBlue = random(0, 256);
    }

  }
  FastLED.show();

}

void rotatelightstrip(CRGB* lightstrip, int num_leds, int directionn) {
  if (directionn > 0) {
    // move forward
    CRGB temp = lightstrip[0];
    for (int currled = 0; currled < num_leds - 1; currled++) {
      lightstrip[currled] = CRGB(lightstrip[currled + 1].red, lightstrip[currled + 1].green, lightstrip[currled + 1].blue);
    }
    lightstrip[num_leds - 1] = temp;
  } else {
    // move backward
    CRGB temp = lightstrip[num_leds - 1];
    for (int currled = num_leds; currled > 0; currled--) {
      lightstrip[currled] = CRGB(lightstrip[currled - 1].red, lightstrip[currled - 1].green, lightstrip[currled - 1].blue);
    }
    lightstrip[0] = temp;
  }
  FastLED.show();
}
