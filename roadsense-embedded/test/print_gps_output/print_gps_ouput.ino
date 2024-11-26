
// Use Serial1 or Serial2 for GPS communication (Hardware Serial for Portenta H7)
#define GPS_BAUD 9600

void setup()
{   
 // Start serial communication for debugging
  Serial.begin(115200);
  while (!Serial); // Wait for the Serial Monitor to open
  Serial1.begin(GPS_BAUD);  //Gps Serial   
} 

void loop()
{  
    while (Serial1.available() > 0)
  { 
    byte gpsData = Serial1.read(); 
   Serial1.write(gpsData); 
  }
    delay(1000);
}