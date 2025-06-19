#include <PS2X_lib.h>

PS2X ps2x; // create PS2 Controller Class object
#define BEBUG_CTRL

// calibration for different kinds of PS2 controller, this value only suitable for the PS2 controller comes with VRC2023 K12 Maker kit 
#define X_JOY_CALIB 127 
#define Y_JOY_CALIB 128

#define PS2_DAT 12 // MISO  19
#define PS2_CMD 13 // MOSI  23
#define PS2_SEL 15 // SS     5
#define PS2_CLK 14 // SLK   18

// define function to ps2 buttons
#define FORCE_STOP PSB_RED
#define BOOST_SPEED_WHEEL PSB_PINK
#define BOOST_SPEED_MOTOR PSB_GREEN
#define CHANGE_DRIVING_MODE PSB_SELECT
#define UP_MOTOR_1 PSB_PAD_UP
#define DOWN_MOTOR_1 PSB_PAD_DOWN
//#define CONTROL_SERVO PSB_PINK
#define UP_SERVO PSB_PAD_RIGHT
#define DOWN_SERVO PSB_PAD_LEFT

#define TOP_SPEED 4095
#define HIGH_SPEED 3072
#define NORM_SPEED 2048
//#define TURNING_FACTOR 1

#define SINGLE_HAND_DRIVING 0
#define TWO_HAND_DRIVING 1
bool driving_mode = SINGLE_HAND_DRIVING;
unsigned long lastModeToggle = 0;

int servo_mode = 0;

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
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Force Stop
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (ps2x.ButtonPressed(FORCE_STOP)){
    setWheelMotors(0, 0, 0, 0);
    setMotor1(0);
    setMotor2(0);
    Serial.println("All motors force stopped");
    return 1;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Wheel Motors
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  int speed = NORM_SPEED;
  if (ps2x.Button(BOOST_SPEED_WHEEL))
    speed = TOP_SPEED;

  if (ps2x.ButtonPressed(CHANGE_DRIVING_MODE) && millis() - lastModeToggle > 300) {
    driving_mode = !driving_mode;
    lastModeToggle = millis();
  }
  
  int nJoyX = X_JOY_CALIB - ps2x.Analog(PSS_RX); // read x-joystick;
  int nJoyY = Y_JOY_CALIB - (driving_mode ? ps2x.Analog(PSS_LY) :ps2x.Analog(PSS_RY)); // read y-joystick
  int nMotMixL;                          // Motor (left) mixed output
  int nMotMixR;                          // Motor (right) mixed output
  // nJoyX = map(nJoyX, 0, 128, 0, 255);
  // nJoyY = map(nJoyY, 0, 128, 0, 255);
  // Serial.println(nJoyY);
  // Serial.println(nJoyX)

  if(nJoyX == -1 && nJoyY == 0) // in case of lost connection with the wireless controller, only used in VRC2023 PS2 wireless controller 
  {
    setWheelMotors(0, 0, 0, 0);
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
  setWheelMotors(c1, c2, c3, c4);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Additional Motors
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (ps2x.Button(UP_MOTOR1)) setMotor1(ps2x.Button(BOOST_SPEED_MOTOR) ? HIGH_SPEED : NORM_SPEED);
  else if (ps2x.Button(DOWN_MOTOR1)) setMotor1(ps2x.Button(BOOST_SPEED_MOTOR) ? HIGH_SPEED : NORM_SPEED);
  else setMotor1(0);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Servo control
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // SERVO 360 controls
  if (ps2x.Button(UP_SERVO)) setServo(2, 2000, true);
  else if (ps2x.Button(DOWN_SERVO)) setServo(2, 1000, true);
  else setServo(2, 1500, true);

  // SERVO 180 controls
  // if (ps2x.ButtonPressed(CONTROL_SERVO)){
  //   servo_mode = (servo_mode + 1) % 5;
  //   if (servo_mode == 0){
  //     // set servo to 0 degree
  //   }
  //   else if (servo_mode == 1){
  //     // set servo to 45 degree
  //   } 
  //   else if (servo_mode == 2){
  //     // set servo to 90 degree
  //   }
  //   else if (servo_mode == 3){
  //     // set servo to 135 degree
  //   }
  //   else if (servo_mode == 4){
  //     // set servo to 180 degree
  //   }
  //}

  return 1;
}
