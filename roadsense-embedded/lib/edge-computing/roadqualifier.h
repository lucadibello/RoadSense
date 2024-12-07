// Include necessary libraries
#include <Wire.h>             // I2C for MPU6050
#include <MPU6050.h>          // MPU6050 library by Electronic Cats
#include <TinyGPS++.h>        // TinyGPS++ library by Mikal Hart
#include <FlashIAPBlockDevice.h>
#include "FlashIAPLimits.h"


// Define constants
#define DEBUG
#define GPS_BAUD 9600         // GPS module baud rate

#define MAX_GPS_WAIT 20000    // Maximum time to wait for GPS data in milliseconds
#define CALIBRATION_TIME 20000 // Calibration time in milliseconds
#define SEGMENT_LENGTH 0.5    // Length of road segment in meters

#define MAX_INT16_VALUE 32767

using namespace mbed;

// A known signature for calibration data
static const uint32_t CALIBRATION_SIGNATURE = 0xDEADBEEF;

// Struct to store calibration data
struct CalibrationData {
  uint32_t signature;
  int32_t minZAccDifference;
  int32_t maxZAccDifference;
};

// Struct to store segment quality data
struct SegmentQuality {
  double latitude;
  double longitude;
  uint8_t quality;
};

// ===================================================== //
// ================= RoadQualifier class ================ //
// ===================================================== //

class RoadQualifier {
  public:
    // ----- API ----- //
    RoadQualifier(); // Constructor
    bool begin(); // Initialize class (returns false if failed) 
    bool isReady(); // Check if class is ready
    bool qualifySegment(); // Analyze a <SEGMENT_LENGTH>m road segment (returns false if segment is invalid)
    SegmentQuality getSegmentQuality();  // Return the quality of the last validly qualified segment (only call after qualifySegment() returns true)
                    

  private:
    // ----- Sensor objects ----- //
    MPU6050 mpu;
    TinyGPSPlus gps;
    TinyGPSCustom antennaStatus;
    TinyGPSCustom speedKmph;

    bool sensorsInitialized = false;

    // ----- Helper functions ----- //
    bool initializeMPU6050(); // Initialize MPU6050 sensor (returns false if not connected)

    bool initializeGPS(); // Initialize GPS module (returns false if not connected)
    bool isGPSAntennaConnected(); // Check if GPS antenna is connected (returns false if not connected)
    bool waitForValidLocation(); // Wait for valid GPS location (returns false if not found within MAX_GPS_WAIT)
    bool waitForValidSpeed(); // Wait for valid speed data (returns false if not found within MAX_GPS_WAIT)
    void readGPSData(); // Read GPS data from serial port
    bool updateLocation(); // Update current location after previous call to readGPSData() (returns false if not updated or invalid)
    bool updateSpeed(); // Update current speed after previous call to readGPSData() (returns false if not updated or invalid)

    uint8_t quantifyToByte(int32_t value, int32_t minValue, int32_t maxValue); // Quantify a value to a byte based on min and max values

    // ----- Calibration functions ----- //
    bool calibrate(unsigned long calibrationTime); // Calibrate accelerations (returns false if failed)

    // ----- Flash memory handling ----- //
    bool initFlashMemory(); // Initialize flash memory for calibration data (returns false if failed)
    bool loadCalibrationFromFlash(); // Load calibration data from flash memory (returns false if failed)
    bool saveCalibrationToFlash(); // Save calibration data to flash memory (returns false if failed)x

    // ----- Data ----- //
    // GPS data
    double currentLatitude = 0.0;
    double currentLongitude = 0.0;
    double segmentLatitude = 0.0;
    double segmentLongitude = 0.0;
    double currentSpeedKmph = 0.0;
    double segmentDistance = 0.0;
    // Acceleration data
    int16_t lastZAcceleration = 0;
    int16_t currentZAcceleration = 0;
    int32_t accelerationDifference = 0;
    int32_t peakSegmentZAccDifference = 0;
    int16_t dummyAcc;
    // Quality data
    uint8_t currentSegmentQuality;
    // Calibration values
    int32_t maxZAccDifference = 0;
    int32_t minZAccDifference = 0;
    // Flash memory handling
    FlashIAPBlockDevice* flashBD = nullptr;
    size_t programBlockSize;
    size_t eraseBlockSize;
    bool flashInitialized = false;
};

// ============================================================ //
// ============== Constructor and begin function ============== //
// ============================================================ //

