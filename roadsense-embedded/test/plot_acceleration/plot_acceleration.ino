#include <Wire.h>
#include <MPU6050.h>  // Include the MPU6050 library by Electronic Cats

MPU6050 mpu;  // Create an MPU6050 object

void setup() {
  int devStatus=0;

  // Initialize serial communication at 115200 baud for debugging and Serial Plotter
  Serial.begin(115200);
  while (!Serial);  // Wait for the serial port to open (for boards like Leonardo/Micro)

  // Initialize I2C communication
  Wire.begin();

  // Initialize the MPU6050 sensor
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
 
  mpu.setXAccelOffset(2290);
  mpu.setYAccelOffset(-2687);
  mpu.setZAccelOffset(5392);
  mpu.setXGyroOffset(35);
  mpu.setYGyroOffset(-55);
  mpu.setZGyroOffset(7);
  /* Making sure it worked (returns 0 if so) */ 
  if (devStatus == 0) {
    mpu.CalibrateAccel(6);  // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.CalibrateGyro(6);
    Serial.println("These are the Active offsets: ");
    mpu.PrintActiveOffsets();
  }

  Serial.println("MPU6050 Initialized");

  Serial.println("MPU6050 initialized successfully!");
}

void loop() {
  int16_t gravityCorrection = -16384;
  // Read MPU6050 data (accelerations and gyroscope)
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Convert accelerometer readings to g (1g = 9.81 m/s^2)
  //ax = ax / 16384.0;  // 16384 is the scale for ±2g range
  //ay = ay / 16384.0;
  //az = az / 16384.0;

  // Print acceleration values for Serial Plotter
  Serial.print("ax:");
  Serial.print(ax);
  Serial.print(",");
  Serial.print("ay:");
  Serial.print(ay);
  Serial.print(",");
  Serial.print("az:");
  Serial.println(az+gravityCorrection);

  delay(50);  // Adjust the delay as needed
}