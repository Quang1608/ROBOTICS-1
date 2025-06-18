#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_1_CHANNEL 2
#define SERVO_2_CHANNEL 3
#define SERVO_3_CHANNEL 4
#define SERVO_4_CHANNEL 5
#define SERVO_5_CHANNEL 6
#define SERVO_6_CHANNEL 7

#define NOTIFY_LED 13

void setServo(uint8_t channel, uint16_t pulse) {
  pwm.setPWM(channel, 0, pulse);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Servo test firmware for MakerBot 2024");

  pwm.begin();
  pwm.setPWMFreq(50); 

  pinMode(NOTIFY_LED, OUTPUT);
}

void loop() {

  // Blink the LED to notify the test is started
  for (int i = 0; i < 3; i++) {
    Serial.println(3 - i);
    digitalWrite(NOTIFY_LED, HIGH);
    delay(500);
    digitalWrite(NOTIFY_LED, LOW);
    delay(500);
  }

    setServo(i, 0); 

    // Sweep from 0 to 4000 in steps of 100
    for (int j = 0; j <= 4000; j+= 100) { 
      Serial.print("Setting servo ");
      Serial.print(i);
      Serial.print(" running at ");
      Serial.println(j);
      setServo(i, j);
      delay(300); // Delay 300ms between each step
    }

    // Set servo to 0 after test
    setServo(i, 0); 

    Serial.println("Change to next servo...");
    
    // Blink the LED to notify the test is started
    digitalWrite(NOTIFY_LED, HIGH);
    delay(500);
    digitalWrite(NOTIFY_LED, LOW);
  }

  delay(5000);
}