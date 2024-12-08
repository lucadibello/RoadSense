#include <WiFi.h>
#include <PubSubClient.h>  // Include the MQTT library

const char* ssid = "Galaxy S10 Lite";
const char* password = "12345678";
const char* mqtt_server = "192.168.173.103";  // RabbitMQ server IP
const int mqtt_port = 1883;  // MQTT port
const char* mqtt_user = "guest";  // MQTT username
const char* mqtt_password = "guest";  // MQTT password
const char* mqtt_client_id = "PortentaClient";  // Client ID
const char* topic = "road_quality_topic";

WiFiClient wifiClient;
PubSubClient _mqttClient(wifiClient);

// Queue to hold messages (max 10 messages in this example)
#define QUEUE_SIZE 10
String messageQueue[QUEUE_SIZE];
int queueHead = 0;
int queueTail = 0;

// Define position and road quality
float latitude = 23.0123;  // Example latitude
float longitude = 72.5695; // Example longitude
uint8_t road_quality = 85; // Example road quality (1 byte)

class RabbitMQClient {
  public:
    RabbitMQClient(const char* user, const char* password) {
        _user = user;
        _password = password;
    }

    bool connect() {
        _mqttClient.setServer(mqtt_server, mqtt_port);
        
        if (_mqttClient.connect(mqtt_client_id, _user, _password)) {
            Serial.println("Connected to RabbitMQ.");
            return true;
        } else {
            Serial.print("Connection failed, rc=");
            Serial.println(_mqttClient.state());
            return false;
        }
    }

    void subscribe(const char* topic) {
        if (_mqttClient.connected()) {
            _mqttClient.subscribe(topic);
        }
    }

    bool publish(const char* topic, const char* message) {
        if (_mqttClient.connected()) {
            bool result = _mqttClient.publish(topic, message);
            if (result) {
                Serial.println("Message published successfully.");
            } else {
                Serial.println("Failed to publish message.");
            }
            return result;
        } else {
            Serial.println("MQTT client not connected.");
            return false;
        }
    }

    void loop() {
        _mqttClient.loop();
    }

    void enqueueMessage(const String& msg) {
        if ((queueTail + 1) % QUEUE_SIZE != queueHead) {
            messageQueue[queueTail] = msg;
            queueTail = (queueTail + 1) % QUEUE_SIZE;
        } else {
            Serial.println("Queue is full, dropping message.");
        }
    }

    String dequeueMessage() {
        if (queueHead != queueTail) {
            String msg = messageQueue[queueHead];
            queueHead = (queueHead + 1) % QUEUE_SIZE;
            return msg;
        }
        return "";
    }

    bool hasMessages() {
        return queueHead != queueTail;
    }

  private:
    const char* _user;
    const char* _password;
};

// Initialize the RabbitMQClient
RabbitMQClient rabbitMQClient(mqtt_user, mqtt_password);

void setup() {
    Serial.begin(115200);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi.");

    // Connect to RabbitMQ (MQTT Broker)
    int retries = 0;
    while (!rabbitMQClient.connect() && retries < 5) {
        delay(5000);  // Retry every 5 seconds
        retries++;
    }
    if (retries >= 5) {
        Serial.println("Failed to connect to RabbitMQ after 5 retries.");
    }
}

void loop() {
    // Prepare the message in JSON format
    String message = "{";
    message += "\"position\":{\"latitude\":" + String(latitude, 4) + ",\"longitude\":" + String(longitude, 4) + "},";
    message += "\"road_quality\":" + String(road_quality);
    message += "}";

    // Enqueue the message
    rabbitMQClient.enqueueMessage(message);

    // Process the queue and send messages
    while (rabbitMQClient.hasMessages()) {
        String msgToPublish = rabbitMQClient.dequeueMessage();
        
        if (rabbitMQClient.publish("road_quality_topic", msgToPublish.c_str())) {
            Serial.println("Message sent: " + msgToPublish);
        } else {
            Serial.println("Message failed to send.");
            // Retry connection if the client is disconnected
            if (!rabbitMQClient.connect()) {
                Serial.println("Reconnecting to RabbitMQ...");
            }
        }
    }

    rabbitMQClient.loop();  // Keep MQTT client loop running
    delay(5000);  // Wait for 5 seconds before next iteration
}
