#define METER_PIN 4
#define PROX_PIN A11

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
/*
  analogWrite(METER_PIN, 255/2);
delay(1500);
  analogWrite(METER_PIN, 255);
delay(1500);
  analogWrite(METER_PIN, 0);
delay(1500);*/

int reading = analogRead(PROX_PIN);

int output = map(reading, 40, 600, 0, 255);
if(output > 255) output = 255;
if(output < 0) output = 0;

  analogWrite(METER_PIN, output);


Serial.print(reading);
Serial.print(" ");
Serial.println(output);


}
