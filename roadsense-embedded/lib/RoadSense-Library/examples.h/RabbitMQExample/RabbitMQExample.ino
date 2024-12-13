#include <WiFi.h>
#include <RabbitMQClient.h>

// Create an instance of the RabbitMQClient class
RabbitMQClient rabbitMQClient;

void setup() {
    Serial.begin(115200);

    // Connect to WiFi
    WiFi.begin(SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("WiFi connected");

    // Connect to RabbitMQ (MQTT)
    if (rabbitMQClient.connect()) {
        Serial.println("Connected to RabbitMQ");
    } else {
        Serial.println("Failed to connect to RabbitMQ");
    }
}

void loop() {
    // Example RoadData to send
    RoadData data = {1234, 75};  // position = 1234, road_quality = 75

    // Convert position and road_quality to string for publishing
    char message[50];
    snprintf(message, sizeof(message), "Position: %u, Road Quality: %u", data.position, data.road_quality);

    // Publish data
    if (rabbitMQClient.publish("test/topic", message)) {
        Serial.println("Message sent");
    } else {
        Serial.println("Message failed");
    }

    delay(2000);  // Wait before sending next message
}
