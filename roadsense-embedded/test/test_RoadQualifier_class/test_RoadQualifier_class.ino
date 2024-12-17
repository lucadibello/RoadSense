#include "roadqualifier.h"

// Define constants
#define DELETE_FLASH
#define SERIAL_BAUD 115200         // Serial baud rate

RoadQualifier roadQualifier;
SegmentQuality segmentQuality;

void setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial);

  #ifdef DELETE_FLASH
    if(!roadQualifier.deleteCalibrationFromFlash()) {
      Serial.println("Failed to delete calibration from flash.");
    } else {
      Serial.println("Calibration deleted from flash.");
    }
  #endif

  while(true){
    if (roadQualifier.begin()) {
      Serial.println("Road Quality Qualifier is ready.");
      break;
    } else {
      Serial.println("Failed to initialize Road Quality Qualifier. Retrying!!!");
      delay(5000);
      // Handle initialization failure (e.g., halt or retry)
    }
  }
}

void loop() {
  // Call the qualifySegment() function to analyze each 0.5m segment
  roadQualifier.qualifySegment();
  segmentQuality = roadQualifier.getSegmentQuality();
}