#ifndef RabbitMQClient_h
#define RabbitMQClient_h

#include <Arduino.h>
#include <WiFi.h>          // Portenta's WiFi library
#include <PubSubClient.h>  // MQTT library for communication
#include <stdint.h>        // For uint16_t

// WiFi Credentials
#define SSID "Galaxy S10 Lite"
#define WIFI_PASSWORD "12345678"

// MQTT Broker details
#define MQTT_SERVER "192.168.58.103"
#define MQTT_PORT 1883
#define MQTT_USER "jam"
#define MQTT_PASSWORD "jam"

class RabbitMQClient {
public:
    RabbitMQClient(const char* host = MQTT_SERVER, uint16_t port = MQTT_PORT, const char* user = MQTT_USER, const char* password = MQTT_PASSWORD)
        : _host(host), _port(port), _user(user), _password(password), _mqttClient(_wifiClient), _errorCode(0) {}

    bool connect() {
        _mqttClient.setServer(_host, _port);
        if (!ensureConnected()) {
            _errorCode = _mqttClient.state();  // Store the error code when connection fails
            return false;
        }
        return true;
    }

    bool publish(const char* topic, const char* message) {
        if (!ensureConnected()) {
            Serial.println("Failed to publish: Not connected to RabbitMQ.");
            _errorCode = _mqttClient.state();  // Store error code on failure
            return false;
        }

        if (_mqttClient.publish(topic, message)) {
            Serial.println("Message published successfully.");
            return true;
        } else {
            _errorCode = _mqttClient.state();  // Store error code on failure
            Serial.println("Failed to publish message.");
            return false;
        }
    }

    void disconnect() {
        _mqttClient.disconnect();
        Serial.println("Disconnected from RabbitMQ.");
    }

    int getErrorCode() {
        return _errorCode;
    }

private:
    const char* _host;
    uint16_t _port;
    const char* _user;
    const char* _password;

    WiFiClient _wifiClient;      // WiFi client for Portenta
    PubSubClient _mqttClient;    // MQTT client for communication

    bool ensureConnected() {
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

    int _errorCode;  // Store the error code
};

// Example data tuple for position and road quality
struct RoadData {
    uint16_t position;  // position as uint16_t (2 bytes)
    uint8_t road_quality;  // road_quality as uint8_t (1 byte)
};

#endif
