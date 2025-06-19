#include <stdio.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define MIN_PWM 0
#define MAX_PWM 4095

// PWM channels of pca9685 0-16
#define WHEEL_CHANNEL1 8
#define WHEEL_CHANNEL2 9
#define WHEEL_CHANNEL3 10
#define WHEEL_CHANNEL4 11

#define MOTOR1_CHANNEL1 12
#define MOTOR1_CHANNEL2 13
#define MOTOR2_CHANNEL1 14
#define MOTOR2_CHANNEL2 15

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setWheelMotors(int c1, int c2, int c3, int c4)
{
  char dbg_str[30];
  sprintf(dbg_str,"C1: %d\tC2: %d\tC3: %d\tC4: %d",c1,c2,c3,c4);
  Serial.println(dbg_str);

  pwm.setPin(WHEEL_CHANNEL1, c1);
  pwm.setPin(WHEEL_CHANNEL2, c2);
  pwm.setPin(WHEEL_CHANNEL3, c3);
  pwm.setPin(WHEEL_CHANNEL4, c4);
}

// set motor channel 12, 13
void setMotor1(int speed){
  if (speed == 0){
    pwm.setPin(MOTOR1_CHANNEL1, 0);
    pwm.setPin(MOTOR1_CHANNEL2, 0);
  }
  else if (speed > 0) pwm.setPin(MOTOR1_CHANNEL1, speed);
  else pwm.setPin(MOTOR1_CHANNEL2, -speed);
}

// set motor channel 14, 15
void setMotor2(int speed){
  if (speed == 0){
    pwm.setPin(MOTOR2_CHANNEL1, 0);
    pwm.setPin(MOTOR2_CHANNEL2, 0);
  }
  else if (speed > 0) pwm.setPin(MOTOR2_CHANNEL1, speed);
  else pwm.setPin(MOTOR2_CHANNEL2, -speed);
}

void setServo(uint8_t channel, uint16_t pulse, bool byMicroseconds = false) {
  if (byMicroseconds){
    pwm.writeMicroseconds(channel, pulse);
  } else {
    pwm.setPWM(channel, 0, pulse);
  }
}

void initMotors()
{
  Wire.begin(); // SDA, SCL, 400000);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);
  Wire.setClock(400000);

  setWheelMotors(0, 0, 0, 0);
  setMotor1(0);
  setMotor2(0);
}
