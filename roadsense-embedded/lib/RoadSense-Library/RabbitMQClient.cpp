#include "RabbitMQClient.h"

RabbitMQClient::RabbitMQClient(const char* host, uint16_t port, const char* user, const char* password)
    : _host(host), _port(port), _user(user), _password(password), _mqttClient(_wifiClient) {}

bool RabbitMQClient::connect() {
    _mqttClient.setServer(_host, _port);
    return ensureConnected();
}

bool RabbitMQClient::publish(const char* topic, const char* message) {
    if (!ensureConnected()) {
        Serial.println("Failed to publish: Not connected to RabbitMQ.");
        return false;
    }

    if (_mqttClient.publish(topic, message)) {
        Serial.println("Message published successfully.");
        return true;
    } else {
        Serial.println("Failed to publish message.");
        return false;
    }
}

void RabbitMQClient::disconnect() {
    _mqttClient.disconnect();
    Serial.println("Disconnected from RabbitMQ.");
}

bool RabbitMQClient::ensureConnected() {
    if (!_mqttClient.connected()) {
        Serial.println("Connecting to RabbitMQ...");
        if (_mqttClient.connect("PortentaClient", _user, _password)) {
            Serial.println("Connected to RabbitMQ.");
        } else {
            Serial.print("Connection failed, rc=");
            Serial.println(_mqttClient.state());
            return false;
        }
    }
    return true;
}
