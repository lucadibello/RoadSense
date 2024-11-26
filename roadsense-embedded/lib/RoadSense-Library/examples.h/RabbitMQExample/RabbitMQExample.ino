#include <WiFi.h>
#include <RabbitMQClient.h>

// WiFi credentials
const char* ssid = "Galaxy S10 Lite";
const char* password = "12345678";

// RabbitMQ details
const char* rabbitmq_host = "rabbit.roadsense.dev";
const int rabbitmq_port = 1883; // For non-TLS connections
const char* rabbitmq_user = "your_user";
const char* rabbitmq_password = "your_password";

// Topic to publish data
const char* topic = "road_quality";

// RabbitMQ client instance
RabbitMQClient rabbitMQ(rabbitmq_host, rabbitmq_port, rabbitmq_user, rabbitmq_password);

void setup() {
    Serial.begin(115200);

    // Connect to WiFi
    Serial.print("Connecting to Wi-Fi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected!");

    // Connect to RabbitMQ
    if (rabbitMQ.connect()) {
        Serial.println("RabbitMQ connected!");
    } else {
        Serial.println("Failed to connect to RabbitMQ.");
    }
}

void loop() {
    // Example data
    String position = "23.0123,72.5695";
    uint8_t road_quality = 85;

    // Publish data
    String message = "{";
    message += "\"position\":\"" + position + "\",";
    message += "\"road_quality\":" + String(road_quality);
    message += "}";

    if (rabbitMQ.publish(topic, message.c_str())) {
        Serial.println("Message sent: " + message);
    } else {
        Serial.println("Message failed to send.");
    }

    delay(5000);
    
}
