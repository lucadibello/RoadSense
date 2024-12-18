#include <TinyGPS++.h>       // TinyGPS++ library for parsing GPS data
#include <Wire.h>

/*
15:49:43.783 -> -----------------------------------
15:49:44.801 -> Latitude: 46.012980, Longitude: 8.960949
15:49:44.801 -> Date (DD/MM/YYYY): 18/12/2024
15:49:44.801 -> Time (HH:MM:SS): 14:49:44
15:49:44.801 -> Speed: 1.72 km/h
15:49:44.801 -> -----------------------------------
15:49:45.788 -> Latitude: 46.012979, Longitude: 8.960957
15:49:45.788 -> Date (DD/MM/YYYY): 18/12/2024
15:49:45.788 -> Time (HH:MM:SS): 14:49:45
15:49:45.788 -> Speed: 1.72 km/h
15:49:45.788 -> -----------------------------------
15:49:46.808 -> Latitude: 46.012977, Longitude: 8.960970
15:49:46.808 -> Date (DD/MM/YYYY): 18/12/2024
15:49:46.808 -> Time (HH:MM:SS): 14:49:46
15:49:46.808 -> Speed: 1.72 km/h
15:49:46.808 -> -----------------------------------
15:49:47.797 -> Latitude: 46.012974, Longitude: 8.960980
15:49:47.797 -> Date (DD/MM/YYYY): 18/12/2024
15:49:47.797 -> Time (HH:MM:SS): 14:49:47
15:49:47.797 -> Speed: 1.72 km/h
15:49:47.797 -> -----------------------------------
*/

// Create a TinyGPSPlus object
TinyGPSPlus gps;

// Create a TinyGPSCustom object to parse the antenna status from the GPTXT sentence
TinyGPSCustom antennaStatus(gps, "GPTXT", 4);  // Field index 4 contains the antenna status

// Define LED pin (use built-in LED)
#define LED_PIN LED_BUILTIN

// Function prototypes
bool isGPSConnected();
bool isGPSAntennaConnected();
bool isGPSWorkingCorrectly();
bool hasSatelliteConnection();
bool hasGoodSignal();
bool waitForSatellite();
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

  // Check for hardware faults
  if (!isGPSWorkingCorrectly()) {
    Serial.println("GPS is not working correctly or antenna is disconnected.");
    // Halt execution until hardware was fixed -> maybe restart periodically with watchdog
  } else {
    Serial.println("GPS is working correctly and antenna is connected.");
  }

  // Wait until Satellite connection is found
  while(true) {
    if (waitForSatellite()) {
      Serial.println("Valid Satellite connection acquired.");
      break;
    } else {
      Serial.println("Failed Satellite connection.");
      Serial.println("Trying again...");
    }
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
    digitalWrite(LED_PIN, LOW);  // LED ON
  } else {
    digitalWrite(LED_PIN, HIGH);   // LED OFF
  }
}

// Function to check if GPS module is connected (has a fix)
bool isGPSConnected() {
  return gps.location.isValid() && gps.location.age() < 2000;  // Age in milliseconds
}

// Check for physical Antenna Connection
bool isGPSAntennaConnected() {
  const char* status = antennaStatus.value();
  if (strcmp(status, "ANTENNA OK") == 0) {
    return true;
  }
  return false;
}

// Check for Satelite connection
bool hasSatelliteConnection() {
  return gps.satellites.isValid() && gps.satellites.value() > 0;
}

// Check for good signal quality (HDOP)
bool hasGoodSignal() {
  return gps.hdop.isValid() && gps.hdop.hdop() < 10.0;
}
// Function to check if GPS is working correctly and antenna is connected
bool isGPSWorkingCorrectly() {
  // Check antenna status
  bool antennaConnected = false;

  // Wait for antenna status message (-> atleast one complete transmission has finished)
  unsigned long start = millis();
  while (millis() - start < 5000) {  // Wait up to 5 seconds
    while (Serial1.available() > 0) {
      char c = Serial1.read();
      gps.encode(c);
    }
    
    if (antennaStatus.isUpdated()) {
      antennaConnected = isGPSAntennaConnected();
      break;
    }
  }

  return antennaConnected;
}

// Function to wait until a Satellite connection was found
bool waitForSatellite() {
  Serial.println("Waiting for Satellite connection...");
  unsigned long start = millis();
  while (millis() - start < 60000) {  // Wait up to 60 seconds
    while (Serial1.available() > 0) {
      char c = Serial1.read();
      gps.encode(c);
    }

    if (hasSatelliteConnection() && hasGoodSignal()) {
      return true;  // Valid Satellite connection
    }

    delay(500);  // Small delay before checking again
  }
  return false;  // Failed to get valid location within timeout
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