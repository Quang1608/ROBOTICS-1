#include <PS2X_lib.h>

PS2X ps2x;

#define BEBUG_CTRL

// calibration for different kinds of PS2 controller, this value only suitable for the PS2 controller comes with VRC2023 K12 Maker kit 
#define X_JOY_CALIB 127 
#define Y_JOY_CALIB 128

#define PS2_DAT 12 // MISO  19
#define PS2_CMD 13 // MOSI  23
#define PS2_SEL 15 // SS     5
#define PS2_CLK 14 // SLK   18

// define control buttons
#define BOOST_SPEED_1 PSB_L3 // hold
#define BOOST_SPEED_2 PSB_R3 // hold
#define SWITCH_DRIVING_MODE PSB_SELECT // press
#define SERVO_CLOCKWISE PSB_L2 // hold
#define SERVO_COUNTERCLOCKWISE PSB_L1 // hold
#define RAISING PSB_GREEN // hold
#define HANGING PSB_PINK // hold
#define FORCE_STOP PSB_RED // press

// define motors and servos speed
#define RAISING_SPEED 500
#define HANGING_SPEED 500
#define SERVO_SPEED 100

#define TOP_SPEED 4095
#define NORM_SPEED 2048

#define SERVO_1_CHANNEL 2

#define SINGLE_HAND_DRIVING 0
#define TWO_HAND_DRIVING 1
bool driving_mode = SINGLE_HAND_DRIVING;
unsigned long lastSwitchDrivingMode = 0;
int tc1, tc2, tc3, tc4;

unsigned long lastForceStop = 0;

void setupPS2controller()
{
  int err = -1;
  while (err != 0)
  {
    err = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, true, true);
  }

}
bool PS2control()
{
  /*
  Force stop function
  If anything unexpected happens while controlling robot, we can press this button to stop all motors and servos
  */

  if (ps2x.ButtonPressed(FORCE_STOP) && (millis() - lastForceStop > 300)){
    lastForceStop = millis();
    setWheelMotors();
    setMotor1();
    setMotor2();
    setServoUS(SERVO_1_CHANNEL, 1500);
    return 1;
  }

  /*
  * Driving functions
  */
  int speed = NORM_SPEED;
  if ((ps2x.Button(BOOST_SPEED_1)) || (ps2x.Button(BOOST_SPEED_2)))
    speed = TOP_SPEED;

  if (ps2x.ButtonPressed(SWITCH_DRIVING_MODE) && (millis() - lastSwitchDrivingMode) > 300){
    lastSwitchDrivingMode = millis();
    driving_mode =! driving_mode;
  }
  
  /*
  2 360 servo use L/R1 and L/R2.
  2 180 servo use A/B/X/Y: ButtonPressed() -> directly move to 180 or 90 degree.

  360 servo uses pulse length to move clockwise and counterclockwise.
  180 servo uses pulse length to move to a specific angle.
  setPWM() and writeMicroseconds() are indifferent.

  2 360 servos use channel 2 and 3, for more info refer to servo.h 
  */

  if (ps2x.Button(SERVO_COUNTERCLOCKWISE)) // First 360 servo moves counterclockwise
    setServoUS(SERVO_1_CHANNEL, 1500 - SERVO_SPEED);
  else if (ps2x.Button(SERVO_CLOCKWISE)) // First 360 servo moves clockwise
    setServoUS(SERVO_1_CHANNEL, 1500 + SERVO_SPEED);
  else // servo stop
    setServoUS(SERVO_1_CHANNEL, 1500);
    
  // if (ps2x.Button(PSB_R1)) // Second 360 servo moves counterclockwise
  //   setServoUS(SERVO_1_CHANNEL, 1000);

  // if (ps2x.Button(PSB_R2)) // Second 360 servo moves clockwise
  //   setServoUS(SERVO_1_CHANNEL, 2000);

  /*
  Raising mechanism

  The DC Motor should be wired to channel 5 and 6
  The code below "should" only raise the box, swap to channel 6 if not moving correctly
  */
  if (ps2x.Button(RAISING)) {
    setMotor1(RAISING_SPEED);
    setMotor2(RAISING_SPEED);
  } 
  else if (ps2x.Button(HANGING)){
    setMotor1(-RAISING_SPEED);
    setMotor2(-RAISING_SPEED);
  }
  else {
    setMotor1(0);
    setMotor2(0);
  }
  /*
  Hanging Mechanism

  The geared DC Motor here should be wired to channel 7 and 8
  The code below should lower the box and pulling the bar down, swap to channel 8 if the gear is not moving correctly
  */

  /*
  Robot Movement 
  */
  int nJoyX = X_JOY_CALIB - ps2x.Analog(PSS_RX); // read x-joystick
  int nJoyY = Y_JOY_CALIB - (driving_mode ? ps2x.Analog(PSS_LY) : ps2x.Analog(PSS_RY)); // read y-joystick
  // nJoyX = map(nJoyX, 0, 128, 0, 255);
  // nJoyY = map(nJoyY, 0, 128, 0, 255);
  // Serial.println(nJoyX);
  // Serial.println(nJoyY);
  int nMotMixL;                          // Motor (left) mixed output
  int nMotMixR;                          // Motor (right) mixed output

  if(nJoyX == -1 && nJoyY == 0) // in case of lost connection with the wireless controller, only used in VRC2023 PS2 wireless controller 
  {
    setWheelMotors();
    return 0;
  }

  bool temp = (nJoyY * nJoyX > 0);
  if (nJoyX) // Turning
  {
    nMotMixL = -nJoyX + (nJoyY * temp);
    nMotMixR = nJoyX + (nJoyY * !temp);
  }
  else // Forward or Reverse
  {
    nMotMixL = nJoyY;
    nMotMixR = nJoyY;
  }
  #ifdef BEBUG_CTRL
  Serial.print(F("Calculated value from joystick: "));
  // Serial.print(nMotMixL);
  Serial.print("\t");
  // Serial.println(nMotMixR);
  #endif
  
  int c1 = 0, c2 = 0, c3 = 0, c4 = 0;

  if (nMotMixR > 0)
  {
    c3 = nMotMixR;
    c3 = map(c3, 0, 128, 0, speed);
  }

  else if (nMotMixR < 0)
  {
    c4 = abs(nMotMixR) + 1;
    c4 = map(c4, 0, 128, 0, speed);
  }

  if (nMotMixL > 0)
  {
    c1 = nMotMixL;
    c1 = map(c1, 0, 128, 0, speed);
  }
  else if (nMotMixL < 0)
  {
    c2 = abs(nMotMixL);
    c2 = map(c2, 0, 128, 0, speed);
  }

  c1 = constrain(c1, 0, speed);
  c2 = constrain(c2, 0, speed);
  c3 = constrain(c3, 0, speed);
  c4 = constrain(c4, 0, speed);
  // setWheelMotors(c1, c2, c3, c4);
  if ((tc1 != c1) || (tc2 != c2) || (tc3 != c3) || (tc4 != c4)) {
    setWheelMotors(0, 0, 0, 0);
    tc1 = c1, tc2 = c2, tc3 = c3, tc4 = c4;
  } else {
    setWheelMotors(c1, c2, c3, c4);
  }
  return 1;
}
