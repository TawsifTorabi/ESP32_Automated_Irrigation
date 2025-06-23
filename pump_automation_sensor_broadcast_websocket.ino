#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WebSocketsServer.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Define the sensor pins
#define SENSOR_1 34
#define SENSOR_2 35
#define SENSOR_3 32
#define SENSOR_4 33
#define DHTPIN 4  
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

// WebSocket server on port 81
WebSocketsServer webSocket(81);

// Timing variables
unsigned long lastUpdate = 0;
const int DISPLAY_INTERVAL = 3000; 
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
    lcd.print("Connecting WiFi...");
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    // Get IP address and display it
    String ipAddress = WiFi.localIP().toString();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP:");
    lcd.setCursor(0, 1);
    lcd.print(ipAddress);
    delay(3000);
    lcd.clear();

    // Start WebSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("WebSocket server started!");
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Read and process soil moisture data
    moisturePercentage1 = calculateMoisturePercentage(readSoilMoisture(SENSOR_1), 0);
    moisturePercentage2 = calculateMoisturePercentage(readSoilMoisture(SENSOR_2), 1);
    moisturePercentage3 = calculateMoisturePercentage(readSoilMoisture(SENSOR_3), 2);
    moisturePercentage4 = calculateMoisturePercentage(readSoilMoisture(SENSOR_4), 3);

    // Control the relays based on moisture levels
    controlRelay(RELAY_1, moisturePercentage1);
    controlRelay(RELAY_2, moisturePercentage2);
    controlRelay(RELAY_3, moisturePercentage3);
    controlRelay(RELAY_4, moisturePercentage4);

    // Read DHT22 sensor data every 3 sec
    if (currentMillis - lastUpdate >= DISPLAY_INTERVAL) {
        lastUpdate = currentMillis;
        showSoilData = !showSoilData;

        temperature = dht.readTemperature();
        humidity = dht.readHumidity();
        
        // Send data via WebSocket
        sendSensorData();
        
        lcd.clear();
    }

    // Update LCD display
    updateLCD();

    // Handle WebSocket clients
    webSocket.loop();

    delay(50);
}

// Function to read soil moisture with delay
int readSoilMoisture(int pin) {
    delay(10);  // Allow ADC to stabilize
    return analogRead(pin);
}

// Function to calculate moisture percentage
int calculateMoisturePercentage(int sensorValue, int sensorIndex) {
    int dryThreshold = dryThresholds[sensorIndex];
    int wetThreshold = wetThresholds[sensorIndex];

    sensorValue = constrain(sensorValue, wetThreshold, dryThreshold);
    return map(sensorValue, dryThreshold, wetThreshold, 0, 100);
}

// Function to control the relay based on moisture percentage
void controlRelay(int relayPin, int moisturePercentage) {
    digitalWrite(relayPin, moisturePercentage < 10 ? LOW : HIGH);
}

// Function to update LCD display
void updateLCD() {
    lcd.setCursor(0, 0);
    if (showSoilData) {
        lcd.print("S1:"); lcd.print(moisturePercentage1); lcd.print("% ");
        lcd.print("S2:"); lcd.print(moisturePercentage2); lcd.print("%");
        lcd.setCursor(0, 1);
        lcd.print("S3:"); lcd.print(moisturePercentage3); lcd.print("% ");
        lcd.print("S4:"); lcd.print(moisturePercentage4); lcd.print("%");
    } else {
        lcd.print("Temp:"); lcd.print(temperature, 1); lcd.print("C ");
        lcd.setCursor(0, 1);
        lcd.print("Humidity:"); lcd.print(humidity, 1); lcd.print("%");
    }
}

// Function to send sensor data via WebSocket
void sendSensorData() {
    String jsonData = "{";
    jsonData += "\"temperature\":" + String(temperature, 1) + ",";
    jsonData += "\"humidity\":" + String(humidity, 1) + ",";
    jsonData += "\"moisture\":[";
    jsonData += String(moisturePercentage1) + ",";
    jsonData += String(moisturePercentage2) + ",";
    jsonData += String(moisturePercentage3) + ",";
    jsonData += String(moisturePercentage4);
    jsonData += "]}";

    Serial.println("Broadcasting: " + jsonData);
    webSocket.broadcastTXT(jsonData);
}

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            Serial.printf("Client [%u] connected\n", num);
            break;
        case WStype_DISCONNECTED:
            Serial.printf("Client [%u] disconnected\n", num);
            break;
        case WStype_TEXT:
            Serial.printf("Received from [%u]: %s\n", num, payload);
            break;
    }
}
