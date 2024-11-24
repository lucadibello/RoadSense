#include <WiFi.h>
#include "RabbitMQClient.h"

// WiFi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// RabbitMQ server details
const char* host = "your-rabbitmq-server.com";
uint16_t port = 5672;
const char* user = "guest";
const char* pass = "guest";

// Queue name
const char* queueName = "testQueue";

RabbitMQClient rabbitMQ(host, port, user, pass);

void setup() {
    Serial.begin(115200);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi.");

    // Connect to RabbitMQ
    if (rabbitMQ.connect()) {
        // Send a test message
        rabbitMQ.sendToQueue(queueName, "Hello, RabbitMQ!");
        rabbitMQ.disconnect();
    }
}

void loop() {
    // No actions in the loop for this example
}

