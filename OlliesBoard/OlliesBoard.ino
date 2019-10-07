#include <Arduino.h>
#include <Adafruit_MPR121.h>
#include <ShiftRegister74HC595.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <SparkFun_TB6612.h>
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

#define DEBUG true
#define Serial if(DEBUG)Serial

// debounce time for the buttons
#define DEBOUNCE 200

// random seed (don't plug anything into this...
#define RANDOM_SEED A15

// current time
unsigned long current_program_time = millis();
// previous time
unsigned long previous_program_time = 0;

// LED shift register output
#define LED_NUM_SHIFT_REGS 2
#define LED_DATA_OUT 22
#define LED_CLOCK_OUT 23
#define LED_LATCH_OUT 24
#define SINGLE_LED_OUT 25
ShiftRegister74HC595 ledShiftRegister(LED_NUM_SHIFT_REGS, LED_DATA_OUT, LED_CLOCK_OUT, LED_LATCH_OUT);

// red on/off button
#define RED_BTN_OUT_SR 10
#define RED_BTN_IN 26
int red_btn_curr_state = HIGH;
int red_btn_reading;
int red_btn_prev_state = LOW;
unsigned long red_btn_press_time = 0;

// white toggle switch
#define WHITE_TGL_OUT_SR 13
#define WHITE_TGL_IN 29
int white_tgl_curr_state = HIGH;
int white_tgl_reading;
int white_tgl_prev_state = LOW;
unsigned long white_tgl_press_time = 0;

// blue on/off button
#define BLUE_BTN_OUT_SR 12
#define BLUE_BTN_IN 28
int blue_btn_curr_state = HIGH;
int blue_btn_reading;
int blue_btn_prev_state = LOW;
unsigned long blue_btn_press_time = 0;

// yellow toggle switch
#define YELLOW_TGL_OUT_SR 11
#define YELLOW_TGL_IN 27
int yellow_tgl_curr_state = HIGH;
int yellow_tgl_reading;
int yellow_tgl_prev_state = LOW;
unsigned long yellow_tgl_press_time = 0;


// green on/off button
#define GREEN_BTN_OUT_SR 14
#define GREEN_BTN_IN 30
int green_btn_curr_state = HIGH;
int green_btn_reading;
int green_btn_prev_state = LOW;
unsigned long green_btn_press_time = 0;

// red toggle switch
#define RED_TGL_OUT_SR 15
#define RED_TGL_IN 31
int red_tgl_curr_state = HIGH;
int red_tgl_reading;
int red_tgl_prev_state = LOW;
unsigned long red_tgl_press_time = 0;

// yellow on/off button
#define YELLOW_BTN_OUT 25
#define YELLOW_BTN_IN 32
int yellow_btn_curr_state = HIGH;
int yellow_btn_reading;
int yellow_btn_prev_state = LOW;
unsigned long yellow_btn_press_time = 0;

// phone dialer
#define PHONE_IN 33
#define PHONE_DEBOUNCE 10
#define PHONE_DIAL_LENGTH 100   // how long to wait for a dial to finish in ms
#define PHONE_LIGHT_LEN 2000    // how long the phone lights should stay lit after they light up in ms
int phone_counter = 0;
int phone_last_state = LOW;
int phone_true_state = LOW;
unsigned long phone_state_change_time = 0;
int phone_cleared = 0;
int phone_print_state = 0;
unsigned long phone_lit_at = 0;

// rgb potentiometer setup
#define RED_RGB_POT_IN A0
#define GREEN_RGB_POT_IN A1
#define BLUE_RGB_POT_IN A2

// rgb led outputs
#define RED_RGB_OUT 7
#define GREEN_RGB_OUT 8
#define BLUE_RGB_OUT 9

// prox sensor
#define PROX_IN A3

// voltmeter
#define VM_OUT 4

//rotary encoder
#define ENCODER_A_IN A4
#define ENCODER_B_IN A5
#define ENCODER_BTN_IN A6
int re_last_encoded_value = 0;
int re_encoder_value = 0;
int re_last_encoder_value = 0;

// led light strip
#define LIGHTSTRIP_NUM_LEDS 150
#define LIGHTSTRIP_IN 6
CRGB lightstrip[LIGHTSTRIP_NUM_LEDS];

// address of touch sensor
#define TOUCH_ADDR 0x5A

