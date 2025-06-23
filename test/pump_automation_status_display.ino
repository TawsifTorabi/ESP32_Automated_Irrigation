#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Define the sensor pins
#define SENSOR_1 13
#define SENSOR_2 12
#define SENSOR_3 14
#define SENSOR_4 27
#define DHTPIN 4  // DHT22 sensor connected to D4
#define DHTTYPE DHT22

// Define the relay pins (active low)
#define RELAY_1 15
#define RELAY_2 19
#define RELAY_3 5
#define RELAY_4 18

// Moisture thresholds
const int dryThresholds[] = {2000, 1800, 2000, 2000};
const int wetThresholds[] = {1300, 1400, 1200, 1500};

// Moisture percentages
int moisturePercentage1 = 0;
int moisturePercentage2 = 0;
int moisturePercentage3 = 0;
int moisturePercentage4 = 0;

// DHT22 sensor
DHT dht(DHTPIN, DHTTYPE);
float temperature = 0.0;
float humidity = 0.0;

// LCD setup (I2C Address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Timing variables
unsigned long lastUpdate = 0;
const int DISPLAY_INTERVAL = 3000; // 3 seconds for each display mode
bool showSoilData = true;

void setup() {
    Serial.begin(115200);
    dht.begin();

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
    lcd.print("Initializing...");
    delay(1000);
    lcd.clear();
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Read and process soil moisture data
    int sensorValue1 = analogRead(SENSOR_1);
    int sensorValue2 = analogRead(SENSOR_2);
    int sensorValue3 = analogRead(SENSOR_3);
    int sensorValue4 = analogRead(SENSOR_4);
    
    moisturePercentage1 = calculateMoisturePercentage(sensorValue1, 0);
    moisturePercentage2 = calculateMoisturePercentage(sensorValue2, 1);
    moisturePercentage3 = calculateMoisturePercentage(sensorValue3, 2);
    moisturePercentage4 = calculateMoisturePercentage(sensorValue4, 3);

    // Control the relays based on moisture levels
    controlRelay(RELAY_1, moisturePercentage1);
    controlRelay(RELAY_2, moisturePercentage2);
    controlRelay(RELAY_3, moisturePercentage3);
    controlRelay(RELAY_4, moisturePercentage4);

    // Read DHT22 sensor data
    if (currentMillis - lastUpdate >= DISPLAY_INTERVAL) {
        lastUpdate = currentMillis;
        showSoilData = !showSoilData;
        lcd.clear();
        
        if (!showSoilData) {
            temperature = dht.readTemperature();
            humidity = dht.readHumidity();
        }
    }
    
    // Update LCD display
    updateLCD();
    delay(50);
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
    } else {
        digitalWrite(relayPin, HIGH); // Turn off pump
    }
}

// Function to update LCD display
void updateLCD() {
    lcd.setCursor(0, 0);
    if (showSoilData) {
        lcd.print("S1:"); lcd.print(moisturePercentage1); lcd.print("% S2:"); lcd.print(moisturePercentage2); lcd.print("%");
        lcd.setCursor(0, 1);
        lcd.print("S3:"); lcd.print(moisturePercentage3); lcd.print("% S4:"); lcd.print(moisturePercentage4); lcd.print("%");
    } else {
        lcd.print("Temp:"); lcd.print(temperature, 1); lcd.print("C");
        lcd.setCursor(0, 1);
        lcd.print("Humidity:"); lcd.print(humidity, 1); lcd.print("%");
    }
}
