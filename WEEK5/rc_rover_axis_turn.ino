// rc_rover_axis_turn.ino // Differential drive rover with zero-radius
turning // Works with Arduino Uno / ESP32 + L298N

// ── Motor A (Left) ── #define ENA 5 #define IN1 6 #define IN2 7

// ── Motor B (Right) ── #define ENB 9 #define IN3 10 #define IN4 11

int speedVal = 180; // Speed range: 0–255

void setup() { Serial.begin(9600);

pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);

pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

stopRover(); }

void loop() { if (Serial.available()) { char cmd = Serial.read();

    switch (cmd) {
      case 'F': moveForward(); break;
      case 'B': moveBackward(); break;
      case 'L': spinLeft(); break;
      case 'R': spinRight(); break;
      case 'S': stopRover(); break;
    }

} }

void moveForward() { analogWrite(ENA, speedVal); analogWrite(ENB,
speedVal);

digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);

digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); }

void moveBackward() { analogWrite(ENA, speedVal); analogWrite(ENB,
speedVal);

digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);

digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); }

void spinLeft() { analogWrite(ENA, speedVal); analogWrite(ENB,
speedVal);

digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);

digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); }

void spinRight() { analogWrite(ENA, speedVal); analogWrite(ENB,
speedVal);

digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);

digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); }

void stopRover() { analogWrite(ENA, 0); analogWrite(ENB, 0);

digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW);
digitalWrite(IN4, LOW); }
