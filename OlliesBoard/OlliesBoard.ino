#include <Arduino.h>
#include "ShiftRegister74HC595.h"

#define DEBUG true
#define Serial if(DEBUG)Serial

// debounce time for the buttons
#define DEBOUNCE 200

// current time
long current_program_time = millis();

// LED shift register output
#define LED_NUM_SHIFT_REGS 2
#define LED_DATA_OUT 22
#define LED_CLOCK_OUT 23
#define LED_LATCH_OUT 24
#define SINGLE_LED_OUT 25
ShiftRegister74HC595 ledShiftRegister(2, LED_DATA_OUT, LED_CLOCK_OUT, LED_LATCH_OUT);

// red on/off button
#define RED_BTN_OUT_SR 10
#define RED_BTN_IN 26
int red_btn_curr_state = HIGH;
int red_btn_reading;
int red_btn_prev_state = LOW;
long red_btn_press_time = 0;

// white toggle switch
#define WHITE_TGL_OUT_SR 11
#define WHITE_TGL_IN 27
int white_tgl_curr_state = HIGH;
int white_tgl_reading;
int white_tgl_prev_state = LOW;
long white_tgl_press_time = 0;

// blue on/off button
#define BLUE_BTN_OUT_SR 12
#define BLUE_BTN_IN 28
int blue_btn_curr_state = HIGH;
int blue_btn_reading;
int blue_btn_prev_state = LOW;
long blue_btn_press_time = 0;

// yellow toggle switch
#define YELLOW_TGL_OUT_SR 13
#define YELLOW_TGL_IN 29
int yellow_tgl_curr_state = HIGH;
int yellow_tgl_reading;
int yellow_tgl_prev_state = LOW;
long yellow_tgl_press_time = 0;


// green on/off button
#define GREEN_BTN_OUT_SR 14
#define GREEN_BTN_IN 30
int green_btn_curr_state = HIGH;
int green_btn_reading;
int green_btn_prev_state = LOW;
long green_btn_press_time = 0;

// red toggle switch
#define RED_TGL_OUT_SR 15
#define RED_TGL_IN 31
int red_tgl_curr_state = HIGH;
int red_tgl_reading;
int red_tgl_prev_state = LOW;
long red_tgl_press_time = 0;

// yellow on/off button
#define YELLOW_BTN_OUT 25
#define YELLOW_BTN_IN 32
int yellow_btn_curr_state = HIGH;
int yellow_btn_reading;
int yellow_btn_prev_state = LOW;
long yellow_btn_press_time = 0;

// phone dialer
#define PHONE_IN 33
#define PHONE_DEBOUNCE 10
#define PHONE_DIAL_LENGTH 100   // how long to wait for a dial to finish in ms
#define PHONE_LIGHT_LEN 2000    // how long the phone lights should stay lit after they light up in ms
int phone_counter = 0;
int phone_last_state = LOW;
int phone_true_state = LOW;
long phone_state_change_time = 0;
int phone_cleared = 0;
int phone_print_state = 0;
long phone_lit_at = 0;

// rgb potentiometer setup
#define RED_RGB_POT_IN A0
#define GREEN_RGB_POT_IN A1
#define BLUE_RGB_POT_IN A2

// rgb led outputs
#define RED_RGB_OUT 7
#define GREEN_RGB_OUT 8
#define BLUE_RGB_OUT 9

//prox sensor
#define PROX_IN A3

//voltmeter 
#define VM_OUT 4


