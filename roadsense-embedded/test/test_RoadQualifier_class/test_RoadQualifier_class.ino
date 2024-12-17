#include "roadqualifier.h"

// Define constants
#define SERIAL_BAUD 115200         // Serial baud rate

RoadQualifier roadQualifier;
SegmentQuality segmentQuality;

void setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial);

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