#include "Arduino.h"
#include "spi.h"
#include "cmd_handling.h"
#include "state_handling.h"
#include "stepper.h"

//The setup function is called once at startup of the sketch
void setup() {

  Serial.begin(115200);

  setupStepper();
  setupSPI();
  setupCmdHandling();
  setupStateHandling();
}

// The loop function is called in an endless loop
void loop() {

//delay(1000);
  performStep();
}
