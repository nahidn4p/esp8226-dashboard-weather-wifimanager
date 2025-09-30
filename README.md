# ESP8266 WiFi Connect + Weather Dashboard

This project runs on an **ESP8266 (e.g., NodeMCU, Wemos D1 mini)** and provides a web dashboard with live weather data from **OpenWeatherMap**, device information, and LED control.  

---

## 🚀 Features
- WiFi captive portal for easy setup (using [WiFiManager](https://github.com/tzapu/WiFiManager))  
- Live temperature, humidity, and location from **OpenWeatherMap API**  
- Responsive web dashboard with charts (temperature in red, humidity in blue)  
- Device info: SSID, IP, MAC, RSSI, uptime  
- LED control (ON/OFF toggle with blinking effect when ON)  
- Auto-refreshing dashboard every 10s  

---

## 📥 Installation

### Step 1: Install Arduino IDE and ESP8266 Core
1. Install [Arduino IDE](https://www.arduino.cc/en/software).  
2. Open **File → Preferences**.  
3. In *Additional Board Manager URLs*, add:
```url
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
5. Go to **Tools → Board → Board Manager**, search **esp8266**, and install it.  

---

### Step 2: Install WiFi Manager Library

Two options:

**Option A: Arduino Library Manager**  
1. Open Arduino IDE → **Sketch → Include Library → Manage Libraries…**  
2. Search for `WiFiManager`.  
3. Install the latest version.  

**Option B: Manual Install from GitHub**  
1. Download ZIP from [WiFiManager releases](https://github.com/tzapu/WiFiManager).  
2. In Arduino IDE, go to **Sketch → Include Library → Add .ZIP Library…**  
3. Select the downloaded ZIP.  

---

### Step 3: Install ArduinoJson
1. Open Arduino IDE → **Sketch → Include Library → Manage Libraries…**  
2. Search for `ArduinoJson`.  
3. Install version **6.x** (latest stable).  

---

## 🔑 OpenWeatherMap Setup
1. Create a free account at [OpenWeatherMap](https://openweathermap.org/api).  
2. Generate your API key.  
3. Replace the placeholder in the code:  

```cpp
const char* apiKey = "YOUR_API_KEY_HERE";
```
4. Update your latitude/longitude to your location.

### Uploading the Code

1. Connect your ESP8266 via USB.
2. Select board (e.g., NodeMCU 1.0 (ESP-12E)) and correct COM port in Arduino IDE.
3. Upload the sketch.
3. On first boot, ESP will start a WiFi AP named ESP8266_AP.
4. Connect to it → open 192.168.4.1 in a browser → select your WiFi SSID + password.
5. ESP will reboot and connect to your WiFi.

### 🌐 Access Dashboard

1. Find ESP IP from your router or Serial Monitor.
2. Open in browser → live dashboard loads.

### 📷 Dashboard Preview

1. Shows Temperature & Humidity chart
2. Displays SSID, IP, MAC, RSSI, Uptime
3. Control LED (ON/OFF + Blink)
4. Auto-refreshes every 10 seconds

### 🛠️ Hardware Required

- ESP8266 (NodeMCU, Wemos D1 Mini, etc.)
- USB cable
- WiFi network

### ⚠️ Notes
- Powering ESP via router USB is safe but use a 5V/1A adapter for stability.
- On power loss, ESP restarts and reconnects automatically.
- Current version: LED state resets to OFF after reboot (no flash wear).

### 📜 License
MIT License – feel free to use and modify.
