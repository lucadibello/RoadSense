# RoadSense - Requirements and Specifications

The **RoadSense** project aims to develop an IoT-based system to detect and map road anomalies such as bumpiness and potholes. By installing sensor nodes on multiple vehicles, the system collects and analyzes road vibration data to create a detailed, interactive heatmap of road conditions. This information is invaluable for road maintenance planning, improving driver safety, and providing real-time alerts for hazardous conditions.

The team consists of three members: Luca Di Bello, Georg Mayer, and Paolo Deidda.

## Objectives

This project aims to address the following objectives:

1. Develop a cost-effective IoT-based system to detect and map road anomalies.
2. Quantify road bumpiness levels and provide real-time alerts for hazardous conditions.
3. Data transmission at established access points to reduce communication overhead.
4. Centralized data management for efficient aggregation and analysis.
5. User-friendly interface for stakeholders to visualize road conditions and manage alerts.
6. Improve the accuracy of road conditons by using multiple vehicles for data collection.

## Requirements

From the objectives outlined in the previous section, the team had identified the following requirements for the **RoadSense** project:

- **Road heatmap**: Create an interactive web application that displays road conditions using a heatmap overlay (lighter colors for smoother roads, darker colors for rougher roads).

- **Alerting system**: The system should automatically create alerts when hazardous road conditions are detected and allow users to view, acknowledge, and resolve these alerts. Users should be also able to manually add alerts on the map.

- **Anomaly Differentiation**: Distinguish between normal road features (i.e. speed bumps, potholes, manholes, etc) and hazardous road conditions.

- **Continuous system calibration**: Each IoT device should be calibrated to the vehicle it is installed in, taking into account the vehicle's characteristics and driving conditions. This calibration should be simple and possibly automatically. This can be archieved by a first calibration phase coupled to an initial parameter set.

- **Scalability**: The system should be highly scalable to potentially handle data from thousands of vehicles simultaneously.

- **Centralized Data Management**: The server should be able to aggregate data from multiple IoT devices and store it efficiently for later analysis and visualization.

- **Optimized Data Transmission**: Since continuous data transmission can be costly for the user, the system should transmit data only when the vehicle is at established access points (e.g., Wi-Fi hotspots).

- **Power Supply**: The system should be designed to operate using the vehicle's power source primarily, with a small backup battery to ensure that the data transmission is not interrupted if the vehicle is turned off.

- **Durable Casing**: The sensor nodes should be robust and weatherproof to withstand various driving conditions.

## System design

The **RoadSense** system consists of the following components:

- **Sensor Nodes**: IoT devices installed in vehicles, responsible for collecting vibration data using an Inertial Measurement Unit (IMU) sensor and location data via a GPS module.

- **Actuator Nodes**: Handle data transmission to the server and manage device power.

- **Server-Side Application**: Collects, aggregates, and analyzes data from multiple devices, and visualizes road quality using heatmaps.

- **Control Logic**: Defines the behavior of the IoT device in terms of data collection, processing, and communication.

- **User Interface**: An interactive web application allowing stakeholders to visualize road conditions and manage alerts.

### Sensor Nodes

#### Requirements

1. **Cost Restriction per node**: XXX CHF
   - To keep cost of installation and parts low, one single node/sensor package will be installed in the drivers cabine.
2. **Quantify felt RoadState for Driver**:
   - Node has to be close to the driver and mounted securely to the chassis to minimize errors.
   - Roadstate will be quantified in a range of 0 (very good) to 14 (very bad) with a value of 15 for hazardous condition.
   - The road state is assigned for 3m of road at a time (reduce communication).
3. **Adapt Quantification to different cars and driving states**:
   - A simple linear Mass-Spring-Damper Model is chosen to model the cars factor on the transduced shocks. (While keeping computational effort low.)
   - A first calibration phase coupled to a initial parameter set aims to fit Mass-Spring-Damper Model parameters.
   - Measured data will be fit to quantified values during calibration phase.
   - Further physical quatities other than z-axis acceleration have to be considered to decouple driving induced accelerations from the road state.
4. **Sensing of physical quantities**:
   1. **Acceleration in z-Axis** to determine road state and potholes. (Adapt pollingrate to vehicle velocity/ must be high enough)
   2. **Acceleration in x,y-Axis and rotational acceleration** to minimize errors induced from driving scenarios.
   3. **Driving Velocity** to coupple shock amplitudes to velocity (through Spring-Damper Model).
   4. **Geographical Position** to reference qualification to current position.
   5. **Driving Direction** to deduce road lane (use gps data).
5. **Transmit Data at established Gatepoints**
   1. Transmitted information Format:
      - (Node ID (2 Byte)) | Position (2 \* 4 Byte (SP)) | RoadState (0.5 Byte)
   2. Preprocess and save (Position, Quality)-Tuples locally on Node
   3. Only save and transmit date every 3 meters
   4. Automatically establish connection at gatepoints and transmit new gathered data

