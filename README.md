# 🌧️ Smart Drainage System — Arduino UNO

> **An automated, low-cost drainage monitoring system** that detects rainfall and tank water levels, automatically activates a pump when water is high, and alerts users with a buzzer — all displayed live on a 16×2 LCD.

---

## 📌 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Components Required](#-components-required)
- [Circuit Connections](#-circuit-connections)
- [Working Logic](#-working-logic)
- [Project Structure](#-project-structure)
- [Installation & Upload](#-installation--upload)
- [Serial Monitor Output](#-serial-monitor-output)
- [Troubleshooting](#-troubleshooting)
- [Future Improvements](#-future-improvements)
- [License](#-license)

---

## 🌐 Overview

The **Smart Drainage System** is a Arduino project designed to solve waterlogging problems in drainage tanks, basements or agricultural pits. It uses an **ultrasonic sensor** to measure the water level inside a tank and a **rain sensor** to detect active rainfall. When the water level becomes critically high, a **relay-controlled pump** automatically drains the water, while a **buzzer** sounds an alert. All status information is displayed on a **16×2 I2C LCD screen** in real time.

This project demonstrates core embedded systems concepts including:
- Digital & analog sensor interfacing
- Relay-based actuator control
- I2C communication (LCD)
- Threshold-based automation with hysteresis
- Common-ground design across multiple power rails

---

## ✨ Features

| Feature | Description |
|---|---|
| 🌧️ Rain Detection | Detects live rainfall using rain sensor DO output |
| 📏 Water Level Sensing | HC-SR04 ultrasonic sensor measures distance |
| 🔄 Auto Pump Control | Relay activates pump automatically at HIGH water level |
| 🔔 Buzzer Alert | Active buzzer sounds when water level is critical |
| 🖥️ LCD Display | 16×2 I2C LCD shows rain status + water level (HIGH/MID/LOW) |
| 💡 Hysteresis Logic | Prevents pump flickering between HIGH and MID thresholds |
| 🔌 External Power | Pump powered by separate 9V battery — Arduino stays safe |
| 🛠️ No Transistor Needed | Relay module has built-in driver circuit |

---

## 🧰 Components Required

| # | Component | Quantity | Notes |
|---|---|---|---|
| 1 | Arduino UNO | 1 | Main microcontroller |
| 2 | HC-SR04 Ultrasonic Sensor | 1 | Water level measurement |
| 3 | Rain Sensor Module | 1 | With DO (digital output) pin |
| 4 | 5V Relay Module | 1 | Active LOW, with built-in driver |
| 5 | DC Water Pump | 1 | 3V–5V rated |
| 6 | 9V Battery (external) | 1 | Powers the pump only |
| 7 | 16×2 LCD with I2C Module | 1 | I2C address: 0x27 or 0x3F |
| 8 | Active Buzzer | 1 | 5V, active HIGH |
| 9 | Jumper Wires | Several | Male-to-male, male-to-female |
| 10 | Breadboard (optional) | 1 | For prototyping |

---

## 🔌 Circuit Connections

### 1️⃣ 16×2 LCD (I2C Module)

| LCD I2C Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

### 2️⃣ Ultrasonic Sensor (HC-SR04)

| HC-SR04 Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | D9 |
| ECHO | D10 |

### 3️⃣ Rain Sensor Module

| Rain Sensor Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| DO | D2 |

> 💡 Use the **DO (digital output)** pin, not AO. Adjust the onboard potentiometer to set rain sensitivity.

### 4️⃣ Relay Module

| Relay Pin | Connected To |
|---|---|
| VCC | Arduino 5V |
| GND | Arduino GND |
| IN | Arduino D7 |

### 5️⃣ Pump + External Battery

| Connection | From → To |
|---|---|
| Battery (+) | → Relay COM |
| Relay NO | → Pump (+) |
| Pump (–) | → Battery (–) |
| Battery (–) | → Arduino GND ⚠️ (common ground) |

> ⚠️ **IMPORTANT:** Connect the external battery's negative terminal to Arduino GND to establish a **common ground**. Without this, the relay circuit will not work correctly.

### 6️⃣ Active Buzzer

| Buzzer Pin | Arduino Pin |
|---|---|
| Positive (+) | D8 |
| Negative (–) | GND |

---

## ⚙️ Working Logic

```
┌──────────────────────────────────────────────────────┐
│                   SYSTEM BOOT                        │
│         LCD shows "Smart Drain System v1.0"          │
└──────────────────┬───────────────────────────────────┘
                   │
                   ▼
        ┌──────────────────────┐
        │  Read Rain Sensor DO │  → D2 LOW = RAIN: YES
        └──────────┬───────────┘
                   │
                   ▼
        ┌──────────────────────────────┐
        │ Measure Distance (HC-SR04)   │
        │   TRIG pulse → ECHO timing   │
        │   dist = duration × 0.0343 / 2│
        └──────────┬───────────────────┘
                   │
          ┌────────┴─────────┐
          │  dist ≤ 9 cm ?   │
          └────┬─────────────┘
          YES  │          NO
               ▼               ┌──────────────────┐
     ┌──────────────────┐      │  dist > 18 cm ?  │
     │  Level: HIGH     │      └────┬─────────────┘
     │  Relay ON (LOW)  │      YES  │          NO
     │  Buzzer ON       │           ▼        ┌──────────────┐
     │  LCD: "HIGH"     │  ┌─────────────┐   │ Level: MID   │
     └──────────────────┘  │ Level: LOW  │   │ Maintain     │
                            │ Relay OFF   │   │ current state│
                            │ Buzzer OFF  │   └──────────────┘
                            │ LCD: "LOW"  │
                            └─────────────┘
                   │
                   ▼
        ┌──────────────────────┐
        │  Update LCD Display  │
        │  Line 1: RAIN:YES/NO │
        │  Line 2: LVL:HIGH    │
        └──────────────────────┘
                   │
              delay(500ms)
                   │
                   └──── Loop back ────►
```

### 📊 Water Level Thresholds

| Distance from Sensor | Level | Pump | Buzzer |
|---|---|---|---|
| ≤ 9 cm | 🔴 HIGH | ON | ON |
| 10–18 cm | 🟡 MID | Maintained (hysteresis) | Maintained |
| > 18 cm | 🟢 LOW | OFF | OFF |

> 🔁 **Hysteresis:** The MID zone acts as a buffer. The pump won't oscillate ON/OFF rapidly — it stays in its last state until the level firmly moves to HIGH or LOW.

---

## 📁 Project Structure

```
SmartDrainageSystem/
│
├── SmartDrainageSystem.ino    # Main Arduino sketch
├── README.md                  # This documentation file
└── circuit/
    └── wiring_diagram.png     # (Add your Fritzing diagram here)
```

---

## 💻 Installation & Upload

### Prerequisites

1. Install [Arduino IDE](https://www.arduino.cc/en/software) (v1.8+ or v2.x)
2. Install the **LiquidCrystal I2C** library:
   - Open Arduino IDE → **Sketch → Include Library → Manage Libraries**
   - Search for `LiquidCrystal I2C` by **Frank de Brabander**
   - Click **Install**

### Upload Steps

```bash
# 1. Clone this repository
git clone https://github.com/YOUR_USERNAME/SmartDrainageSystem.git

# 2. Open the sketch
#    File → Open → SmartDrainageSystem.ino

# 3. Select Board
#    Tools → Board → Arduino UNO

# 4. Select Port
#    Tools → Port → COMx (Windows) or /dev/ttyUSBx (Linux/Mac)

# 5. Upload
#    Click the Upload button (→)
```

### Find Your LCD I2C Address

If your LCD doesn't display anything, run an I2C scanner sketch to find the correct address (commonly `0x27` or `0x3F`), then update this line in the code:

```cpp
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change 0x27 if needed
```

---

## 📟 Serial Monitor Output

Open Serial Monitor at **9600 baud** to see live debug output:

```
Rain:NO   Dist:22cm  Level:LOW    Pump:OFF
Rain:NO   Dist:14cm  Level:MID    Pump:OFF
Rain:YES  Dist:7cm   Level:HIGH   Pump:ON
Rain:YES  Dist:7cm   Level:HIGH   Pump:ON
Rain:YES  Dist:20cm  Level:LOW    Pump:OFF
```

---

## 🛠️ Troubleshooting

| Problem | Possible Cause | Solution |
|---|---|---|
| LCD shows nothing | Wrong I2C address | Run I2C scanner; change address in code |
| LCD shows garbled text | Poor I2C connection | Check SDA/SCL wires on A4/A5 |
| Pump doesn't run | No common ground | Connect battery (–) to Arduino GND |
| Pump doesn't run | Low battery voltage | Use a charged 9v–6V battery; check pump rating |
| Relay clicks but pump off | COM/NO wiring wrong | Ensure battery (+) → COM, NO → Pump (+) |
| Rain sensor always YES | Sensitivity too high | Turn onboard potentiometer clockwise |
| Distance reads 999 cm | Ultrasonic timeout | Check TRIG/ECHO wires on D9/D10 |
| Buzzer always ON | Active LOW buzzer used | Change `HIGH` to `LOW` in buzzer logic |

---

## 🚀 Future Improvements

- 📱 **GSM/SMS Alert** — Send SMS when water level is critical using SIM800L
- 🌐 **IoT Dashboard** — Send data to ThingSpeak or Blynk via ESP8266 Wi-Fi
- 📈 **Data Logging** — Log water level readings to SD card over time
- 🔋 **Solar Power** — Add solar panel + charge controller for off-grid use
- 📊 **OLED Display** — Upgrade to graphical OLED for water level bar chart
- 🌡️ **Turbidity Sensor** — Detect water quality / pollution level
- ⏱️ **RTC Module** — Log timestamps with every pump activation

---

## 👨‍💻 Author

**BISMAYA GURUPRASAD PANDA**
- 📧 Email: bismayabismaya04@email.com
- 🐙 GitHub: [@bismaya-guruprasad-panda-8741](https://github.com/bismaya-guruprasad-panda-8741)
<p>With Co-Author/ My Friend
Debasish Nayak
Github - https://github.com/Debasish-Nayak-556</p>

---

## 📄 License

This project is licensed under the **MIT License** — feel free to use, modify, and distribute it for educational and personal projects.

```
MIT License © 2025 Bismaya Guruprasad Panda
```

---

## ⭐ Show Your Support

If this project helped you, please consider giving it a **⭐ star** on GitHub — it motivates further development!

> 🎓 *Built as a diploma-level embedded systems project. Perfect for learning Arduino, sensors, relay control, and I2C communication.*
