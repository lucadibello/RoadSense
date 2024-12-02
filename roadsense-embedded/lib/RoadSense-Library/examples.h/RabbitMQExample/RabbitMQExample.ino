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
const char* mqtt_vhost = "my_vhost";  // Virtual Host
const char* mqtt_client_id = "PortentaClient";  // Client ID

WiFiClient wifiClient;
PubSubClient _mqttClient(wifiClient);

class RabbitMQClient {
  public:
    RabbitMQClient(const char* user, const char* password, const char* vhost) {
        _user = user;
        _password = password;
        _vhost = vhost;
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
    void publish(const char* topic, const char* message) {
        if (_mqttClient.connected()) {
            _mqttClient.publish(topic, message);
        }
    }

    // Loop to maintain the MQTT connection
    void loop() {
        _mqttClient.loop();
    }

  private:
    const char* _user;
    const char* _password;
    const char* _vhost;
};

// Initialize the RabbitMQClient
RabbitMQClient rabbitMQClient(mqtt_user, mqtt_password, mqtt_vhost);

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
    rabbitMQClient.subscribe("your/topic");
}

void loop() {
    // Keep the connection alive and process incoming messages
    rabbitMQClient.loop();
    
    // You can publish messages as needed
    // rabbitMQClient.publish("your/topic", "Hello, RabbitMQ!");
}
