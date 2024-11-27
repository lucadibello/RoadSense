
// Use Serial1 or Serial2 for GPS communication (Hardware Serial for Portenta H7)
#define GPS_BAUD 9600 // default 9600, try 4800

void setup()
{   
 // Start serial communication for debugging
  Serial.begin(115200);
  while (!Serial); // Wait for the Serial Monitor to open
  Serial1.begin(GPS_BAUD);  //Gps Serial 
  Serial.println("GPS initialized");
} 

void loop()
{  
    Serial.println("======================================================");
    while (Serial1.available() > 0)
  { 
    byte gpsData = Serial1.read(); 
   Serial.write(gpsData); 
  }
    Serial.println("\n======================================================");
    delay(4000);
}