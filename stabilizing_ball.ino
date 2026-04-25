/*
 * Self-Balancing Ball on Ramp (Single Servo + Rope Design)
 * 
 * How it works:
 * - Ultrasonic sensor measures distance to ball
 * - If ball is too close -> Servo pulls rope (lowers platform) -> ball rolls back
 * - If ball is too far -> Servo releases rope (raises platform) -> ball rolls forward
 * - PID controller keeps ball at center position
 * 
 * Connections:
 * - HC-SR04 VCC  -> ESP32 5V/VIN
 * - HC-SR04 GND  -> ESP32 GND
 * - HC-SR04 TRIG -> GPIO 5
 * - HC-SR04 ECHO -> GPIO 18 (via 3x 10kΩ voltage divider)
 * - Servo Red    -> External 5V power supply (NOT ESP32!)
 * - Servo Brown  -> GND (common with ESP32)
 * - Servo Orange -> GPIO 13
 */

#include <ESP32Servo.h>

// ========== PIN DEFINITIONS ==========
#define TRIG_PIN 5
#define ECHO_PIN 18
#define SERVO_PIN 13

// ========== BALANCE SETTINGS ==========
float setpoint = 11.5;      // Target distance (cm) - center of 23cm holder
float tolerance = 1.5;      // Dead zone (± cm) - servo won't respond inside this range

// ========== PID GAINS (Tune these for your setup) ==========
float Kp = 1.5;     // Proportional gain - responds to current error
float Ki = 0.03;    // Integral gain - corrects accumulated error
float Kd = 0.4;     // Derivative gain - dampens oscillations

// ========== PID VARIABLES ==========
float error = 0;           // Current error (setpoint - current distance)
float lastError = 0;       // Previous error for derivative calculation
float integral = 0;        // Accumulated error for integral term
float derivative = 0;      // Rate of change of error
float output = 0;          // PID output value

// ========== TIME MANAGEMENT ==========
unsigned long lastTime = 0;
float dt = 0.025;          // 25ms loop time (40Hz update rate)

// ========== SERVO SETTINGS ==========
int servoAngle = 90;       // Current servo angle (90 = center)
int minAngle = 45;         // Minimum angle (fully released rope = platform up)
int maxAngle = 135;        // Maximum angle (fully pulled rope = platform down)

Servo myServo;

// ========== SETUP FUNCTION ==========
void setup() {
  Serial.begin(115200);
  delay(500);
  
  // Initialize servo
  myServo.attach(SERVO_PIN);
  myServo.write(90);       // Start at center position
  Serial.println("Servo initialized at 90 degrees");
  
  // Initialize ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  
  Serial.println("=================================");
  Serial.println("Ball Balancing System Ready!");
  Serial.print("Target distance: ");
  Serial.print(setpoint);
  Serial.println(" cm");
  Serial.println("Place ball on the platform");
  Serial.println("=================================");
  
  delay(2000);
}

// ========== MAIN LOOP ==========
void loop() {
  // Calculate time difference
  unsigned long now = micros();
  dt = (now - lastTime) / 1000000.0;
  lastTime = now;
  
  // Read current ball position
  float distance = readUltrasonic();
  
  // Calculate PID
  calculatePID(distance);
  
  // Apply PID output to servo
  updateServo();
  
  // Debug output
  printDebug(distance);
  
  lastError = error;
  delay(25);    // 40Hz control loop
}

// ========== READ ULTRASONIC SENSOR ==========
float readUltrasonic() {
  // Send trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure echo pulse duration (with 30ms timeout)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  
  // Calculate distance (speed of sound = 343 m/s = 0.034 cm/μs)
  // Distance = (duration / 2) * 0.034
  if (duration == 0) {
    return setpoint;  // No reading - assume ball at center
  }
  
  float distance = duration * 0.034 / 2;
  
  // Limit to realistic range (5cm to 23cm for your holder)
  distance = constrain(distance, 5.0, 23.0);
  
  return distance;
}

// ========== PID CALCULATION ==========
void calculatePID(float currentDistance) {
  // Calculate error (positive = ball too far, negative = ball too close)
  error = setpoint - currentDistance;
  
  // Integral term with anti-windup (limit accumulation)
  integral += error * dt;
  integral = constrain(integral, -100, 100);
  
  // Derivative term (rate of change)
  derivative = (error - lastError) / dt;
  
  // Calculate PID output
  output = (Kp * error) + (Ki * integral) + (Kd * derivative);
  
  // Limit output to prevent extreme movements
  output = constrain(output, -20, 20);
}

// ========== UPDATE SERVO BASED ON PID OUTPUT ==========
void updateServo() {
  // Apply dead zone to prevent jitter at center
  if (abs(error) < tolerance) {
    // Within dead zone - don't change servo angle
    return;
  }
  
  // Output positive means ball is too far (need to raise platform)
  // Output negative means ball is too close (need to lower platform)
  // Adjust servo angle accordingly
  servoAngle += output;
  
  // Keep servo within safe range
  servoAngle = constrain(servoAngle, minAngle, maxAngle);
  
  // Move servo to new position
  myServo.write(servoAngle);
}

// ========== DEBUG OUTPUT ==========
void printDebug(float distance) {
  static unsigned long lastPrint = 0;
  
  // Print every 100ms (not every loop)
  if (millis() - lastPrint >= 100) {
    lastPrint = millis();
    
    Serial.print("Dist: ");
    Serial.print(distance);
    Serial.print(" cm | Error: ");
    Serial.print(error);
    Serial.print(" | P: ");
    Serial.print(Kp * error, 1);
    Serial.print(" I: ");
    Serial.print(Ki * integral, 1);
    Serial.print(" D: ");
    Serial.print(Kd * derivative, 1);
    Serial.print(" | Output: ");
    Serial.print(output, 1);
    Serial.print(" | Servo: ");
    Serial.print(servoAngle);
    Serial.print("°");
    
    // Add visual indicator of ball position
    Serial.print(" | Ball: ");
    if (error < -2) {
      Serial.println("TOO CLOSE ⬅️");
    } else if (error > 2) {
      Serial.println("TOO FAR ➡️");
    } else {
      Serial.println("CENTER ✅");
    }
  }
}

// ========== HELPER FUNCTION: TUNE PID FROM SERIAL ==========
// Uncomment this section if you want to tune PID without re-uploading

/*
void serialTuning() {
  if (Serial.available()) {
    String command = Serial.readStringUntil(' ');
    float value = Serial.parseFloat();
    
    if (command == "Kp") {
      Kp = value;
      Serial.print("Kp set to: ");
      Serial.println(Kp);
    } 
    else if (command == "Ki") {
      Ki = value;
      Serial.print("Ki set to: ");
      Serial.println(Ki);
    }
    else if (command == "Kd") {
      Kd = value;
      Serial.print("Kd set to: ");
      Serial.println(Kd);
    }
    else if (command == "set") {
      setpoint = value;
      Serial.print("Setpoint set to: ");
      Serial.println(setpoint);
    }
  }
}*/
