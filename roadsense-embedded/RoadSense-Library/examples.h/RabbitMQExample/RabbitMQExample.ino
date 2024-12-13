#include <WiFi.h>
#include <RabbitMQClient.h>


// Create an instance of the RabbitMQClient class
RabbitMQClient rabbitMQClient;

// Structure to hold data to send
SegmentQuality segmentData = {25.276987, 55.296249, 80};  // Example: latitude, longitude, quality

void setup() {
    Serial.begin(115200);

    // Connect to WiFi
    rabbitMQClient.connectWiFi();

    // Start the loop that checks connection and sends data
    while (true) {
        rabbitMQClient.sendDataCallback(segmentData);
    }
}

void loop() {
    // Normally, you'd put your code here for other tasks
    // In this case, sendDataCallback is used for continuous operation
}
