#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>
#include <RTClib.h>
#include <MPU6050_tockn.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// Pin Definitions
#define IR_RECEIVE_PIN 16
#define BUZZER_PIN 17
#define PIR_PIN 13
#define RUDDER_SERVO_PIN 12
#define SAIL_SERVO_PIN 14
#define TRIGGER_PIN 5
#define ECHO_PIN 18

// Constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define MAX_SAILING_TIME 3600000 // 1 hour sailing max
#define TACK_INTERVAL 15000 // 15 seconds
#define OBSTACLE_DISTANCE 100 // 1 meter

// Objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo rudderServo;
Servo sailServo;
RTC_DS3231 rtc;
MPU6050 mpu6050(Wire);

// IR Receiver object
IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

// Global Variables
float heading = 0;
int rudderAngle = 90;
int sailAngle = 90;
float distance = 0;
bool obstacleDetected = false;
int sailMode = 0; // 0: Manual, 1: Auto Sailing, 2: Return to Home
unsigned long sailingStartTime = 0;
unsigned long lastTackTime = 0;
float roll = 0;
float pitch = 0;
float windDirection = 0;

void setup() {
  Serial.begin(115200);
  initializeComponents();
  calibrateSensors();
}

void loop() {
  readSensors();
  processIRCommands();
  updateNavigation();
  controlSailboat();
  updateDisplay();
  checkSafetyConditions();
  delay(100);
}

void initializeComponents() {
  
  // Initialize IR receiver
  irrecv.enableIRIn();
  
  // Initialize buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize PIR sensor
  pinMode(PIR_PIN, INPUT);
  
  // Initialize servos
  rudderServo.attach(RUDDER_SERVO_PIN);
  sailServo.attach(SAIL_SERVO_PIN);
  rudderServo.write(rudderAngle);
  sailServo.write(sailAngle);
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Initialize ultrasonic sensor
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Initialize MPU6050
  Wire.begin();
  mpu6050.begin();
}

void calibrateSensors() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Calibrating...");
  display.println("Keep boat still");
  display.display();
  
  mpu6050.calcGyroOffsets(true);
  
  display.println("Calibration done!");
  display.display();
  delay(2000);
}

void readSensors() {
  // Read ultrasonic sensor
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;
  obstacleDetected = (distance < OBSTACLE_DISTANCE);
  
  // Read motion sensor
  bool motionDetected = digitalRead(PIR_PIN);
  
  // Read current time from RTC
  DateTime now = rtc.now();
  
  // Read MPU6050
  mpu6050.update();
  heading = mpu6050.getAngleZ();
  roll = mpu6050.getAngleX();
  pitch = mpu6050.getAngleY();
  
  // Estimate wind direction based on boat heel
  windDirection = heading + (roll > 0 ? 90 : -90);
  windDirection = fmod(windDirection + 360, 360);
  
  // Use the sensor data as needed
  if (motionDetected) {
    Serial.print("Motion detected at ");
    Serial.println(now.timestamp());
  }
}

void processIRCommands() {
  if (irrecv.decode(&results)) {
    switch(results.value) {
      case 0xFF30CF: // Example Buttons: Button 1
        setSailMode(0);
        break;
      case 0xFF18E7: // Button 2
        setSailMode(1);
        break;
      case 0xFF7A85: // Button 3
        setSailMode(2);
        break;
      case 0xFF10EF: // Button 4 - Emergency Stop
        emergencyStop();
        break;
    }
    irrecv.resume();
  }
}

void setSailMode(int mode) {
  sailMode = mode;
  if (mode == 1 || mode == 2) {
    sailingStartTime = millis();
  }
}

void updateNavigation() {
  if (sailMode == 1) { // Auto Sailing mode
    autoSail();
  } else if (sailMode == 2) { // Return to Home mode
    returnToHome();
  }
}

void autoSail() {
  float relativeWindDirection = fmod(windDirection - heading + 360, 360);
  
  if (relativeWindDirection > 45 && relativeWindDirection < 135) {
    // Beam reach
    rudderAngle = 90;
    sailAngle = 45;
  } else if (relativeWindDirection >= 135 && relativeWindDirection < 225) {
    // Running
    rudderAngle = 90;
    sailAngle = 90;
  } else if (relativeWindDirection >= 225 && relativeWindDirection < 315) {
    // Beam reach (other side)
    rudderAngle = 90;
    sailAngle = 45;
  } else {
    // Close-hauled, need to tack
    if (millis() - lastTackTime > TACK_INTERVAL) {
      rudderAngle = (rudderAngle == 45) ? 135 : 45;
      sailAngle = 20;
      lastTackTime = millis();
    }
  }
}

void returnToHome() {
  // Simplified return to home logic
  rudderAngle = 90; // Go straight
  sailAngle = 45; // Set sail for moderate speed
}

void controlSailboat() {
  if (obstacleDetected) {
    avoidObstacle();
  } else {
    rudderServo.write(rudderAngle);
    sailServo.write(sailAngle);
  }
}

void avoidObstacle() {
  rudderAngle = 0; // Avoid obstacle
  sailAngle = 90; // Slow down
  tone(BUZZER_PIN, 1000, 100); // Alarm
  rudderServo.write(rudderAngle);
  sailServo.write(sailAngle);
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0,0);
  DateTime now = rtc.now();
  display.print("Time: ");
  display.println(now.timestamp());
  display.print("Mode: ");
  display.println(sailMode == 0 ? "Manual" : (sailMode == 1 ? "Auto" : "RTH"));
  display.print("Head: ");
  display.println(heading, 1);
  display.print("Roll: ");
  display.println(roll, 1);
  display.print("Wind: ");
  display.println(windDirection, 1);
  display.print("Dist: ");
  display.println(distance);
  display.display();
}

void checkSafetyConditions() {
  unsigned long sailingTime = millis() - sailingStartTime;
  
  if (sailingTime > MAX_SAILING_TIME) {
    emergencyStop();
  }
}

void emergencyStop() {
  sailMode = 0;
  rudderAngle = 90;
  sailAngle = 90;
  rudderServo.write(rudderAngle);
  sailServo.write(sailAngle);
  tone(BUZZER_PIN, 2000, 1000);
}