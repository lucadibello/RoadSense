#include <TinyGPS++.h>       // TinyGPS++ library for parsing GPS data
#include <Wire.h>

// Create a TinyGPSPlus object
TinyGPSPlus gps;

// Create a TinyGPSCustom object to parse the antenna status from the GPTXT sentence
TinyGPSCustom antennaStatus(gps, "GPTXT", 4);  // Field index 4 contains the antenna status

// Define LED pin (use built-in LED)
#define LED_PIN LED_BUILTIN

// Function prototypes
bool isGPSConnected();
bool isGPSWorkingCorrectly();
bool waitForValidLocation();
void signalSetupProcess(bool status);

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  // Initialize serial communication with GPS module (adjust as needed)
  Serial1.begin(9600);

  // Initialize LED pin as OUTPUT
  pinMode(LED_PIN, OUTPUT);

  // Wait for the serial port to open
  while (!Serial);
  Serial.println("GPS Module Initialization Started");

  // Signal start of setup (LED ON)
  signalSetupProcess(true);

  // Check if GPS is working correctly and antenna is connected
  if (!isGPSWorkingCorrectly()) {
    Serial.println("GPS is not working correctly or antenna is disconnected.");
    // You can choose to halt or continue based on your application
    // For this code, we'll attempt to proceed and wait for valid data
  } else {
    Serial.println("GPS is working correctly and antenna is connected.");
  }

  // Wait until a valid location is found
  if (waitForValidLocation()) {
    Serial.println("Valid GPS location acquired.");
  } else {
    Serial.println("Failed to acquire valid GPS location.");
  }

  // Signal end of setup (LED OFF)
  signalSetupProcess(false);

  Serial.println("GPS Module Initialization Completed");
}

void loop() {
  // Read data from GPS module
  while (Serial1.available() > 0) {
    char c = Serial1.read();
    gps.encode(c);
  }

  // Check if GPS data is valid
  if (gps.location.isUpdated() && gps.location.isValid()) {
    // Print location
    Serial.print("Latitude: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(", Longitude: ");
    Serial.println(gps.location.lng(), 6);

    // Print date and time
    if (gps.date.isValid() && gps.time.isValid()) {
      Serial.print("Date (DD/MM/YYYY): ");
      Serial.print(gps.date.day());
      Serial.print("/");
      Serial.print(gps.date.month());
      Serial.print("/");
      Serial.println(gps.date.year());

      Serial.print("Time (HH:MM:SS): ");
      Serial.print(gps.time.hour());
      Serial.print(":");
      Serial.print(gps.time.minute());
      Serial.print(":");
      Serial.println(gps.time.second());
    } else {
      Serial.println("Date/Time data is invalid.");
    }

    // Print speed in km/h
    if (gps.speed.isValid()) {
      Serial.print("Speed: ");
      Serial.print(gps.speed.kmph());
      Serial.println(" km/h");
    } else {
      Serial.println("Speed data is invalid.");
    }

    Serial.println("-----------------------------------");
  } else {
    Serial.println("Waiting for valid GPS data...");
  }

  // Optional: Check antenna status during operation
  if (antennaStatus.isUpdated()) {
    const char* status = antennaStatus.value();

    Serial.print("Antenna Status Message: ");
    Serial.println(status);

    if (strcmp(status, "ANTENNA OK") == 0) {
      Serial.println("Antenna is connected and functioning properly.");
    } else if (strcmp(status, "ANTENNA OPEN") == 0) {
      Serial.println("Antenna is disconnected or not functioning.");
    } else {
      Serial.println("Unknown antenna status.");
    }
  }

  delay(1000);  // Wait before checking again
}

// Function to signal setup process using LED
void signalSetupProcess(bool status) {
  if (status) {
    digitalWrite(LED_PIN, HIGH);  // LED ON
  } else {
    digitalWrite(LED_PIN, LOW);   // LED OFF
  }
}

// Function to check if GPS module is connected (has a fix)
bool isGPSConnected() {
  return gps.location.isValid() && gps.location.age() < 2000;  // Age in milliseconds
}

// Function to check if GPS is working correctly and antenna is connected
bool isGPSWorkingCorrectly() {
  // Check antenna status
  bool antennaConnected = false;

  // Wait for antenna status message
  unsigned long start = millis();
  while (millis() - start < 5000) {  // Wait up to 5 seconds
    while (Serial1.available() > 0) {
      char c = Serial1.read();
      gps.encode(c);
    }

    if (antennaStatus.isUpdated()) {
      const char* status = antennaStatus.value();
      if (strcmp(status, "ANTENNA OK") == 0) {
        antennaConnected = true;
        break;
      } else if (strcmp(status, "ANTENNA OPEN") == 0) {
        antennaConnected = false;
        break;
      }
    }
  }

  // Check for valid satellite data
  bool hasSatellites = gps.satellites.isValid() && gps.satellites.value() > 0;

  // Check HDOP (signal quality)
  bool hasGoodSignal = gps.hdop.isValid() && gps.hdop.hdop() < 10.0;

  return antennaConnected && hasSatellites && hasGoodSignal;
}

// Function to wait until a valid location is found
bool waitForValidLocation() {
  Serial.println("Waiting for valid GPS location...");
  unsigned long start = millis();
  while (millis() - start < 60000) {  // Wait up to 60 seconds
    while (Serial1.available() > 0) {
      char c = Serial1.read();
      gps.encode(c);
    }

    if (gps.location.isValid() && gps.location.age() < 2000) {
      return true;  // Valid location found
    }

    delay(500);  // Small delay before checking again
  }
  return false;  // Failed to get valid location within timeout
}