// touch sensor setup
Adafruit_MPR121 touch_sensor = Adafruit_MPR121();
// Keeps track of the last pins touchedn so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

// dfplayer setup
#define DFPLAYER_RX 10
#define DFPLAYER_TX 11
SoftwareSerial dfplayer_serial(DFPLAYER_RX, DFPLAYER_TX);
DFRobotDFPlayerMini dfplayer;

// motor hat setup
#define MOTOR_AIN1 45
#define MOTOR_AIN2 44
#define MOTOR_PWMA 2
#define MOTOR_BIN1 43
#define MOTOR_BIN2 42
#define MOTOR_PWMB 3
#define MOTOR_STDBY 52
Motor pc_fan = Motor(MOTOR_AIN1, MOTOR_AIN2, MOTOR_PWMA, 1, MOTOR_STDBY);
Motor vibes = Motor(MOTOR_BIN1, MOTOR_BIN2, MOTOR_PWMB, 1, MOTOR_STDBY);

// fan potentiometer setup
#define FAN_POT_IN A7

// FSR setup
#define FSR_IN A8


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(RANDOM_SEED));

  // turn off all leds on shift register to start
  ledShiftRegister.setAllLow();

  // setup red on/off button
  pinMode(RED_BTN_IN, INPUT_PULLUP);
  red_btn_reading = digitalRead(RED_BTN_IN);
  ledShiftRegister.set(RED_BTN_OUT_SR, !red_btn_reading);

  //setup white toggle
  pinMode(WHITE_TGL_IN, INPUT);
  white_tgl_reading = digitalRead(WHITE_TGL_IN);
  ledShiftRegister.set(WHITE_TGL_OUT_SR, white_tgl_reading);

  // setup blue on/off button
  pinMode(BLUE_BTN_IN, INPUT_PULLUP);
  blue_btn_reading = digitalRead(BLUE_BTN_IN);
  ledShiftRegister.set(BLUE_BTN_OUT_SR, !blue_btn_reading);

  //setup yellow toggle
  pinMode(YELLOW_TGL_IN, INPUT);
  yellow_tgl_reading = digitalRead(YELLOW_TGL_IN);
  ledShiftRegister.set(YELLOW_TGL_OUT_SR, yellow_tgl_reading);

  // setup green on/off button
  pinMode(GREEN_BTN_IN, INPUT_PULLUP);
  green_btn_reading = digitalRead(GREEN_BTN_IN);
  ledShiftRegister.set(GREEN_BTN_OUT_SR, !green_btn_reading);

  //setup red toggle
  pinMode(RED_TGL_IN, INPUT);
  red_tgl_reading = digitalRead(RED_TGL_IN);
  ledShiftRegister.set(RED_TGL_OUT_SR, red_tgl_reading);

  // setup yellow on/off button
  pinMode(YELLOW_BTN_IN, INPUT_PULLUP);
  pinMode(YELLOW_BTN_OUT, OUTPUT);
  yellow_btn_reading = digitalRead(YELLOW_BTN_IN);
  digitalWrite(YELLOW_BTN_OUT, !yellow_btn_reading);

  // setup phone dialer input
  pinMode(PHONE_IN, INPUT);
  // turn off all of the phone output register pins
  for (int i = 0; i <= 9; i++) {
    ledShiftRegister.set(i, LOW);
  }

  // setup single LED pin
  pinMode(SINGLE_LED_OUT, OUTPUT);

  //setup RGB pots and output
  pinMode(RED_RGB_POT_IN, INPUT);
  pinMode(GREEN_RGB_POT_IN, INPUT);
  pinMode(BLUE_RGB_POT_IN, INPUT);
  pinMode(RED_RGB_OUT, OUTPUT);
  pinMode(GREEN_RGB_OUT, OUTPUT);
  pinMode(BLUE_RGB_OUT, OUTPUT);

  // setup prox and voltmeter
  pinMode(PROX_IN, INPUT);
  pinMode(VM_OUT, OUTPUT);

  // setup rotary encoder inputs
  pinMode(ENCODER_A_IN, INPUT_PULLUP);
  pinMode(ENCODER_B_IN, INPUT_PULLUP);
  pinMode(ENCODER_BTN_IN, INPUT_PULLUP);
  // get the rotary encoder starting position
  int re_lastMSB = digitalRead(ENCODER_A_IN);
  int re_lastLSB = digitalRead(ENCODER_B_IN);
  re_last_encoded_value = (re_lastMSB << 1) | re_lastLSB;

  // setup lightstrip output
  FastLED.addLeds<WS2812B, LIGHTSTRIP_IN>(lightstrip, LIGHTSTRIP_NUM_LEDS);
  FastLED.clear();
  init_lightstrip(LIGHTSTRIP_NUM_LEDS);

  // touch sensor setup
  touch_sensor.begin(TOUCH_ADDR);

  // dfplayer setup
  dfplayer_serial.begin(9600);
  dfplayer.begin(dfplayer_serial);
  dfplayer.volume(20);

  // fan input setup
  pinMode(FAN_POT_IN, INPUT);

  // fsr setup
  pinMode(FSR_IN, INPUT);
}

