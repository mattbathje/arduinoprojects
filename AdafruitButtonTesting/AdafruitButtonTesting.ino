int inPin = 30;
// for metal buttons, NO1 connected to pin 23
// for black buttons, either pin connected to pin 23
// for toggles, connect + to pin 23
int outPin = 23;

int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

void setup()
{
  Serial.begin(9600);
  pinMode(inPin, INPUT_PULLUP);
  pinMode(outPin, OUTPUT);

  reading = digitalRead(inPin);
  digitalWrite(outPin, !reading);
}

void loop()
{
 
  Serial.print(reading);
  Serial.print("  ");
  Serial.println(previous);

  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  /*if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH)
      state = LOW;
    else
      state = HIGH;

    time = millis();    
  }*/

  if((reading != previous) && (millis() - time > debounce)) {
    Serial.println("here");
    
    digitalWrite(outPin, !reading);
    previous = reading;
  }

  //digitalWrite(outPin, state);
  reading = digitalRead(inPin);
  //previous = reading;
}
