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

1. **Arduino Board with Wi-Fi Capability**: Use an Arduino Uno with a Wi-Fi module or an Arduino Nano 33 IoT for processing and connectivity.
2. **Wi-Fi Module**: ESP8266 or ESP32 if using a board without built-in Wi-Fi.
3. **IMU Sensor**: MPU-6050 or MPU-9250 for accelerometer and gyroscope data.
4. **GPS Module**: Ublox NEO-6M GPS module for accurate positioning.
5. **Power Supply**: Rechargeable Li-Po battery with a voltage regulator (e.g., LM7805) for stable power.
6. **Enclosure**: Protective casing to house all components securely.

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

- **Microcontroller Selection**: Use an Arduino with built-in Wi-Fi or add an ESP8266/ESP32 module.
- **Connecting Sensors**: Wire the IMU and GPS modules to the Arduino following manufacturer guidelines.
- **Power Management**: Use a voltage regulator to ensure consistent power supply from the battery.
- **Enclosure Design**: Design a compact and durable case using 3D printing or off-the-shelf enclosures.

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
