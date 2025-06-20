#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include "hardware.h"

//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_1_CHANNEL 2
#define SERVO_2_CHANNEL 3
#define SERVO_3_CHANNEL 4
#define SERVO_4_CHANNEL 5
#define SERVO_5_CHANNEL 6
#define SERVO_6_CHANNEL 7

// Set Servo directly on PWM
void setServoPWM(uint8_t channel, uint16_t pulse) {
  pwm.setPWM(channel, 0, pulse);
}

// Set servo directly on us
void setServoUS(uint8_t channel, uint16_t microseconds) {
	pwm.writeMicroseconds(channel, microseconds); // writeMicroseconds function only has 2 arguments
}

// Set servo indirectly on angle
void setServoAngle(uint8_t channel, int angle) {
  // Constrain angle to 0-180
  angle = constrain(angle, 0, 180);

  // Map angle to pulse width in s (adjust min/max as needed)
  int us = map(angle, 0, 180, 1000, 2000);

  // Convert to PCA9685 ticks
  int tick = us * 4096 / 20000;

  pwm.setPWM(channel, 0, tick);
}

void initServos() {

  pwm.begin();
  pwm.setPWMFreq(50);

}