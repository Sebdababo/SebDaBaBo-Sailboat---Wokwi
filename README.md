# SebDaBaBo Sailboat Project

This project involves a smart sailboat controlled by an ESP32 microcontroller, capable of autonomous navigation using various sensors and manual override through an IR remote. The sailboat can perform auto sailing, return to home functionality, and avoid obstacles detected through its sensors. The system's state and critical information are displayed on an OLED screen, and it can handle emergency situations via an onboard buzzer alarm.

The project is available both as a physical setup and a Wokwi simulation for easier testing and demonstration.

## Components Required

- **ESP32 Microcontroller**
- **OLED Display (128x64)**
- **MPU6050 Accelerometer and Gyroscope Sensor**
- **DS3231 Real-Time Clock (RTC) Module**
- **Ultrasonic Sensor (HC-SR04)**
- **PIR Motion Sensor**
- **IR Receiver Module**
- **Servo Motors (2x)** - One for rudder control, one for sail control
- **Buzzer**
- **Connecting Wires**
- **Breadboard**

## Libraries Required

Ensure you have the following libraries installed in your Arduino IDE:

- `Wire.h` - For I2C communication
- `Adafruit_GFX.h` - For graphics on the OLED display
- `Adafruit_SSD1306.h` - For controlling the OLED display
- `ESP32Servo.h` - For controlling servos with the ESP32
- `RTClib.h` - For handling the DS3231 RTC module
- `MPU6050_tockn.h` - For reading and processing data from the MPU6050 sensor
- `IRremoteESP8266.h`, `IRrecv.h`, `IRutils.h` - For handling IR remote input

## Circuit Diagram

1. **IR Receiver Module**:  
   - Signal pin to digital pin `16`
   - VCC to `3.3V`
   - GND to `GND`

2. **Buzzer**:  
   - Signal pin to digital pin `17`
   - VCC to `3.3V`
   - GND to `GND`

3. **PIR Sensor**:  
   - Signal pin to digital pin `13`
   - VCC to `5V`
   - GND to `GND`

4. **Servo Motors**:  
   - Rudder servo signal to digital pin `12`
   - Sail servo signal to digital pin `14`
   - Both powered from `5V` and `GND`

5. **Ultrasonic Sensor (HC-SR04)**:  
   - Trigger pin to digital pin `5`
   - Echo pin to digital pin `18`
   - VCC to `5V`
   - GND to `GND`

6. **OLED Display (128x64)**:  
   - SCL to `SCL`
   - SDA to `SDA`
   - VCC to `3.3V`
   - GND to `GND`

7. **MPU6050 Sensor**:  
   - SCL to `SCL`
   - SDA to `SDA`
   - VCC to `3.3V`
   - GND to `GND`

8. **DS3231 RTC Module**:  
   - SCL to `SCL`
   - SDA to `SDA`
   - VCC to `3.3V`
   - GND to `GND`

## Setup Instructions

1. **Hardware Setup**:
   - Connect all components according to the circuit diagram described above.
   - Ensure the power connections are accurate to prevent damage to components.

2. **Software Setup**:
   - Install the required libraries via the Arduino IDE's Library Manager.
   - Download or clone this repository from GitHub.
   - Open the project sketch (`.ino` file) in the Arduino IDE.
   - Upload the sketch to your ESP32 microcontroller.

## How to Use

1. **Sailing Modes**:
   - **Manual Mode**: Control the sailboat manually using the rudder and sail servos.
   - **Auto Sailing Mode**: The sailboat uses sensor data to navigate automatically, avoiding obstacles and optimizing sail position based on wind direction.
   - **Return to Home Mode**: The sailboat attempts to return to its starting position.

2. **IR Remote Control**:
   - Button 1: Switch to Manual Mode
   - Button 2: Switch to Auto Sailing Mode
   - Button 3: Switch to Return to Home Mode
   - Button 4: Emergency Stop

3. **Obstacle Detection**:
   - The ultrasonic sensor measures the distance to objects in front of the boat.
   - If an obstacle is detected within 1 meter, the sailboat takes evasive action and sounds an alarm.

4. **Display Information**:
   - The OLED screen shows real-time data such as current mode, heading, roll, wind direction, distance to obstacles, and the current time.

5. **Safety Features**:
   - The sailboat will automatically stop if the maximum sailing time is exceeded.
   - Emergency stop can be triggered via the IR remote to halt all operations immediately.

## Simulation

You can test this project using the [Wokwi simulation platform](https://wokwi.com/projects/406195414988333057). This simulation allows you to run and modify the sailboat project without needing the physical components.

## Project Setup

<img width="1617" alt="image" src="https://github.com/user-attachments/assets/1b9edeee-e1e9-4c60-96db-b020dadaece6">


