// Define the sensor pins
#define SENSOR_1 13  // ESP32 pin D13
#define SENSOR_2 12  // ESP32 pin D12
#define SENSOR_3 14  // ESP32 pin D14
#define SENSOR_4 27  // ESP32 pin D27

// Define the relay pins (active low)
#define RELAY_1 15  // ESP32 pin D13
#define RELAY_2 19  // ESP32 pin D12
#define RELAY_3 5   // ESP32 pin D14
#define RELAY_4 18  // ESP32 pin D27

// Define the dry and wet thresholds for each sensor
const int dryThresholds[] = {2000, 1800, 2000, 2000};
const int wetThresholds[] = {1300, 1400, 1200, 1500};

// Variables to store moisture percentages
int moisturePercentage1 = 0;
int moisturePercentage2 = 0;
int moisturePercentage3 = 0;
int moisturePercentage4 = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Set relay pins as output and turn them off (active low)
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, HIGH);
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
  Serial.print("Sensor 1 Moisture: ");
  Serial.print(moisturePercentage1);
  Serial.println("%");

  Serial.print("Sensor 2 Moisture: ");
  Serial.print(moisturePercentage2);
  Serial.println("%");

  Serial.print("Sensor 3 Moisture: ");
  Serial.print(moisturePercentage3);
  Serial.println("%");

  Serial.print("Sensor 4 Moisture: ");
  Serial.print(moisturePercentage4);
  Serial.println("%");

  // Control the relays based on moisture levels
  controlRelay(RELAY_1, moisturePercentage1);
  controlRelay(RELAY_2, moisturePercentage2);
  controlRelay(RELAY_3, moisturePercentage3);
  controlRelay(RELAY_4, moisturePercentage4);

  // Delay before next reading
  delay(1000);
}

// Function to calculate moisture percentage
int calculateMoisturePercentage(int sensorValue, int sensorIndex) {
  int dryThreshold = dryThresholds[sensorIndex];
  int wetThreshold = wetThresholds[sensorIndex];

  // Ensure the sensor value is within the threshold range
  sensorValue = constrain(sensorValue, wetThreshold, dryThreshold);

  // Map the sensor value to a percentage
  int moisturePercentage = map(sensorValue, dryThreshold, wetThreshold, 0, 100);

  return moisturePercentage;
}

// Function to control the relay based on moisture percentage
void controlRelay(int relayPin, int moisturePercentage) {
  if (moisturePercentage < 10) {  // If soil is dry (adjust threshold as needed)
    digitalWrite(relayPin, LOW);  // Turn on the relay (active low)
    Serial.println("Relay ON - Pumping water");
  } else {
    digitalWrite(relayPin, HIGH);  // Turn off the relay
    Serial.println("Relay OFF - Soil is wet enough");
  }
}
