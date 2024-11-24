#include <WiFi.h>         // For WiFi connection
#include <PubSubClient.h> // MQTT library

// WiFi credentials
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// RabbitMQ MQTT broker details
const char* mqtt_server = "rabbit.roadsense.dev";
const int mqtt_port = 8883; // Use 1883 for non-TLS, 8883 for TLS
const char* mqtt_user = "your_rabbitmq_user";
const char* mqtt_password = "your_rabbitmq_password";

// Topic to send data
const char* topic = "road_quality";

// Initialize WiFi and MQTT clients
WiFiClientSecure wifiClient; // Use WiFiClientSecure for TLS
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  connectToWiFi();

  // Configure MQTT
  client.setServer(mqtt_server, mqtt_port);

  // Connect to MQTT broker
  connectToMQTT();
}

void loop() {
  // Ensure the MQTT connection remains active
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  // Prepare data
  String position = "23.0123,72.5695"; // Example position (latitude, longitude)
  uint8_t road_quality = 85;          // Example road quality

  // Send data to RabbitMQ
  sendData(position, road_quality);

  delay(5000); // Wait before sending the next message
}

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi!");
}

void connectToMQTT() {
  Serial.print("Connecting to MQTT broker...");
  while (!client.connected()) {
    if (client.connect("ArduinoClient", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT broker!");
    } else {
      Serial.print("Failed. Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void sendData(String position, uint8_t road_quality) {
  // Format message as JSON
  String message = "{";
  message += "\"position\":\"" + position + "\",";
  message += "\"road_quality\":" + String(road_quality);
  message += "}";

  // Publish message to RabbitMQ
  if (client.publish(topic, message.c_str())) {
    Serial.println("Message sent: " + message);
  } else {
    Serial.println("Failed to send message");
  }
}
