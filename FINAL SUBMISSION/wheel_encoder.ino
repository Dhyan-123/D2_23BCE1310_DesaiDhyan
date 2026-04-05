#include <Wire.h>
#include <MPU6050.h>

MPU6050 imu;

// ---------------- MOTOR PINS ----------------
#define ENA 14
#define IN1 27
#define IN2 26
#define ENB 25
#define IN3 33
#define IN4 32

// ---------------- ENCODER ----------------
#define ENCODER_PIN 34
volatile long encoderCount = 0;

// ---------------- CONSTANTS ----------------
#define WHEEL_CIRCUMFERENCE 20.0   // cm (adjust)
#define PULSES_PER_REV 20
#define DIST_PER_PULSE (WHEEL_CIRCUMFERENCE / PULSES_PER_REV)

// ---------------- VARIABLES ----------------
float currentAngle = 0;
unsigned long prevTime = 0;

// ---------------- USER INPUT ----------------
float fieldLength = 200;   // cm
float fieldBreadth = 100;  // cm
float rowSpacing = 20;     // cm

// ---------------- INTERRUPT ----------------
void IRAM_ATTR encoderISR() {
  encoderCount++;
}

// ---------------- MOTOR CONTROL ----------------
void moveForward(int speed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward(int speed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ---------------- MOVE DISTANCE ----------------
void moveDistance(float distance_cm, bool forward = true) {
  encoderCount = 0;

  long targetPulses = distance_cm / DIST_PER_PULSE;

  if (forward)
    moveForward(180);
  else
    moveBackward(180);

  while (encoderCount < targetPulses) {
    // wait
  }

  stopMotors();
}

// ---------------- IMU ----------------
void setupIMU() {
  Wire.begin();
  imu.initialize();

  if (!imu.testConnection()) {
    Serial.println("IMU not connected!");
    while (1);
  }
}

float getYaw() {
  int16_t gx, gy, gz;
  imu.getRotation(&gx, &gy, &gz);

  unsigned long currentTime = millis();
  float dt = (currentTime - prevTime) / 1000.0;
  prevTime = currentTime;

  float gyroZ = gz / 131.0;
  currentAngle += gyroZ * dt;

  return currentAngle;
}

// ---------------- ROTATION ----------------
void rotateAngle(float targetAngle, bool clockwise = true) {
  currentAngle = 0;
  prevTime = millis();

  analogWrite(ENA, 150);
  analogWrite(ENB, 150);

  if (clockwise) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }

  while (abs(getYaw()) < targetAngle) {
    // wait
  }

  stopMotors();
}

// ---------------- SERPENTINE LOGIC ----------------
void serpentineTraversal() {

  int numRows = fieldBreadth / rowSpacing;

  for (int i = 0; i < numRows; i++) {

    // Move along length
    if (i % 2 == 0) {
      moveDistance(fieldLength, true);   // forward
    } else {
      moveDistance(fieldLength, false);  // backward
    }

    // If last row → stop
    if (i == numRows - 1) break;

    // Turn + shift to next row
    if (i % 2 == 0) {
      rotateAngle(90, true);
      moveDistance(rowSpacing, true);
      rotateAngle(90, true);
    } else {
      rotateAngle(90, false);
      moveDistance(rowSpacing, true);
      rotateAngle(90, false);
    }
  }

  stopMotors();
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(ENCODER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), encoderISR, RISING);

  setupIMU();
}

// ---------------- LOOP ----------------
void loop() {

  serpentineTraversal();

  while (1); // stop after one full traversal
}