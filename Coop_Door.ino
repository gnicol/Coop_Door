#define PIN_GO_UP       10
#define PIN_GO_DOWN     11
#define PIN_SENSE_UP    12
#define PIN_SENSE_DOWN  13

#define MAX_RUN         4500UL  // max runtime of 4.5 seconds

unsigned long timeout;

void setup()  {
  // open serial port, sets data rate to 9600 bps
  Serial.begin(9600);     
  
  // break motor
  digitalWrite(PIN_GO_UP,      LOW);
  digitalWrite(PIN_GO_DOWN,    LOW);
  
  // set sense pins as inputs and enable pull upss
  pinMode(PIN_SENSE_UP,        INPUT);
  pinMode(PIN_SENSE_DOWN,      INPUT);
  digitalWrite(PIN_SENSE_UP,   HIGH);
  digitalWrite(PIN_SENSE_DOWN, HIGH);
}

void loop() {
  if (Serial.available() > 0) {
    switch (Serial.read()) {
      case 'd':
        Serial.write("Going Down!\n");
        doorMove(PIN_SENSE_DOWN, PIN_GO_DOWN);
        break;
      case 'D':
        Serial.write("Going Up!\n");
        doorMove(PIN_SENSE_UP, PIN_GO_UP);
        break;
    }
  }
}

void doorMove(int sensePin, int controlPin) {
  // kill power
  digitalWrite(PIN_GO_UP,   LOW);
  digitalWrite(PIN_GO_DOWN, LOW);  
  
  // if we're already there, bail!
  if (digitalRead(sensePin) == LOW) {
    Serial.write("Already There!\n");
    return;
  }

  digitalWrite(controlPin, HIGH);
  
  // wait for hall to trip or timeout to expire
  // if millis rolls timeout won't work correctly; should be rare
  // also if you only let it run partway up/down and then switch 
  // directions, timeout should shorten but doesn't... don't do that :)
  timeout = millis() + MAX_RUN;
  while (digitalRead(sensePin) != LOW && timeout > millis());
 
  // kill power
  digitalWrite(controlPin, LOW);
  
  // if we sense we're down all is well; otherwise assume timeout
  // in reality a bouncy sense pin could trip this test erroneously.
  if (digitalRead(sensePin) == LOW) {
    Serial.write("Moved There!\n");
  } else {
    Serial.write("Timed out :(\n");
  }
}