void loop() {
  current_program_time = millis();

  process_button_presses();

  process_phone_dial();

  process_rgb_dials();

  process_prox_sensor();

  process_lightstrip();

  process_touch_sensor();

  process_fan();

  process_vibrate();

}

void process_button_presses() {
  // red button
  if ((red_btn_reading != red_btn_prev_state) && (millis() - red_btn_press_time > DEBOUNCE)) {
    ledShiftRegister.set(RED_BTN_OUT_SR, !red_btn_reading);
    red_btn_prev_state = red_btn_reading;
    red_btn_press_time = current_program_time;
  }
  red_btn_reading = digitalRead(RED_BTN_IN);

  // white toggle
  if ((white_tgl_reading != white_tgl_prev_state) && (millis() - white_tgl_press_time > DEBOUNCE)) {
    ledShiftRegister.set(WHITE_TGL_OUT_SR, white_tgl_reading);
    white_tgl_prev_state = white_tgl_reading;
    white_tgl_press_time = current_program_time;
  }
  white_tgl_reading = digitalRead(WHITE_TGL_IN);

  // blue button
  if ((blue_btn_reading != blue_btn_prev_state) && (millis() - blue_btn_press_time > DEBOUNCE)) {
    ledShiftRegister.set(BLUE_BTN_OUT_SR, !blue_btn_reading);
    blue_btn_prev_state = blue_btn_reading;
    blue_btn_press_time = current_program_time;
  }
  blue_btn_reading = digitalRead(BLUE_BTN_IN);

  // yellow toggle
  if ((yellow_tgl_reading != yellow_tgl_prev_state) && (millis() - yellow_tgl_press_time > DEBOUNCE)) {
    ledShiftRegister.set(YELLOW_TGL_OUT_SR, yellow_tgl_reading);
    yellow_tgl_prev_state = yellow_tgl_reading;
    yellow_tgl_press_time = current_program_time;
  }
  yellow_tgl_reading = digitalRead(YELLOW_TGL_IN);

  // green button
  if ((green_btn_reading != green_btn_prev_state) && (millis() - green_btn_press_time > DEBOUNCE)) {
    ledShiftRegister.set(GREEN_BTN_OUT_SR, !green_btn_reading);
    green_btn_prev_state = green_btn_reading;
    green_btn_press_time = current_program_time;
  }
  green_btn_reading = digitalRead(GREEN_BTN_IN);

  // red toggle
  if ((red_tgl_reading != red_tgl_prev_state) && (millis() - red_tgl_press_time > DEBOUNCE)) {
    ledShiftRegister.set(RED_TGL_OUT_SR, red_tgl_reading);
    red_tgl_prev_state = red_tgl_reading;
    red_tgl_press_time = current_program_time;
  }
  red_tgl_reading = digitalRead(RED_TGL_IN);


  // yellow button
  if ((yellow_btn_reading != yellow_btn_prev_state) && (millis() - yellow_btn_press_time > DEBOUNCE)) {
    digitalWrite(YELLOW_BTN_OUT, !yellow_btn_reading);
    yellow_btn_prev_state = yellow_btn_reading;
    yellow_btn_press_time = current_program_time;
  }
  yellow_btn_reading = digitalRead(YELLOW_BTN_IN);

}

