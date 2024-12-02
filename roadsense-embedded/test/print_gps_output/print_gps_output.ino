/*
Example output od DF-ROBOT GPS+BEIDOU
$GNGGA,095347.000,4600.71957,N,00857.65470,E,1,11,1.2,320.9,M,46.9,M,,*4B
$GNGLL,4600.71957,N,00857.65470,E,095347.000,A,A*4E
$GNGSA,A,3,05,13,,,,,,,,,,,2.2,1.2,1.9,1*3D
$GNGSA,A,3,07,10,14,24,25,26,33,41,42,,,,2.2,1.2,1.9,4*3A
$GPGSV,2,1,05,05,59,245,28,07,22,061,,08,06,040,,13,61,306,26,0*6D
$GPGSV,2,2,05,30,56,059,,0*5C
$BDGSV,3,1,10,07,18,069,23,10,21,082,22,14,48,143,25,24,70,078,28,0*73
$BDGSV,3,2,10,25,47,211,28,26,21,050,24,33,76,164,30,40,,,25,0*4E
$BDGSV,3,3,10,41,47,307,25,42,22,138,23,0*7D
$GNRMC,095347.000,A,4600.71957,N,00857.65470,E,0.00,302.96,021224,,,A,V*0A
$GNVTG,302.96,T,,M,0.00,N,0.00,K,A*2D
$GNZDA,095347.000,02,12,2024,00,00*41
$GPTXT,01,01,01,ANTENNA OK*35

const char *gpsStream =
  "$GNGGA,095347.000,4600.71957,N,00857.65470,E,1,11,1.2,320.9,M,46.9,M,,*4B\r\n"
  "$GNGLL,4600.71957,N,00857.65470,E,095347.000,A,A*4E\r\n"
  "$GNGSA,A,3,05,13,,,,,,,,,,,2.2,1.2,1.9,1*3D\r\n"
  "$GNGSA,A,3,07,10,14,24,25,26,33,41,42,,,,2.2,1.2,1.9,4*3A\r\n"
  "$GPGSV,2,1,05,05,59,245,28,07,22,061,,08,06,040,,13,61,306,26,0*6D\r\n"
  "$GPGSV,2,2,05,30,56,059,,0*5C\r\n"
  "$BDGSV,3,1,10,07,18,069,23,10,21,082,22,14,48,143,25,24,70,078,28,0*73\r\n"
  "$BDGSV,3,2,10,25,47,211,28,26,21,050,24,33,76,164,30,40,,,25,0*4E\r\n"
  "$BDGSV,3,3,10,41,47,307,25,42,22,138,23,0*7D\r\n"
  "$GNRMC,095347.000,A,4600.71957,N,00857.65470,E,0.00,302.96,021224,,,A,V*0A\r\n"
  "$GNVTG,302.96,T,,M,0.00,N,0.00,K,A*2D\r\n"
  "$GNZDA,095347.000,02,12,2024,00,00*41\r\n"
  "$GPTXT,01,01,01,ANTENNA OK*35\r\n";

*/


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
    //Serial.println("======================================================");
    while (Serial1.available() > 0)
  { 
    byte gpsData = Serial1.read(); 
   Serial.write(gpsData); 
  }
    //Serial.println("\n======================================================");
    //delay(500);
}