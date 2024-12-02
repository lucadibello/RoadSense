#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiSSLClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <WiFi.h>
#include <PubSubClient.h>  // Include the MQTT library
#include <ArduinoJson.h>    // For working with JSON data (optional)

// Define the necessary variables
const char* ssid = "Galaxy S10 Lite";
const char* password = "12345678";
const char* mqtt_server = "192.168.173.103";  // RabbitMQ server IP
const int mqtt_port = 1883;  // MQTT port
const char* mqtt_user = "guest";  // MQTT username
const char* mqtt_password = "guest";  // MQTT password
const char* mqtt_client_id = "PortentaClient";  // Client ID

WiFiClient wifiClient;
PubSubClient _mqttClient(wifiClient);

class RabbitMQClient {
  public:
    RabbitMQClient(const char* user, const char* password) {
        _user = user;
        _password = password;
    }

    // Connect to RabbitMQ using MQTT
    bool connect() {
        _mqttClient.setServer(mqtt_server, mqtt_port);
        
        // Connect to the RabbitMQ server using MQTT credentials
        if (_mqttClient.connect(mqtt_client_id, _user, _password)) {
            Serial.println("Connected to RabbitMQ.");
            return true;
        } else {
            Serial.print("Connection failed, rc=");
            Serial.println(_mqttClient.state());
            return false;
        }
    }

    // Subscribe to MQTT topic
    void subscribe(const char* topic) {
        if (_mqttClient.connected()) {
            _mqttClient.subscribe(topic);
        }
    }

    // Publish a message to MQTT topic
    bool publish(const char* topic, const char* message) {
        if (_mqttClient.connected()) {
            bool result = _mqttClient.publish(topic, message);
            if (result) {
                Serial.println("Message published successfully.");
            } else {
                Serial.println("Failed to publish message.");
            }
            return result;
        }
        return false;  // Return false if not connected
    }

    // Loop to maintain the MQTT connection
    void loop() {
        _mqttClient.loop();
    }

  private:
    const char* _user;
    const char* _password;
};

// Initialize the RabbitMQClient
RabbitMQClient rabbitMQClient(mqtt_user, mqtt_password);

// Define position and road quality
String position = "23.0123,72.5695";  // Example position (latitude, longitude)
uint8_t road_quality = 85;  // Example road quality (1 byte)

void setup() {
    Serial.begin(115200);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi.");

    // Connect to RabbitMQ
    while (!rabbitMQClient.connect()) {
        delay(5000);  // Retry every 5 seconds if connection fails
    }

    // Subscribe to a topic after successful connection
    rabbitMQClient.subscribe("road_quality_topic");
}

void loop() {
    // Prepare the message (position, road_quality as a tuple)
    String message = "{";
    message += "\"position\":\"" + position + "\",";
    message += "\"road_quality\":" + String(road_quality);
    message += "}";

    // Publish the message
    bool publishStatus = rabbitMQClient.publish("road_quality_topic", message.c_str());
    if (publishStatus) {
        Serial.println("Message sent: " + message);
    } else {
        Serial.println("Message failed to send.");
    }

    // Keep the connection alive and process incoming messages
    rabbitMQClient.loop();
    
    // Delay for 5 seconds before sending the next message
    delay(5000);
}