RoadQualifier::RoadQualifier() : 
  antennaStatus(gps, "GPTXT", 4),
  speedKmph(gps, "GNVTG", 6)
{}

bool RoadQualifier::begin() {
  Serial.println("Initializing RoadQualifier...");

  if (!initializeMPU6050()) {
    Serial.println("Failed to initialize MPU6050.");
    return false;
  }

  if (!initFlashMemory()) {
    Serial.println("Failed to initialize flash memory for calibration data.");
    return false;
  }

  if (!loadCalibrationFromFlash()) {
    Serial.println("No valid calibration found. Starting calibration...");
    if (!calibrate(20000)) { // calibrate for 20 seconds
      Serial.println("Calibration failed.");
      return false;
    }
    if (!saveCalibrationToFlash()) {
      Serial.println("Failed to save calibration to flash.");
      return false;
    }
  } else {
    Serial.println("Calibration loaded from flash.");
  }

  if (!initializeGPS()) {
    Serial.println("Failed to initialize GPS module.");
    return false;
  }

  // get initial valid information from th gps
  if (!waitForValidLocation()) {
    Serial.println("Failed to acquire valid GPS location.");
    return false;
  }

  if(!waitForValidSpeed()) {
    Serial.println("Failed to acquire valid speed data.");
    return false;
  }

  sensorsInitialized = true;
  Serial.println("RoadQualifier initialized successfully.");
  Serial.print("MinZAcceleration: "); Serial.println(minZAccDifference);
  Serial.print("MaxZAcceleration: "); Serial.println(maxZAccDifference);

  return true;
}

// ===================================================== //
// ================= isReady function ================== //
// ===================================================== //

bool RoadQualifier::isReady() {
  return sensorsInitialized;
}

// ====================================================== //
// ============== Qualify Segment function ============== //
// ====================================================== //

bool RoadQualifier::qualifySegment() {
  #ifdef DEBUG
  if (!sensorsInitialized) {
    Serial.println("Sensors not initialized. Call begin() first.");
    return false;
  }
  #endif

  const float segmentTotalDistance = (float)SEGMENT_LENGTH;  
  const float first10PercentDistance = segmentTotalDistance * 0.1f; // 0.05 m

  segmentDistance = 0.0;
  peakSegmentZAccDifference = 0;

  bool segmentComplete = false;
  bool haveInitialGPSForSegment = false;
  bool haveInitialSpeedForSegment = false;
  bool segmentValid = true;

  // Use last known gps data at segment start

  mpu.getAcceleration(&dummyAcc, &dummyAcc, &lastZAcceleration);
  
  unsigned long iterationEnd = millis();

  while (!segmentComplete) {
    unsigned long iterationStart = iterationEnd; // Start time of iteration

    // Try reading GPS data
    readGPSData();

    // Update GPS data
    if(updateLocation() && (segmentDistance <= first10PercentDistance) && !haveInitialGPSForSegment) {
      // Lock onto this GPS reading for the segment start
      haveInitialGPSForSegment = true;
      segmentLatitude = currentLatitude;
      segmentLongitude = currentLongitude;
    }else if(!haveInitialGPSForSegment && segmentDistance > first10PercentDistance){
      segmentValid = false;
      break;
    }

    // Update speed data
    if(updateSpeed() && (segmentDistance <= first10PercentDistance) && !haveInitialSpeedForSegment) {
      haveInitialSpeedForSegment = true;
    }else if(!haveInitialSpeedForSegment && segmentDistance > first10PercentDistance){
      segmentValid = false;
      break;
    }

    // Update acceleration difference
    mpu.getAcceleration(&dummyAcc, &dummyAcc, &currentZAcceleration);
    int16_t diff = abs(currentZAcceleration - lastZAcceleration);
    if (diff > peakSegmentZAccDifference) {
      peakSegmentZAccDifference = diff;
    }
    lastZAcceleration = currentZAcceleration;

    // Compute distance traveled
    iterationEnd = millis();
    segmentDistance += currentSpeedKmph * (iterationEnd - iterationStart) / 3600.0f; // [km/h] * [ms] / [3600 s/h] = [m]

    if (segmentDistance >= segmentTotalDistance) {
      segmentComplete = true;
    }

    delay(10);
  }

  if (!segmentValid) {
    Serial.println("Segment invalid: Unable to get initial GPS data within the first 10% or conditions not met.");
    return false;
  }

  // Segment complete and valid
  int16_t usedMin = (minZAccDifference == maxZAccDifference) ? 0 : minZAccDifference;
  int16_t usedMax = (minZAccDifference == maxZAccDifference) ? (2 * 16384) : maxZAccDifference;
  currentSegmentQuality = quantifyToByte(peakSegmentZAccDifference, usedMin, usedMax);

  Serial.println("Segment completed:");
  Serial.print("Initial Segment Position: Lat=");
  Serial.print(segmentLatitude, 6);
  Serial.print(", Lon=");
  Serial.println(segmentLongitude, 6);
  Serial.print("Quality Measure: ");
  Serial.println(currentSegmentQuality);

  return true;
}

