# Self-Balancing Ball Project with ESP32 🎯
A self-balancing ball system that uses an ultrasonic sensor and servo motor to keep a ball centered on a 23cm platform.
The system employs a PID controller to make real-time adjustments based on the ball's position.


<img width="1500" height="800" alt="image" src="https://github.com/user-attachments/assets/724c6709-46d4-494c-88e4-322db317893e" />

## 📸 Project Overview
This project creates a mechanical platform where a ball rolls freely. An ultrasonic sensor detects the ball's position, and a servo motor adjusts the platform's tilt via a rope mechanism to keep the ball balanced at the center.

##  Components Used

| Component | Quantity | Description |
|-----------|----------|-------------|
| **ESP32-S NodeMCU** | 1 | Main microcontroller (3.3V logic) |
| **HC-SR04 Ultrasonic Sensor** | 1 | 4-pin distance measuring sensor (5V) |
| **Servo Motor (SG90/MG995)** | 1 | Controls platform tilt via rope |
| **Jumper Wires** | Multiple | Female-Female, Female-Male, Male-Male |
| **10kΩ Resistors** | 3 | For voltage divider circuit |
| **Breadboard** | 1 | For prototyping connections |

##  Platform Specifications

- **Holder length:** 23 cm feel free to adjust them
- **Target position (setpoint):** 11.5 cm (center) 
- **Ball:** Any small, round object that rolls freely

## 🎯 How It Works
Ball Position → Ultrasonic Sensor → ESP32 → PID Calculation → Servo Angle → Platform Tilt → Ball Adjusts Position

### Logic Table

| Ball Position | Distance | Error | Servo Action | Platform Effect |
|---------------|----------|-------|--------------|-----------------|
| Too Close | < 10 cm | Negative | Release rope (angle ↓) | Platform rises |
| At Center | ~11.5 cm | Near Zero | Hold position | Level |
| Too Far | > 13 cm | Positive | Pull rope (angle ↑) | Platform lowers |


## 🚀 Our Journey: From Zero to Working Project

### Chapter 1: First Time with ESP32

This was our **first time** using an ESP32 microcontroller. We had previous experience with Arduino (including accidentally burning one!), so we approached this project with extra caution.

### Chapter 2: Installing the Board (5+ Hours Struggle!) ⏰

The biggest initial challenge was getting the ESP32 board recognized in Arduino IDE.

**Problem:** After installing the ESP32 board package, nothing worked properly.

**Solution:** The issue was the **version**. The latest version had compatibility problems. We downgraded to an older, stable version, and it finally worked!

**Steps we took:**
1. Added ESP32 board URL to Arduino IDE preferences
2. Tried latest version → Failed
3. Searched forums for solutions
4. **Success:** Downgraded to version 2.0.14
5. Board finally appeared in Tools → Board menu

### Chapter 3: Driver Installation & Device Manager

We discovered that the ESP32 requires specific USB-to-serial drivers.

**Problem:** Computer wasn't recognizing the board properly.

**Solution:** Installed **CP210x driver** from Silicon Labs.

**Steps:**
1. Downloaded CP210x driver from Silicon Labs website
2. Installed the driver
3. Opened Device Manager
4. Identified the correct COM port (COM3 in our case)
5. Selected "NodeMCU-32S" as the board type

### Chapter 4: The Voltage Divider Challenge (Most Critical!) ⚡

This was the most technically challenging part of the project.

**The Problem:** 
- HC-SR04 ultrasonic sensor outputs **5V** on its ECHO pin
- ESP32 expects maximum **3.3V** on GPIO pins
- Direct connection would destroy the ESP32!

**The Solution:** Voltage divider circuit using 3 resistors (10kΩ each)
Vout = Vin × (R_bottom / (R_top + R_bottom))
Vout = 5V × (20kΩ / (10kΩ + 20kΩ))
Vout = 5V × (20/30) = 3.33V ✅ (Safe for ESP32)



**Testing:** We simulated the circuit on **Falstad.com** (online circuit simulator) before building it physically.
<table>
  <tr>
    <td><img width="500" height="450" alt="image" src="https://github.com/user-attachments/assets/63a4d6ac-987f-475e-bae7-44a8b8e078eb" />
</td>
    <td><img width="350" height="450" alt="image" src="https://github.com/user-attachments/assets/5fee48fe-7929-449f-975f-4184e5dd33a4" />
</td>
  </tr>
  <tr>
    <td align="center">Simulation</td>
    <td align="center">Real </td>
  </tr>
</table>


**Learning:** Using only 2 resistors gave 2.5V, which was too low for reliable signal detection. Adding the third resistor increased voltage to 3.33V and solved the problem.

### Chapter 5: Flashing the Code - The "Connecting... , then timeout" problem 

**Problem:** When trying to upload code, we saw:
Connecting...______
A serial exception error occurred: Write timeout

**What we tried:**
- Holding BOOT button while uploading
- Pressing EN button sequences
- Changing COM ports
- Different USB cables

**The Real Solution:** After all that effort, the problem was simply the **driver**! Once we properly installed the CP210x driver, the upload worked immediately.

### Chapter 6: Testing the Servo

Before integrating everything, we tested the servo alone:

**Success Criteria:**
- Servo moves to 0°, 90°, and 180° positions
- No buzzing or overheating
- ESP32 stays warm but not hot

## Wiring Diagram
### Complete Pin Connections
| HC-SR04 Ultrasonic	| ESP32 Pin |
|---------------------|-----------|
| VCC | 	5V / VIN |
| GND	| GND|
| TRIG| GPIO 5|
| ECHO|	GPIO 18 (via voltage divider)|
--- 
|Servo Motor	Connection|ESP|
|-----------------------|---|
|**Red (Power)** | 5V bread board +ve rail (but to be safe use external power supply 5v)|
|**Brown (GND)**	| GND (common with ESP32)|
|**Orange (Signal)**	| GPIO 13|