void process_phone_dial() {
  int phone_curr_reading = digitalRead(PHONE_IN);
  if ((millis() - phone_state_change_time) > PHONE_DIAL_LENGTH) {
    if (phone_print_state) {
      for (int i = 0; i < phone_counter; i++) {
        ledShiftRegister.set(i, HIGH);
      }
      phone_lit_at = millis();
      phone_print_state = 0;
      phone_counter = 0;
      phone_cleared = 0;
    }
  }

  if (phone_curr_reading != phone_last_state) {
    phone_state_change_time = millis();
  }

  if ((millis() - phone_state_change_time) > PHONE_DEBOUNCE) {
    if (phone_curr_reading  != phone_true_state) {
      phone_true_state = phone_curr_reading;
      if (phone_true_state == HIGH) {
        phone_counter++;
        phone_print_state = 1;
      }
    }
  }
  phone_last_state = phone_curr_reading;

  if ((phone_lit_at != 0) && ((millis() - phone_lit_at) > PHONE_LIGHT_LEN)) {
    for (int i = 0; i <= 9; i++) {
      ledShiftRegister.set(i, LOW);
    }
    phone_lit_at = 0;
  }
  if (phone_lit_at < 0) phone_lit_at = 0;

}

void process_rgb_dials() {
  int red_val = analogRead(RED_RGB_POT_IN);
  int green_val = analogRead(GREEN_RGB_POT_IN);
  int blue_val = analogRead(BLUE_RGB_POT_IN);

  red_val = constrain(map(red_val, 25, 1023, 0, 255), 0, 255);
  green_val = constrain(map(green_val, 25, 1023, 0, 255), 0, 255);
  blue_val = constrain(map(blue_val, 25, 1023, 0, 255), 0, 255);

  analogWrite(RED_RGB_OUT, red_val);
  analogWrite(GREEN_RGB_OUT, green_val);
  analogWrite(BLUE_RGB_OUT, blue_val);
}

void process_prox_sensor() {
  int prox_reading = analogRead(PROX_IN);
  int vm_output = constrain(map(prox_reading, 100, 600, 0, 255), 0, 255);
  analogWrite(VM_OUT, vm_output);
}

void init_lightstrip(int num_leds) {
  int num_sections = 4;
  int leds_per_section = num_leds / num_sections;
  int currRed = random(0, 256);
  int currGreen = random(0, 256);
  int currBlue = random(0, 256);
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

void process_lightstrip() {
  int MSB = digitalRead(ENCODER_A_IN);
  int LSB = digitalRead(ENCODER_B_IN);
  int encoded = (MSB << 1) | LSB;
  int sum = (re_last_encoded_value << 2) | encoded;


  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) re_encoder_value++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) re_encoder_value--;

  re_last_encoded_value = encoded;

  if (re_encoder_value != re_last_encoder_value) {
    if (re_encoder_value > re_last_encoder_value) {
      rotatelightstrip(LIGHTSTRIP_NUM_LEDS, 1);
    } else {
      rotatelightstrip(LIGHTSTRIP_NUM_LEDS, -1);
    }
    re_last_encoder_value = re_encoder_value;
  }

  if (digitalRead(ENCODER_BTN_IN) == LOW) {
    re_encoder_value = 0;
    init_lightstrip(LIGHTSTRIP_NUM_LEDS);
  }
}

void rotatelightstrip(int num_leds, int directionn) {
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
    for (int currled = num_leds - 1; currled > 0; currled--) {
      lightstrip[currled] = CRGB(lightstrip[currled - 1].red, lightstrip[currled - 1].green, lightstrip[currled - 1].blue);
    }
    lightstrip[0] = temp;
  }
  FastLED.show();
}

void process_touch_sensor() {
  currtouched = touch_sensor.touched();
  for (uint8_t i = 0; i < 12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      if (i == 0) {
        dfplayer.play(2);
        // DO SOMETHING ABOUT BEING TOUCHED Serial.println("0 touched!");
      }
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      // DO SOMETHING ABOUT BEING RELEASED Serial.print(i); Serial.println(" released");
    }
  }

  // reset our state
  lasttouched = currtouched;
}

void process_fan() {
  int pot_reading = analogRead(FAN_POT_IN);
  int fan_speed = constrain(map(pot_reading, 40 , 1023, 0, 255), 0, 255);

  if(fan_speed == 0) {
    pc_fan.brake();
  } else {
    pc_fan.drive(fan_speed);
  }
}

void process_vibrate() {
  int fsr_reading = analogRead(FSR_IN);
  int vibrate_speed = constrain(map(fsr_reading, 20, 1000, 0, 255), 0, 255);
  if(vibrate_speed == 0) {
    vibes.brake();
  } else {
    vibes.drive(vibrate_speed);
  }
}