// returns the quality of the last validly qualified segment
// only call this function after qualifySegment() returns true
SegmentQuality RoadQualifier::getSegmentQuality() {
  return {segmentLatitude, segmentLongitude, currentSegmentQuality};
}

// ===================================================== //
// ================== Helper functions ================= //
// ===================================================== //

// Initializes MPU6050 (returns false if not connected)
bool RoadQualifier::initializeMPU6050() {
  Serial.println("Initializing MPU6050...");
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    return false;
  }

  Serial.println("MPU6050 connection successful");

  // Using previously set offsets
  mpu.setXAccelOffset(2290);
  mpu.setYAccelOffset(-2687);
  mpu.setZAccelOffset(5392);
  mpu.setXGyroOffset(35);
  mpu.setYGyroOffset(-55);
  mpu.setZGyroOffset(7);

  #ifdef DEBUG
  mpu.CalibrateAccel(6);
  mpu.CalibrateGyro(6);
  Serial.println("These are the Active offsets: ");
  mpu.PrintActiveOffsets();
  #endif

  Serial.println("MPU6050 calibrated");
  return true;
}

// Initializes GPS module (returns false if not connected)
bool RoadQualifier::initializeGPS() {
  Serial.println("Initializing GPS module...");
  Serial1.begin(GPS_BAUD);

  unsigned long start = millis();
  while (millis() - start < 5000) {
    readGPSData();
    if (antennaStatus.isUpdated()) {
      if (isGPSAntennaConnected()) {
        Serial.println("GPS antenna connected.");
        return true;
      } else {
        Serial.println("GPS antenna disconnected or not functioning.");
        return false;
      }
    }
  }

  Serial.println("GPS antenna status not available.");
  return false;
}

// Waits for valid GPS location (returns false if not found within 20 seconds)
bool RoadQualifier::waitForValidLocation() {
  Serial.println("Waiting for valid GPS location...");
  unsigned long end = millis() + MAX_GPS_WAIT;
  while (millis() < end) {
    readGPSData();
    if (gps.location.isValid() && gps.location.age() < 2000) {
      currentLatitude = gps.location.lat();
      currentLongitude = gps.location.lng();
      return true;
    }
    delay(500);
  }
  return false;
}

// Waits for valid speed data (returns false if not found within 20 seconds)
bool RoadQualifier::waitForValidSpeed() {
  Serial.println("Waiting for valid speed data...");
  unsigned long end = millis() + MAX_GPS_WAIT;
  while (millis() < end) {
    readGPSData();
    if (speedKmph.isUpdated()) {
      const char* speedStr = speedKmph.value();
      if (speedStr && speedStr[0] != '\0') {
        currentSpeedKmph = atof(speedStr); 
        return true;
      }
    }
    delay(500);
  }
  return false;
}

// Checks if GPS antenna is connected
bool RoadQualifier::isGPSAntennaConnected() {
  const char* status = antennaStatus.value();
  return (strcmp(status, "ANTENNA OK") == 0);
}

// Reads GPS data from serial port
void RoadQualifier::readGPSData() {
  while (Serial1.available() > 0) {
    char c = Serial1.read();
    gps.encode(c);
  }
}

// Updates current location after previous call to readGPSData()
// (returns false if not updated or invalid)
bool RoadQualifier::updateLocation(){
  if (!gps.location.isUpdated() || !gps.location.isValid())
    return false;
  
  currentLatitude = gps.location.lat();
  currentLongitude = gps.location.lng();

  return true;
}

// Updates current speed after previous call to readGPSData()
// (returns false if not updated or invalid)
bool RoadQualifier::updateSpeed(){
  if (!speedKmph.isUpdated() || speedKmph.value()[0] == '\0')
    return false;
  
  currentSpeedKmph = atof(speedKmph.value());
  return true;
}

