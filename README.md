# 🌱 Smart Irrigation System using ESP32

A smart irrigation system that monitors soil moisture, temperature, and humidity using ESP32, DHT22, and four analog soil sensors. It displays data on an LCD and a web dashboard in real time via WebSocket and automatically controls water pumps through relays.

---

## 📦 Features

- 📊 Real-time data from:
  - 4x Soil Moisture Sensors
  - 1x DHT22 Temperature and Humidity Sensor
- 🧠 Automated pump control based on soil dryness
- 📺 LCD display that alternates between soil and climate data
- 🌐 WebSocket server running on ESP32
- 🖥️ Responsive web dashboard for remote monitoring

---

## 🧰 Hardware Required

| Component               | Quantity |
|------------------------|----------|
| ESP32 Dev Board        | 1        |
| Soil Moisture Sensors  | 4        |
| DHT22 Temperature Sensor | 1      |
| 4-Channel Relay Module | 1        |
| I2C 16x2 LCD Display   | 1        |
| Jumper Wires           | As needed |
| Power Supply           | Suitable for pumps |

---

## 🚀 Getting Started

### 1. ESP32 Setup

1. Install the following libraries in Arduino IDE:
   - `WiFi.h`
   - `Wire.h`
   - `LiquidCrystal_I2C.h`
   - `DHT sensor library`
   - `Adafruit Unified Sensor`
   - `WebSocketsServer`

2. Open `ESP32_Firmware.ino`.

3. Update WiFi credentials:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
   4. Connect sensors to the defined GPIO pins:
   - Soil Moisture Sensors: GPIO 34, 35, 32, 33
   - DHT22: GPIO 4
   - Relays: GPIO 15, 19, 5, 18

5. Upload the code to your ESP32.

6. Once connected, note the IP address printed on the Serial Monitor or LCD.

---

### 2. Web Dashboard Setup

1. Open `dashboard.html` in any modern browser.

2. Update the ESP32 IP address in the script:
   ```html
   <script>
       const ipAddress = '192.168.0.xxx'; // Replace with actual ESP32 IP
   </script>
   ```
3. The dashboard will auto-connect via WebSocket (ws://<ESP32_IP>:81) and start receiving data.

---

## 💡 System Behavior

- Soil moisture is read continuously.
- If any sensor value drops below 10%, its corresponding pump (relay) is turned **ON**.
- Data is sent to connected WebSocket clients every 3 seconds.
- LCD alternates between:
  - Line 1: Soil Moisture % (S1-S4)
  - Line 2: Temperature and Humidity

---

## 📊 Sample JSON Output

```json
{
  "temperature": 27.5,
  "humidity": 68.2,
  "moisture": [35, 22, 5, 80]
}

```
---

## 🛠️ Troubleshooting

- Ensure all components are properly wired.
- Use pull-up resistors for DHT if necessary.
- Ensure ESP32 and PC/phone are on the same WiFi network.
- Make sure port `81` is not blocked by firewalls or antivirus software.
- If "NaN" appears, it means the sensor didn’t return a valid reading.

---

## 🔌 Relay Note

- The relays are **active-low**, meaning:
  - `LOW` = Pump ON
  - `HIGH` = Pump OFF

---

## 📸 Optional Enhancements

- Add a camera stream for visual soil inspection.
- Add a database for logging historical data.
- Integrate Google Assistant or Alexa using IFTTT.
- Control relays manually via dashboard buttons.

---

## 📃 License

This project is licensed under the MIT License.  
Feel free to use, modify, and share with attribution.

---

## 👨‍💻 Author

Developed by **[Tawsif Torabi]**  
[United International University / UIU Mars Rover Team]

> "Grow smarter, not harder."