#### RT-UML with State Charts

![RT-UML](../assets/diagrams/edge_node_rtuml/edge_node_rtuml.svg)

#### Hardware Components

1. **Microcontroller**:
   - **Arduino Nano 33 IoT** or similar with built-in Wi-Fi capability.
2. **IMU Sensor**:
   - **MPU-6050** or **MPU-9250** accelerometer and gyroscope module.
3. **GPS Module**:
   - **Ublox NEO-6M** GPS module for accurate positioning.
4. **Power Supply**:
   - **Primary**: Connected to the vehicle's power supply.
   - **Backup**: Rechargeable Li-Po battery with voltage regulation.
5. **Enclosure**:
   - Durable casing with LEDs for status indication (e.g., transmission activity, errors).
6. **Connectivity**:
   - Wi-Fi module (if not integrated) like **ESP8266** or **ESP32**.

#### Firmware Development

- **Sensor Calibration**:
  - Implement routines to calibrate the IMU sensor for accurate readings.
- **Data Sampling**:
  - Sample sensor data at appropriate intervals (e.g., 50 Hz).
- **Noise Reduction**:
  - Apply Kalman Filter or Complementary Filter to fuse sensor data.
  - Use moving averages or median filters to smooth out transient spikes.
- **Baseline Adjustment**:
  - Establish a baseline for the vehicle's normal vibrations.
  - Adjust subsequent readings by subtracting the baseline values.
- **Wi-Fi Connectivity**:
  - Configure to connect to known Wi-Fi networks.
  - Implement setup mode for inputting Wi-Fi credentials.
- **Data Packaging**:
  - Format data (timestamp, GPS coordinates, vibration metrics) for transmission.
- **Data Transmission**:
  - Use HTTP/HTTPS protocols to send data to the server.
  - Implement error handling and retries for network issues.
- **Power Management**:
  - Monitor power source and switch between vehicle power and backup battery as needed.
  - Implement sleep modes when the vehicle is not in motion.

#### Data Processing and Noise Reduction

- **Calibration Period**:
  - Collect initial data to establish the vehicle's baseline vibration patterns.
  - Dynamically adjust the baseline to account for changes (e.g., vehicle load).
- **Filtering Techniques**:
  - **Low-Pass Filter**: Remove high-frequency noise unrelated to road conditions.
  - **High-Pass Filter**: Eliminate low-frequency movements like vehicle tilts.
  - **Band-Pass Filter**: Focus on frequencies corresponding to road-induced vibrations.
- **Statistical Methods**:
  - **Standard Deviation and Variance**: Measure dispersion of vibration data.
  - **Peak Detection**: Identify significant deviations indicating bumps or potholes.
  - **Thresholding**: Categorize road conditions based on vibration intensity thresholds.
- **Edge Computing**:
  - **Data Compression**: Reduce data size by transmitting only significant events.
  - **Event Detection**: Implement on-device logic to detect and report anomalies.
  - **Power Efficiency**: Optimize code to reduce processor load and conserve battery life.

### Actuator Nodes

The **actuator nodes** are integrated within the sensor nodes, handling data transmission and power management:

- **Data Transmission**:
  - Manage communication protocols and ensure secure data transfer to the server.
  - Queue data for transmission when connectivity is unavailable.
- **Power Management**:
  - Switch between vehicle power and backup battery seamlessly.
  - Monitor battery levels and optimize power consumption.

## System Architecture

The **RoadSense** consists of multiple IoT devices installed in vehicles, communicating with a central server designed to be highly scalable to handle data from thousands of devices.

#### IoT Data Pipeline

The data pipeline has been designed with scalability in mind, allowing for efficient data collection, processing, and data analysis from multiple (potentially thousands) concurrent IoT devices. The following diagram illustrates the pipeline steps, from data ingestion to the storage of processed data.

![IoT Data Pipeline](../assets/diagrams/iot_data_pipeline/iot_data_pipeline.svg)

The pipeline consists of the following components:

1. Data ingestion: IoT devices collect vibration, GPS, and other relevant data points. When the vehicle reaches an access point, the data will be transmitted to the server.

