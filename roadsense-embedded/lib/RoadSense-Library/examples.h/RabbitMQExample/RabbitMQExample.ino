#include <WiFi.h>
#include <RabbitMQClient.h>

// WiFi Credentials
#define SSID "Galaxy S10 Lite"
#define WIFI_PASSWORD "12345678"

// MQTT Broker details
#define MQTT_SERVER "192.168.58.103"
#define MQTT_PORT 1883
#define MQTT_USER "jam"
#define MQTT_PASSWORD "jam"

// Create an instance of the RabbitMQClient class
RabbitMQClient rabbitMQClient(MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD);

// Structure to hold data to send
SegmentQuality segmentData = {25.276987, 55.296249, 80};  // Example: latitude, longitude, quality

// WiFi connection setup
void connectWiFi() {
    WiFi.begin(SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("WiFi connected");
}

// Callback function to periodically send data
void sendDataCallback() {
    // Attempt to connect to RabbitMQ if not connected
    if (rabbitMQClient.connect()) {
        Serial.println("Connected to RabbitMQ");

        // Publish data to RabbitMQ
        if (rabbitMQClient.publishSegmentQuality("test/topic", segmentData)) {
            Serial.println("SegmentQuality data sent successfully.");
        } else {
            Serial.println("Failed to send SegmentQuality data.");
        }
    } else {
        Serial.println("Failed to connect to RabbitMQ");
    }

    // Simulate periodic operation (e.g., send data every 2 seconds)
    delay(2000);
}

void setup() {
    Serial.begin(115200);

    // Connect to WiFi
    connectWiFi();

    // Start a loop that checks connection and sends data
    while (true) {
        sendDataCallback();
    }
}

void loop() {
    // Normally, you'd put your code here for other tasks
    // In this case, sendDataCallback is used for continuous operation
}
