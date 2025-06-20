/*
 * A better version of MakerBotwPS2 example used with makerbot BANHMI hardware platform
 * This version simplify the joystick driving method, where the functionality stay the same.
 * Added new controling menthod, switch driving mode by pressing SELECT button
 * Added nitro like speed booster when holding L2
 * 
 * Writen by Tu Dang - Makerviet
 */

#include "motors.h"
#include "servo.h"
#include "ps2_controller.h"

void setup()
{
  Serial.begin(115200);
  initMotors();
  initServos();
  setupPS2controller();
  Serial.println("Done setup!");
}

void loop()
{
  ps2x.read_gamepad(0, 0);
  bool ps2 = PS2control();
  unsigned long lastInputTime = millis();
  if ((ps2) && ((unsigned long) (millis() - lastInputTime) > 21))
    setWheelMotors(0, 0, 0, 0);
    delay(30);
  delay(50);
}