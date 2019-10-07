#include <SparkFun_TB6612.h>

#define AIN1 45
#define AIN2 44
#define PWMA 2
#define BIN1 43
#define BIN2 44
#define PWMB 3

#define STDBY 52

#define FSR_PIN A8

#define POT_PIN A15

Motor motorA = Motor(AIN1, AIN2, PWMA, 1, STDBY);
Motor motorB = Motor(BIN1, BIN2, PWMB, 1, STDBY);
void setup() {
  // put your setup code here, to run once:

  pinMode(POT_PIN, INPUT);
  pinMode(FSR_PIN, INPUT);

  randomSeed(analogRead(0));

  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:


  int potSense = analogRead(POT_PIN)/4;

  int fsrReading = analogRead(FSR_PIN);  
 
  Serial.print("Analog reading = ");
  Serial.print(fsrReading);     
  Serial.println("");


  //motorA.drive(potSense);

  //107 is ~5v if input is ~12v
  //44 is ~2v if input is ~12v
  //motorB.drive(107);
  //motorB.drive(44);

  if(fsrReading<100) {
    motorB.brake();
  } else {
    int motorBSpeed = map(fsrReading, 0, 1023, 44, 107);
    motorB.drive(motorBSpeed);
  }

  //int amt = random(44, 107);
  //motorB.drive(amt, 1000);
  //motorB.brake();
  //delay(1000);
  


}
