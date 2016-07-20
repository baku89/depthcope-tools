#include <Wire.h>
#include <Adafruit_MotorShield.h>

#define MOTOR_SPEED 50
#define PRECISION 1000.0

Adafruit_MotorShield shield = Adafruit_MotorShield();
Adafruit_StepperMotor *motor = shield.getStepper(200, 2);


void setup() {
  Serial.begin(9600);
  Serial.println("Stepper Test!");
  
  shield.begin();
  motor->setSpeed(MOTOR_SPEED);
}

void loop() {
  
  motor->step(100, FORWARD, MICROSTEP);
  motor->step(100, BACKWARD, MICROSTEP);
}
