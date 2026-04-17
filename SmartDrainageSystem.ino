#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/*
  ==========================================
            SMART DRAINAGE SYSTEM          |
  ==========================================
  Features:
  - Rain Detection (Digital)
  - Water Level Measurement (Ultrasonic)
  - Automatic Pump Control (Relay)
  - Buzzer Alert at High Level
  - LCD Display (Rain + Water Level)

  Tank Depth: 30 cm
  Thresholds:
    HIGH  → ≤ 9 cm
    MID   → ≤ 18 cm
    LOW   → > 18 cm
  ==========================================
*/

// -------- Pin Definitions --------
#define TRIG_PIN   9
#define ECHO_PIN   10
#define RAIN_PIN   2
#define RELAY_PIN  7
#define BUZZER_PIN 8

// -------- LCD Setup (I2C Address) --------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// -------- Global Variables --------
long duration;
float distance;
bool pumpState = false;   // Latch variable for stable control

// ==========================================
// Function: Measure Distance using Ultrasonic
// ==========================================
float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  // Distance formula (cm)
  float d = duration * 0.034 / 2;
  return d;
}

// ==========================================
// Setup Function (Runs Once)
// ==========================================
void setup() {

  // Initialize Pin Modes
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initial States
  digitalWrite(RELAY_PIN, HIGH);   // Relay OFF (Active LOW)
  digitalWrite(BUZZER_PIN, LOW);   // Buzzer OFF

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.backlight();

  // Startup Message
  lcd.setCursor(0, 0);
  lcd.print("SMART DRAINAGE");
  delay(2000);
  lcd.clear();
}

// ==========================================
// Main Loop (Runs Continuously)
// ==========================================
void loop() {

  // -------- Read Sensors --------
  int rainStatus = digitalRead(RAIN_PIN);
  distance = getDistance();

  // -------- Pump Control Logic (Latch) --------

  // Turn ON pump when water is HIGH
  if (distance <= 9) {
    pumpState = true;
  }

  // Turn OFF pump when water reduces to MID
  if (distance > 18) {
    pumpState = false;
  }

  // -------- Apply Outputs --------
  if (pumpState) {
    digitalWrite(RELAY_PIN, LOW);    // Pump ON
    digitalWrite(BUZZER_PIN, HIGH);  // Buzzer ON
  } else {
    digitalWrite(RELAY_PIN, HIGH);   // Pump OFF
    digitalWrite(BUZZER_PIN, LOW);   // Buzzer OFF
  }

  // -------- LCD Display --------
  lcd.clear();

  // Line 1: Rain Status
  lcd.setCursor(0, 0);
  if (rainStatus == LOW) {
    lcd.print("RAIN: YES");
  } else {
    lcd.print("RAIN: NO ");
  }

  // Line 2: Water Level Status
  lcd.setCursor(0, 1);
  if (distance <= 9) {
    lcd.print("LEVEL: HIGH");
  }
  else if (distance <= 18) {
    lcd.print("LEVEL: MID ");
  }
  else {
    lcd.print("LEVEL: LOW ");
  }

  // Small delay for stability
  delay(800);
}