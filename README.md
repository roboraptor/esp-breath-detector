# ESP32 Breath Sensor PoC (Elastomer Stretch Sensor)

This project is a **proof-of-concept** for breath detection using a **conductive elastomer rubber tube (stretch sensor)**. It demonstrates how to analyze a breathing curve in real-time using an ESP32 and simple electronics. The output signal can be used to control various devices, but the primary goal is to demonstrate the sensor technology itself.

The project includes an advanced **Tasmota-style** web interface with a live chart, manual control options, and OTA update capabilities.

## 🧬 Sensor Principle

The sensor used is a **conductive carbon rubber** (elastomer cord).

1.  The rubber cord is secured around the chest (e.g., with kinesiology tape or a strap).
2.  As the chest expands during inhalation, the cord stretches.
3.  **Resistance Change:** The stretching changes the material's electrical resistance.
4.  The ESP32 measures this resistance via a voltage divider on an analog input and programmatically detects breathing phases (inhalation/exhalation).

## ⚙️ Hardware

*   **MCU:** ESP32 (DevKit V1 or compatible)
*   **Sensor:** Conductive Rubber Cord / Stretch Sensor
*   **Actuator (Example):** A relay to demonstrate switching (e.g., for a 5V/12V mini air pump).
*   **Switching (Example):** A MOSFET transistor (e.g., IRLZ44N) or a relay module to control an external device.
*   **Resistor:** 10kΩ - 47kΩ (for the voltage divider with the sensor)

### Pinout

| Component | ESP32 Pin | Note |
| :--- | :--- | :--- |
| **Sensor (ADC)** | `GPIO 34` | Analog input for resistance measurement (voltage divider) |
| **Output (PWM/Digital)** | `GPIO 25` | Example output for control (e.g., MOSFET Gate) |

*Note: Connect the sensor in series with the resistor between 3.3V and GND. Connect the point between the sensor and the resistor to GPIO 34.*

## 🖥️ Web Interface (WebUI)

After connecting to WiFi (or via AP mode), the interface is available at the ESP32's IP address (port 80).

*   **Design:** Tasmota Dark Theme (mobile-friendly).
*   **Live Chart:** Real-time graph of the breathing curve (canvas, 20Hz refresh).
*   **Indicators:**
    *   `🔼 IN` - Inhaling
    *   `⏸️ HOLD IN` - Breath Hold (Full)
    *   `🔽 EX` - Exhaling
    *   `⏸️ HOLD EX` - Rest State (Empty)
*   **Controls:**
    *   **Breathe Aroma:** Activates the output signal only when inhalation or a resting state is detected. Demonstrates reactive control based on breathing.
    *   **Force Aroma:** Activates the output signal immediately, regardless of breath (for testing and manual override).

## 🚀 Installation and Flashing

The project is built on the **PlatformIO** framework.

1.  Open the project folder in VS Code with the PlatformIO extension.
2.  Connect the ESP32 via USB.
3.  Upload the firmware (click the arrow icon in the bottom bar or run `PlatformIO: Upload`).

### First Run (WiFi Manager)

1.  After flashing, if the ESP32 doesn't know your WiFi credentials, it will create an access point (Hotspot):
    *   SSID: **BreathSensorAP**
    *   Password: *(none)*
2.  Connect to this network with your phone.
3.  A configuration page (Captive Portal) should appear automatically.
4.  Select your home WiFi, enter the password, and save.
5.  The ESP32 will restart and connect to your network.

## 🧩 Code Structure

*   `src/main.cpp` - Main loop and initializations.
*   `src/sensor.cpp` - ADC reading, noise filtering, and breath detection logic.
*   `src/network.cpp` - WiFiManager, WebServer, HTML/JS generator.
*   `src/globals.cpp` - Shared variables and configuration.

## 🛠️ Debugging

For sensor debugging, open the **Serial Monitor** (baud rate 115200) or **Serial Plotter**. You will see raw data, filtered data, and detected states in real-time.
