# Road Bumpiness Telemetry - IoT Project

## Overview

The **Road Bumpiness Telemetry** project aims to develop an IoT device that can be mounted on cars to detect and record road vibrations. By collecting and analyzing this data from multiple vehicles, we can create a detailed heatmap that highlights the bumpiness of roads in a specific area. This information can be invaluable for road maintenance planning and improving driver safety.

## The team

- Luca Di Bello
- Georg Mayer
- Paolo Deidda

---

## Objectives

- **Develop an Arduino-based IoT device** capable of detecting road vibrations and recording positional data.
- **Implement noise reduction algorithms** to account for different vehicle baselines and reduce data inaccuracies.
- **Enable data transmission** from the IoT device to a centralized server via Wi-Fi.
- **Create a centralized server** to collect, aggregate, and analyze data from multiple devices.
- **Visualize the data** by overlaying a heatmap on a map to display road bumpiness levels.
- **Enhance data precision** by increasing the number of participating vehicles.

---

## Required Components

### Hardware

1. **Arduino Board with Wi-Fi Capability**: Arduino Portenta H7 [*datasheet*](./documentations/portenta_datasheet.pdf) [*library*](https://github.com/TheSpaceDragon/rdv-gy521) 
2. **IMU Sensor**: GY-521 with MPU6050 6DOF (3-Axis Gyro and 3-Axis Accelerometer) 
   [*datasheet_MPU650*](./documentations/imu_gy521_mpu6050_datasheet.pdf)
3. **GPS Module**: DFRobot GPS + BDS BeiDou [*wiki*](https://wiki.dfrobot.com/GPS_+_BDS_BeiDou_Dual_Module_SKU_TEL0132)
4. **Power Supply**: Rechargeable Li-Po battery with a voltage regulator (e.g., LM7805) for stable power. **OUTSTANDING**
5. **Enclosure**: Protective casing to house all components securely. **OUTSTANDING**

### Software

- **Arduino IDE**: For programming the Arduino board.
- **Server-Side Application**: Using Node.js, Python (Django or Flask), or similar frameworks.
- **Database**: MySQL, PostgreSQL, or MongoDB for data storage.
- **Mapping API**: Google Maps API or OpenStreetMap for visualization.

---

## System Architecture

1. **Data Acquisition**: The IoT device collects vibration and position data using the IMU and GPS modules.
2. **Preprocessing**: On-device algorithms filter out noise and adjust for the vehicle's baseline bumpiness.
3. **Data Transmission**: Processed data is sent directly from the IoT device to the centralized server via Wi-Fi.
4. **Data Aggregation and Analysis**: The server aggregates data from multiple devices and performs further analysis.
5. **Visualization**: A heatmap is generated and overlaid on a map to display the bumpiness levels of roads.

---

## Implementation Details

### 1. Hardware Setup

- **Microcontroller Selection**: Use an Arduino with built-in Wi-Fi.
- **Connecting Sensors**: Wire the IMU and GPS modules to the Arduino following manufacturer guidelines.
- **Power Management**: Use a voltage regulator to ensure consistent power supply from the battery.
- **Enclosure Design**: Design a compact and durable case using 3D printing or off-the-shelf enclosures.
  
#### Pin Connections

| Sensor                           | Pin on Sensor         | Arduino Portenta H7 Pin   | Description                                                                 |
|----------------------------------|-----------------------|---------------------------|-----------------------------------------------------------------------------|
| **GY-521 (MPU6050)**             | VCC                   | 3.3V                      | Power supply (3.3V)                                                          |
|                                  | GND                   | GND                       | Ground                                                                     |
|                                  | SDA                   | SDA (Pin 11)               | I2C Data line (SDA) for communication with the Arduino                       |
|                                  | SCL                   | SCL (Pin 12)               | I2C Clock line (SCL) for communication with the Arduino                      |
|                                  | XDA                   | Not connected             | Extra I2C data line (not used in standard connections)                      |
|                                  | XCL                   | Not connected             | Extra I2C clock line (not used in standard connections)                     |
|                                  | AD0                   | GND                       | I2C address selection pin (set to GND for default I2C address)              |
|                                  | INT                   | Not connected (optional)  | Interrupt pin, can be used to trigger an event in the program (optional)    |
| **DFRobot GPS + BDS BeiDou**     | VCC                   | 5V                        | Power supply (5V) for GPS module                                             |
|                                  | GND                   | GND                       | Ground                                                                     |
|                                  | TX                    | RX (Pin 13)                | Serial data transmit line (TX from GPS to RX on Portenta H7)                |
|                                  | RX                    | TX (Pin 14)                | Serial data receive line (RX from GPS to TX on Portenta H7)                 |
|                                  | SDA                   | Not connected             | I2C Data line (not used in this module, GPS communicates via UART)          |
|                                  | SCL                   | Not connected             | I2C Clock line (not used in this module, GPS communicates via UART)         |
|                                  | PPS                   | Not connected             | Pulse Per Second (optional, can be used for precise time synchronization)   |

### 2. Firmware Development

- **Sensor Calibration**: Implement routines to calibrate the IMU sensor for accurate readings.
- **Data Sampling**: Program the Arduino to sample sensor data at appropriate intervals (e.g., 50 Hz).
- **Noise Reduction**:
  - Apply a **Kalman Filter** or **Complementary Filter** to fuse accelerometer and gyroscope data.
  - Use moving averages or median filters to smooth out transient spikes.
- **Baseline Adjustment**:
  - Record initial data to establish a baseline for the vehicle's normal vibrations.
  - Adjust subsequent readings by subtracting the baseline values.
- **Wi-Fi Connectivity**:
  - Configure the device to connect to known Wi-Fi networks.
  - Implement a setup mode for users to input Wi-Fi credentials.
- **Data Packaging**: Format the data (timestamp, GPS coordinates, vibration metrics) for transmission.
- **Data Transmission**:
  - Use HTTP/HTTPS protocols to send data to the server.
  - Implement error handling and retries for network issues.

### 3. Server-Side Development

- **API Development**:
  - Create RESTful APIs for data ingestion and retrieval.
  - Implement authentication and rate limiting.
- **Database Design**:
  - Define schemas to store device data, sensor data, and aggregated results.
  - Optimize queries for real-time data access.
- **Data Processing**:
  - Aggregate data from multiple devices.
  - Use spatial indexing (e.g., R-trees) for efficient geographical queries.
- **Heatmap Generation**:
  - Calculate bumpiness scores for road segments.
  - Use mapping libraries (e.g., Leaflet, Mapbox) to overlay heatmaps.

### 4. Data Visualization

- **Web Interface**:
  - Develop a responsive web application to display the heatmap.
  - Include features like zooming, panning, and road selection.
- **Data Layers**:
  - Allow users to filter data by time ranges or bumpiness levels.
  - Provide statistical summaries for selected areas.

### 5. Testing and Validation

- **Field Testing**:
  - Install devices in different vehicle types (sedans, SUVs, trucks).
  - Collect data over various road conditions.
- **Algorithm Tuning**:
  - Adjust filtering parameters based on test results.
  - Validate bumpiness scores against known road conditions.
- **User Feedback**:
  - Incorporate feedback from initial users to improve the system.

---

## Data Processing and Noise Reduction

### Calibration Period

- **Initial Baseline**: Collect data for the first few minutes to establish the vehicle's normal vibration patterns.
- **Dynamic Adjustment**: Continuously update the baseline to account for changes (e.g., vehicle load, tire pressure).

### Filtering Techniques

- **Low-Pass Filter**: Removes high-frequency noise unrelated to road conditions.
- **High-Pass Filter**: Eliminates low-frequency movements like vehicle tilts.
- **Band-Pass Filter**: Focuses on frequencies that correspond to typical road-induced vibrations.

### Statistical Methods

- **Standard Deviation and Variance**: Measure the dispersion of vibration data.
- **Peak Detection**: Identify significant deviations that indicate bumps or potholes.
- **Thresholding**: Set vibration intensity thresholds to categorize road conditions.

### Edge Computing

- **Data Compression**: Reduce data size by transmitting only significant events.
- **Event Detection**: Implement on-device logic to detect and report only when anomalies occur.
- **Power Efficiency**: Optimize code to reduce processor load and conserve battery life.

---

## Possible problems / solutions

### Network Connectivity

- **Wi-Fi Availability**:
  - **Challenge**: Continuous Wi-Fi connectivity may not be available during travel.
  - **Solution**: Store data locally on the device and transmit when Wi-Fi is available.
  - **Alternative**: Use a mobile hotspot or integrate a GSM module for cellular data.
- **Data Security**:
  - Encrypt data transmissions to prevent eavesdropping or tampering.

### Vehicle Variability

- **Machine Learning Models**: Train models to recognize and adjust for different vehicle characteristics.
- **User Input**: Allow users to specify vehicle type during device setup for better calibration.

### Power Management

- **Sleep Modes**: Implement low-power modes when the vehicle is not moving.
- **Efficient Components**: Use low-power sensors and modules where possible.

### Data Volume Management

- **Data Sampling**: Reduce sampling rate without compromising data quality.
- **Selective Reporting**: Transmit only aggregated or significant data points.

---

## Possible enhancements (to be discussed)

- **Advanced Analytics**: Use AI to predict road degradation over time.
- **Real-Time Alerts**: Notify drivers of upcoming road hazards.

---
