#include "Arduino.h"
#include "spi.h"
#include "cmd_processing.h"
#include "state_handling.h"
#include "stepper.h"

#ifdef __cplusplus
extern "C" {
#endif

//The setup function is called once at startup of the sketch
void setup() {

  Serial.begin(115200);

  setupStepper();
  setupSPI();
  setupCommandProcessing();
  setupStateHandling();
}

// The loop function is called in an endless loop
void loop() {

//delay(1000);
  performStep();
}

#ifdef __cplusplus
}
#endif
