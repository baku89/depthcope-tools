#include <Wire.h>
#include <Adafruit_MotorShield.h>

#define MOTOR_SPEED 20

#define INT_ROT      0


Adafruit_MotorShield shield = Adafruit_MotorShield();
Adafruit_StepperMotor *motor = shield.getStepper(200, 2);

volatile unsigned int rotCnt = 0;
volatile bool isRotating = false;

unsigned char code = 0;
int value = 0;
int steps = 0;

void setup() {
  Serial.begin(9600);
  
  attachInterrupt(digitalPinToInterrupt(2), onRot, CHANGE);
  
  shield.begin();
  motor->setSpeed(MOTOR_SPEED);
}

void loop() {
  
  while (Serial.available() >= 3) {
    code = Serial.read();
    
    if (code == 'R') {
      value = readInt();
      
      steps = minutesToStepperSteps(value);
      
      isRotating = true;
      rotCnt = 0;
      motor->step(steps, FORWARD, MICROSTEP);
      sendValue('D', steps);
      isRotating = false;
      
    }
  }

}

int minutesToStepperSteps(int minutes) {
  return minutes / 27;
} 

void onRot() {
  if (isRotating) {
    rotCnt++;
    sendValue('H', rotCnt);
  }
}

int readInt() {
  byte low = Serial.read();
  byte high = Serial.read();
  return (high << 8) | low;
}

void sendValue(unsigned char code, int val) {
  Serial.write(code);
  Serial.write(lowByte(val));
  Serial.write(highByte(val));
}
