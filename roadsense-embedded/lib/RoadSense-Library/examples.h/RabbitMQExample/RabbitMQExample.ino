#include <WiFi.h>
#include <PubSubClient.h>
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
  // Example of sending a message to RabbitMQ
  if (rabbitMQClient.publish("test/topic", "Hello RabbitMQ")) {
    Serial.println("Message sent");
  } else {
    Serial.println("Message failed");
  }
  delay(2000);
}
