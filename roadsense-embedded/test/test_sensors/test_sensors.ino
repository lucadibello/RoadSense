#include <Wire.h>             // I2C communication library for MPU6050 sensor 
                              // -> Built-in library in Arduino IDE
#include <MPU6050.h>          // MPU6050 library for reading accelerometer and gyroscope data
                              // -> Install from Arduino Library Manager (Search for "MPU6050 by Electornic Cats")
#include <TinyGPS++.h>        // TinyGPS++ library for parsing GPS data
                              // Install from Arduino Library Manager (Search for "TinyGPSPlus" by Mikal Hart)

// Use Serial1 or Serial2 for GPS communication (Hardware Serial for Portenta H7)
#define GPS_BAUD 9600

// Create instances for MPU6050 and GPS
MPU6050 mpu;
TinyGPSPlus gps; // GPS object

// Define data structure to hold sensor measurements
struct SensorData {
  String date;
  String time;
  double latitude;
  double longitude;
  float velocity;
  float ax, ay, az; // Accelerations in all axes
  float gx, gy, gz; // Gyroscopic angular velocities in all axes
};

// Initialize sensor readings
SensorData sensorData;

void setup() {
  // Define status for debugging purposes
  int devStatus = 0;

  // Start serial communication for debugging
  Serial.begin(115200);
  while (!Serial); // Wait for the Serial Monitor to open

  // ============= Initialize MPU6050 (GY-521) =============
  Wire.begin();
  mpu.initialize();
  
  // Check if the MPU6050 is connected
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1); // Halt the program
  } else {
    Serial.println("MPU6050 connection successful");
  }

  /* Supply your gyro offsets here, scaled for min sensitivity */
  // Offstes form running EC MPU6050 Lib example: MPU650_zero.ino
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);

  /* Making sure it worked (returns 0 if so) */ 
  if (devStatus == 0) {
    mpu.CalibrateAccel(6);  // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.CalibrateGyro(6);
    Serial.println("These are the Active offsets: ");
    mpu.PrintActiveOffsets();
  }

  Serial.println("MPU6050 Initialized");

  // ============

  // Initialize GPS communication with Serial1
  Serial1.begin(GPS_BAUD);
  Serial.println("GPS Initialized");

  // Allow time for sensors to initialize
  delay(2000);
}

void loop() {
  // Read MPU6050 data (accelerations and gyroscope)
  int16_t axRaw, ayRaw, azRaw, gxRaw, gyRaw, gzRaw;
  mpu.getAcceleration(&axRaw, &ayRaw, &azRaw);
  mpu.getRotation(&gxRaw, &gyRaw, &gzRaw);

  // Convert accelerometer readings to g (1g = 9.81 m/s^2)
  sensorData.ax = axRaw / 16384.0;  // 16384 is the scale for ±2g range
  sensorData.ay = ayRaw / 16384.0;
  sensorData.az = azRaw / 16384.0;

  // Convert gyroscope readings to degrees per second
  sensorData.gx = gxRaw / 131.0;  // 131 is the scale for ±250°/s range
  sensorData.gy = gyRaw / 131.0;
  sensorData.gz = gzRaw / 131.0;

  // Read GPS data
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }

  // If GPS fix is available, store the position and velocity
  if (gps.location.isUpdated()) {
    sensorData.latitude = gps.location.lat();
    sensorData.longitude = gps.location.lng();
  }

  if (gps.speed.isUpdated()) {
    sensorData.velocity = gps.speed.kmph();  // Velocity in km/h
  }

  // Get current date and time from GPS
  if (gps.date.isUpdated() && gps.time.isUpdated()) {
    sensorData.date = String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year());
    sensorData.time = String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());
  }

  // Print the sensor measurements to the Serial Monitor
  printSensorData(sensorData);

  // Wait before next reading (set to 1 second)
  delay(1000);
}

// Function to print sensor data in a nice format
void printSensorData(SensorData data) {
  Serial.println("====================================");
  Serial.println("Measurement Data:");
  Serial.print("Date: ");
  Serial.println(data.date);
  Serial.print("Time: ");
  Serial.println(data.time);
  Serial.print("GPS Coordinates: ");
  Serial.print("Lat: ");
  Serial.print(data.latitude, 6);
  Serial.print(", Lon: ");
  Serial.println(data.longitude, 6);
  Serial.print("Velocity: ");
  Serial.print(data.velocity);
  Serial.println(" km/h");

  Serial.print("Acceleration (g): ");
  Serial.print("X: ");
  Serial.print(data.ax, 3);
  Serial.print(", Y: ");
  Serial.print(data.ay, 3);
  Serial.print(", Z: ");
  Serial.println(data.az, 3);

  Serial.print("Gyroscopic Angular Velocity (°/s): ");
  Serial.print("X: ");
  Serial.print(data.gx, 2);
  Serial.print(", Y: ");
  Serial.print(data.gy, 2);
  Serial.print(", Z: ");
  Serial.println(data.gz, 2);

  Serial.println("====================================");
}