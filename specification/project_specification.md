# RoadSense - requirements and specifications

## Requirements
- Create an up-to-date map with in-depth qualification of road bumpiness and hazards.
- Visualization of road conditions through a heatmap overlay on a map, with markings for potholes.
- The map must be interactive, enabling stakeholders to visualize road conditions and engage with generated alerts. This includes actions such as viewing all alerts and marking alerts as resolved.
- Distinguishing between speed bumps, manholes, road markings, and potholes in urban areas.
- Relies on data collected from multiple vehicles to ensure accuracy and coverage.
- Detects road vibrations and relates them to positional data to pinpoint road conditions.
- Scalable and cost-effective solution for widespread adoption.
- Centralized server to collect, aggregate, and analyze data from multiple devices.
- Optimized data transmission from IoT devices to the server for real-time updates.
- Noise reduction algorithms to account for different vehicle baselines and reduce data inaccuracies.
- Data transmission from the IoT device to the centralized server via Wi-Fi when in range of dedicated hotspots.
- Both battery-powered and vehicle-powered IoT devices to
- The primary energy source is the vehicle's power supply, with a backup battery to ensure data transmission when the vehicle is off.
- A compact casing should securely enclose all components, shielding them from external elements. Additionally, it should feature LEDs to indicate device status, such as transmission activity or errors.

## High-Level Specification
The **RoadSense system** will detect road anomalies through IoT devices placed on multiple vehicles. The collected data will be transmitted to a centralized server that processes, aggregates, and visualizes it on an interactive map. The heatmap will provide stakeholders with an overview of road conditions, allowing them to focus on specific road segments for maintenance.

### System Components
- **Sensor Nodes**: Responsible for collecting vibration data using an Inertial Measurement Unit (IMU) sensor and location data via a GPS module.
- **Actuator Nodes**: Will perform actions like transmitting data to the server and managing device power.
- **Server-Side Application**: Will aggregate data from multiple sensor nodes, analyze it, and visualize the road quality using heatmaps.
- **Control Logic**: Defines the behavior of the IoT device in terms of data collection, processing, and communication.

## System Architecture
The **RoadSense system** consists of multiple IoT devices (sensor nodes) installed in vehicles, which communicate with a central server. The system architecture is composed of:

1. **Sensor Nodes**: Each vehicle will have a sensor node equipped with an IMU sensor and GPS module to collect vibration and positional data. The data will be processed on the device using filtering techniques to reduce noise.
2. **Data Transmission**: The processed data will be sent to a central server via Wi-Fi when the vehicle is within range of hotspots.
3. **Central Server**: Aggregates data from multiple vehicles, applies further filtering and analysis, and generates a heatmap overlay to display road bumpiness.
4. **User Interface**: Stakeholders can interact with the heatmap through a web interface to explore road conditions and manage alerts.

## Sensor Nodes
Each **sensor node** consists of an Arduino board with an integrated Wi-Fi module, an IMU sensor for measuring road vibrations, and a GPS module for precise location tracking. The sensor node will collect data continuously while the vehicle is in motion, applying noise reduction techniques such as Kalman filtering or Complementary filtering to ensure accurate readings.

## Actuator Nodes
The **actuator nodes** will handle data transmission and power management. They will ensure the sensor node remains operational and transmit data to the central server when Wi-Fi connectivity is available. Actuator nodes will also manage backup power to ensure data transmission in case the primary vehicle power source is unavailable.

## Server-Side Application
The **server-side application** will be responsible for receiving, storing, and analyzing the data sent from the IoT devices. It will include a database to store sensor data and an analysis engine to process the data for generating road quality insights. The server will also be responsible for managing user interactions through a web interface, allowing stakeholders to view the heatmap and handle alerts.

## Control Logic
The **control logic** for the RoadSense system includes data collection from the IMU and GPS sensors, real-time processing to adjust for noise and vehicle-specific baselines, and transmitting data to the server when within Wi-Fi range. Additionally, the system will monitor the device's power state and switch to battery power when necessary.