uint8_t RoadQualifier::quantifyToByte(int32_t value, int32_t minValue, int32_t maxValue) {
  #ifdef DEBUG
  if (minValue >= maxValue) {
    return 0;
  }
  #endif

  if (value <= minValue) {
    return 0;
  } else if (value >= maxValue) {
    return 255;
  }

  uint32_t range = maxValue - minValue;
  uint32_t numerator = (uint32_t)(value - minValue) * 255 + (range / 2);
  uint8_t quantified = (uint8_t)(numerator / range);
  return quantified;
}

// ======================================================= //
// ================= Calibration Function ================ //
// ======================================================= //

bool RoadQualifier::calibrate(unsigned long calibrationTime) {
  Serial.println("Calibrating accelerations...");
  unsigned long endTime = millis() + calibrationTime;
  
  minZAccDifference = 0;
  maxZAccDifference = 0;

  mpu.getAcceleration(&dummyAcc, &dummyAcc, &currentZAcceleration);

  while(millis() < endTime) {

    lastZAcceleration = currentZAcceleration;
    mpu.getAcceleration(&dummyAcc, &dummyAcc, &currentZAcceleration);
    accelerationDifference = abs((int32_t)currentZAcceleration - (int32_t)lastZAcceleration);

    if (accelerationDifference > maxZAccDifference)
      maxZAccDifference = accelerationDifference;

    if ((accelerationDifference < minZAccDifference) || (minZAccDifference == 0))
      minZAccDifference = accelerationDifference;
    
    delay(5);
  }

  Serial.println("Calibration complete.");
  Serial.print("Calibrated minZAccDifference: "); Serial.println(minZAccDifference);
  Serial.print("Calibrated maxZAccDifference: "); Serial.println(maxZAccDifference);

  return true;
}

// ======================================================= //
// ============== Flash Memory Handling ================== //
// ======================================================= //

// Initializes flash memory for calibration data (returns false if failed)
bool RoadQualifier::initFlashMemory() {
  if (flashInitialized) return true;

  auto [flashSize, startAddress, iapSize] = getFlashIAPLimits();

  if (iapSize == 0) {
    Serial.println("No available flash for IAP.");
    return false;
  }

  flashBD = new FlashIAPBlockDevice(startAddress, iapSize);
  int err = flashBD->init();
  if (err != 0) {
    Serial.println("FlashIAPBlockDevice init failed.");
    return false;
  }

  programBlockSize = flashBD->get_program_size();
  eraseBlockSize = flashBD->get_erase_size();

  flashInitialized = true;
  Serial.println("Flash memory initialized successfully.");
  return true;
}

// Loads calibration data from flash memory (returns false if failed)
bool RoadQualifier::loadCalibrationFromFlash() {
  if (!flashInitialized || !flashBD) return false;

  CalibrationData calData;
  memset(&calData, 0, sizeof(CalibrationData));

  flashBD->read(&calData, 0, sizeof(CalibrationData));

  if (calData.signature == CALIBRATION_SIGNATURE) {
    minZAccDifference = calData.minZAccDifference;
    maxZAccDifference = calData.maxZAccDifference;
    Serial.println("Previously stored calibration data:");
    Serial.print("MinZAcceleration: "); Serial.println(minZAccDifference);
    Serial.print("MaxZAcceleration: "); Serial.println(maxZAccDifference);
    return true;
  }

  return false;
}

// Saves calibration data to flash memory (returns false if failed)
bool RoadQualifier::saveCalibrationToFlash() {
  if (!flashInitialized || !flashBD) return false;

  CalibrationData calData;
  calData.signature = CALIBRATION_SIGNATURE;
  calData.minZAccDifference = minZAccDifference;
  calData.maxZAccDifference = maxZAccDifference;

  size_t dataSize = sizeof(CalibrationData);
  size_t eraseBlocks = (dataSize + eraseBlockSize - 1) / eraseBlockSize;
  flashBD->erase(0, eraseBlocks * eraseBlockSize);

  size_t programSize = ((dataSize + programBlockSize - 1) / programBlockSize) * programBlockSize;
  uint8_t *buffer = (uint8_t*)malloc(programSize);
  memset(buffer, 0xFF, programSize);
  memcpy(buffer, &calData, dataSize);

  int err = flashBD->program(buffer, 0, programSize);
  free(buffer);

  if (err != 0) {
    Serial.println("Failed to program flash.");
    return false;
  }

  Serial.println("Calibration saved to flash.");
  return true;
}