void setup() {
  Serial.begin(9600);

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
  for(int i=0; i<=9; i++) {
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

}

void loop() {
  current_program_time = millis();

  process_button_presses();

  process_phone_dial();

  process_rgb_dials();

  process_prox_sensor();

}

void process_button_presses() {
  // red button
  if((red_btn_reading != red_btn_prev_state) && (millis() - red_btn_press_time > DEBOUNCE)) {
    ledShiftRegister.set(RED_BTN_OUT_SR, !red_btn_reading);
    red_btn_prev_state = red_btn_reading;
    red_btn_press_time = current_program_time;
  }
  red_btn_reading = digitalRead(RED_BTN_IN);

  // white toggle
  if((white_tgl_reading != white_tgl_prev_state) && (millis() - white_tgl_press_time > DEBOUNCE)) {
    ledShiftRegister.set(WHITE_TGL_OUT_SR, white_tgl_reading);
    white_tgl_prev_state = white_tgl_reading;
    white_tgl_press_time = current_program_time;
  }
  white_tgl_reading = digitalRead(WHITE_TGL_IN);

  // blue button
  if((blue_btn_reading != blue_btn_prev_state) && (millis() - blue_btn_press_time > DEBOUNCE)) {
    ledShiftRegister.set(BLUE_BTN_OUT_SR, !blue_btn_reading);
    blue_btn_prev_state = blue_btn_reading;
    blue_btn_press_time = current_program_time;
  }
  blue_btn_reading = digitalRead(BLUE_BTN_IN);

// yellow toggle
  if((yellow_tgl_reading != yellow_tgl_prev_state) && (millis() - yellow_tgl_press_time > DEBOUNCE)) {
    ledShiftRegister.set(YELLOW_TGL_OUT_SR, yellow_tgl_reading);
    yellow_tgl_prev_state = yellow_tgl_reading;
    yellow_tgl_press_time = current_program_time;
  }
  yellow_tgl_reading = digitalRead(YELLOW_TGL_IN);

  // green button
  if((green_btn_reading != green_btn_prev_state) && (millis() - green_btn_press_time > DEBOUNCE)) {
    ledShiftRegister.set(GREEN_BTN_OUT_SR, !green_btn_reading);
    green_btn_prev_state = green_btn_reading;
    green_btn_press_time = current_program_time;
  }
  green_btn_reading = digitalRead(GREEN_BTN_IN);

// red toggle
  if((red_tgl_reading != red_tgl_prev_state) && (millis() - red_tgl_press_time > DEBOUNCE)) {
    ledShiftRegister.set(RED_TGL_OUT_SR, red_tgl_reading);
    red_tgl_prev_state = red_tgl_reading;
    red_tgl_press_time = current_program_time;
  }
  red_tgl_reading = digitalRead(RED_TGL_IN);


 // yellow button
  if((yellow_btn_reading != yellow_btn_prev_state) && (millis() - yellow_btn_press_time > DEBOUNCE)) {
    digitalWrite(YELLOW_BTN_OUT, !yellow_btn_reading);
    yellow_btn_prev_state = yellow_btn_reading;
    yellow_btn_press_time = current_program_time;
  }
  yellow_btn_reading = digitalRead(YELLOW_BTN_IN);
  
}

void process_phone_dial() {
  int phone_curr_reading = digitalRead(PHONE_IN);
  if((millis() - phone_state_change_time) > PHONE_DIAL_LENGTH) {
    if(phone_print_state) {
      for(int i=0; i<phone_counter; i++) {
        ledShiftRegister.set(i, HIGH);
      }
      phone_lit_at = millis();
      phone_print_state = 0;
      phone_counter = 0;
      phone_cleared = 0;
    }
  }

  if(phone_curr_reading != phone_last_state) {
    phone_state_change_time = millis();
  }

  if((millis() - phone_state_change_time) > PHONE_DEBOUNCE) {
      if(phone_curr_reading  != phone_true_state) {
        phone_true_state = phone_curr_reading;
        if(phone_true_state == HIGH) {
          phone_counter++;
          phone_print_state = 1;
        }
      }
    }
    phone_last_state = phone_curr_reading;

  if((phone_lit_at != 0) && ((millis() - phone_lit_at) > PHONE_LIGHT_LEN)) {
    for(int i=0; i<=9; i++) {
      ledShiftRegister.set(i, LOW);
    }
    phone_lit_at = 0;
  }
  if(phone_lit_at < 0) phone_lit_at = 0;
  
}

void process_rgb_dials() {
  int red_val = analogRead(RED_RGB_POT_IN);
  int green_val = analogRead(GREEN_RGB_POT_IN);
  int blue_val = analogRead(BLUE_RGB_POT_IN);

  red_val = constrain(map(red_val, 0, 1023, 0, 255), 0, 255);
  green_val = constrain(map(green_val, 0, 1023, 0, 255), 0, 255);
  blue_val = constrain(map(blue_val, 0, 1023, 0, 255), 0, 255);

  analogWrite(RED_RGB_OUT, red_val);
  analogWrite(GREEN_RGB_OUT, green_val);
  analogWrite(BLUE_RGB_OUT, blue_val);
}

void process_prox_sensor() {
  int prox_reading = analogRead(PROX_IN);
  int vm_output = constrain(map(prox_reading, 100, 600, 0, 255), 0, 255);
  analogWrite(VM_OUT, vm_output);
}