2. Authentication and security: Each device is authenticated before data transmission to ensure data integrity and prevent unauthorized access. For this purpose, we chose to use [Keycloak](https://www.keycloak.org/) for identity and access management.

3. Geographical distribution: The server leverages DNS-based load balancing to distribute incoming data across regional gateways for efficient processing. We have chosen to use [BIND9](https://www.isc.org/bind/) for DNS-based load balancing along with [GeoIP](https://www.maxmind.com/en/geoip2-databases) for geolocation.

4. Message queues: Each gateway node processes incoming data and forwards it to a regional queuing system to allow for parallel processing. After evaluating multiple options, we decided to use [RabbitMQ](https://www.rabbitmq.com/) as the message queue system. To ensure high availability, we will deploy RabbitMQ in a cluster configuration (refer to the [RabbitMQ Clustering Guide](https://www.rabbitmq.com/clustering.html)).

5. Data preprocessing: Each region has a set of preprocessing microservices that consume incoming data from the regional message queuing system, perform data validation, and run initial data processing tasks. These microservices are deployed using containerization technology like Docker and in a future production environment, managed by [Kubernetes](https://kubernetes.io/). Since we want to ensure high-performance data processing, and a small memory footprint, we chose to use [Go](https://golang.org/) as the primary language for these microservices.

6. Data storage: Processed data is stored in a scalable database system that can handle high volumes of data. Since we are dealing with both date-time and geospatial data, we chose to use [TimescaleDB](https://www.timescale.com/) as the database system with the [PostGIS](https://postgis.net/) extension to support geospatial queries. To ensure data durability and high availability, we will deploy TimescaleDB in a clustered configuration.

#### Client-Server Architecture

This section describes the client-server architecture of the system, focusing on the interaction between the web application and the server-side components. The following diagram illustrates how the client (web browser) interacts with the server to load and visualize collected data:

![Client-Server Architecture](../assets/diagrams/web_app_architecture/web_app_architecture.svg)

## Server-Side Application

### API Development

- **RESTful APIs**:
  - For data ingestion from IoT devices.
  - For data retrieval by the web interface.
- **Authentication and Security**:
  - Implement token-based authentication.
  - Secure data transmission with HTTPS.
- **Rate Limiting**:
  - Prevent server overload by controlling the rate of incoming requests.

### Database Design

- **Data Storage**:
  - Use scalable databases like PostgreSQL or MongoDB.
  - Define schemas for raw sensor data, processed data, and aggregated results.
- **Spatial Indexing**:
  - Utilize geospatial indexing for efficient geographical queries.
- **Optimization**:
  - Optimize queries for real-time data access and analysis.

### Data Aggregation and Analysis

- **Data Processing Pipeline**:
  - Aggregate data from multiple devices.
  - Apply further filtering and anomaly detection.
- **Scalability**:
  - Design the backend to handle high volumes of data.
  - Use message queues and microservices architecture.
- **Analysis Techniques**:
  - Machine learning algorithms to improve anomaly detection.
  - Predictive analytics for road degradation.

### Visualization

- **Web Interface**:
  - Develop a responsive web application for data visualization.
  - Integrate mapping APIs like Google Maps or OpenStreetMap.
- **Heatmap Generation**:
  - Calculate bumpiness scores for road segments.
  - Overlay heatmaps on the map interface.
- **Interactive Features**:
  - Enable filtering by time ranges, severity levels, or specific areas.
  - Provide statistical summaries and trends.
- **User Engagement**:
  - Allow stakeholders to add comments or additional data.

## Control Logic

- **Data Collection**:
  - Continuously collect IMU and GPS data when the vehicle is in motion.
- **Data Processing**:
  - Apply noise reduction and baseline adjustments in real-time.
- **Communication**:
  - Transmit data to the server when connectivity is available.
  - Implement retry mechanisms for failed transmissions.
- **Power Management**:
  - Switch between power sources as needed.
  - Enter low-power modes when idle.
- **Error Handling**:
  - Monitor system health and report errors to the server.
  - Indicate status through LEDs on the device casing.

## Testing and Validation

- **Field Testing**:
  - Install devices in various vehicle types (sedans, SUVs, trucks).
  - Collect data over different road conditions (urban, rural, highways).
- **Algorithm Tuning**:
  - Adjust filtering parameters based on test results.
  - Validate bumpiness scores against known road conditions.
- **User Feedback**:
  - Gather feedback from stakeholders to improve system accuracy and usability.
- **Simulation**:
  - Use simulated data to test the system under various scenarios.

## Possible Problems and Solutions

### Network Connectivity

- **Wi-Fi Availability**:
  - **Challenge**: Continuous Wi-Fi connectivity may not be available during travel.
  - **Solution**: Store data locally and transmit when connectivity is available.
  - **Alternative**: Use mobile hotspots or integrate GSM modules for cellular data.
- **Data Security**:
  - Encrypt data transmissions to prevent interception or tampering.
- **Error Handling**:
  - Implement robust error handling for network disruptions.

### Vehicle Variability

- **Machine Learning Models**:
  - Train models to adjust for different vehicle characteristics.
- **User Input**:
  - Allow users to specify vehicle type during device setup for better calibration.
- **Adaptive Algorithms**:
  - Continuously learn and adapt to the vehicle's behavior over time.

### Power Management

- **Sleep Modes**:
  - Implement low-power modes when the vehicle is stationary.
- **Efficient Components**:
  - Use low-power sensors and microcontrollers.
- **Power Monitoring**:
  - Provide alerts when battery levels are low.

### Data Volume Management

- **Data Sampling**:
  - Optimize sampling rates to balance data quality and volume.
- **Selective Reporting**:
  - Transmit only aggregated or significant data points.
- **Data Compression**:
  - Compress data before transmission to reduce bandwidth usage.
