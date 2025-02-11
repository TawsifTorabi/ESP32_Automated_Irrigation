#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define the sensor pins
#define SENSOR_1 13  // ESP32 pin D13
#define SENSOR_2 12  // ESP32 pin D12
#define SENSOR_3 14  // ESP32 pin D14
#define SENSOR_4 27  // ESP32 pin D27

// Define the relay pins (active low)
#define RELAY_1 15
#define RELAY_2 19
#define RELAY_3 5
#define RELAY_4 18

// Define moisture thresholds
const int dryThresholds[] = {2000, 1800, 2000, 2000};
const int wetThresholds[] = {1300, 1400, 1200, 1500};

// Moisture percentages
int moisturePercentage1 = 0;
int moisturePercentage2 = 0;
int moisturePercentage3 = 0;
int moisturePercentage4 = 0;

// LCD setup (I2C Address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Timing variables
unsigned long lastLCDUpdate = 0;
const int LCD_UPDATE_INTERVAL = 2000;  // Update LCD every 2 seconds

void setup() {
  Serial.begin(115200);

  // Initialize relays
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, HIGH);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Moisture Sensor");
  delay(1000);
  lcd.clear();
}

void loop() {
  // Read sensor values
  int sensorValue1 = analogRead(SENSOR_1);
  int sensorValue2 = analogRead(SENSOR_2);
  int sensorValue3 = analogRead(SENSOR_3);
  int sensorValue4 = analogRead(SENSOR_4);

  // Calculate moisture percentages
  moisturePercentage1 = calculateMoisturePercentage(sensorValue1, 0);
  moisturePercentage2 = calculateMoisturePercentage(sensorValue2, 1);
  moisturePercentage3 = calculateMoisturePercentage(sensorValue3, 2);
  moisturePercentage4 = calculateMoisturePercentage(sensorValue4, 3);

  // Print moisture percentages to serial monitor
  Serial.print("Sensor 1: "); Serial.print(moisturePercentage1); Serial.println("%");
  Serial.print("Sensor 2: "); Serial.print(moisturePercentage2); Serial.println("%");
  Serial.print("Sensor 3: "); Serial.print(moisturePercentage3); Serial.println("%");
  Serial.print("Sensor 4: "); Serial.print(moisturePercentage4); Serial.println("%");

  // Control the relays based on moisture levels
  controlRelay(RELAY_1, moisturePercentage1);
  controlRelay(RELAY_2, moisturePercentage2);
  controlRelay(RELAY_3, moisturePercentage3);
  controlRelay(RELAY_4, moisturePercentage4);

  // Update LCD every LCD_UPDATE_INTERVAL ms
  if (millis() - lastLCDUpdate >= LCD_UPDATE_INTERVAL) {
    lastLCDUpdate = millis();
    updateLCD();
  }
}

// Function to calculate moisture percentage
int calculateMoisturePercentage(int sensorValue, int sensorIndex) {
  int dryThreshold = dryThresholds[sensorIndex];
  int wetThreshold = wetThresholds[sensorIndex];

  sensorValue = constrain(sensorValue, wetThreshold, dryThreshold);
  int moisturePercentage = map(sensorValue, dryThreshold, wetThreshold, 0, 100);
  return moisturePercentage;
}

// Function to control the relay based on moisture percentage
void controlRelay(int relayPin, int moisturePercentage) {
  if (moisturePercentage < 10) {
    digitalWrite(relayPin, LOW);  // Turn on pump
    Serial.println("Relay ON - Pumping water");
  } else {
    digitalWrite(relayPin, HIGH); // Turn off pump
    Serial.println("Relay OFF - Soil is wet enough");
  }
}

// Function to update LCD display
void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S1:");
  lcd.print(moisturePercentage1);
  lcd.print("%  S2:");
  lcd.print(moisturePercentage2);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("S3:");
  lcd.print(moisturePercentage3);
  lcd.print("%  S4:");
  lcd.print(moisturePercentage4);
  lcd.print("%");
}
