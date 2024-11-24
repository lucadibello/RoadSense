#include "RabbitMQClient.h"

RabbitMQClient::RabbitMQClient(const char* host, uint16_t port, const char* user, const char* password)
    : _host(host), _port(port), _user(user), _password(password) {}

bool RabbitMQClient::connect() {
    if (_amqpClient.connect(_wifiClient, _host, _port, _user, _password)) {
        Serial.println("Connected to RabbitMQ.");
        return true;
    }
    Serial.println("Failed to connect to RabbitMQ.");
    return false;
}

bool RabbitMQClient::sendToQueue(const char* queueName, const char* message) {
    if (_amqpClient.isConnected()) {
        if (_amqpClient.publish(queueName, message)) {
            Serial.println("Message sent to RabbitMQ.");
            return true;
        }
        Serial.println("Failed to send message.");
    } else {
        Serial.println("Not connected to RabbitMQ.");
    }
    return false;
}

void RabbitMQClient::disconnect() {
    _amqpClient.close();
    Serial.println("Disconnected from RabbitMQ.");
}

