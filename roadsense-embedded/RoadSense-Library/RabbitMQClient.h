#ifndef RabbitMQClient_h
#define RabbitMQClient_h

#include <Arduino.h>
#include <WiFi.h>          // Portenta's WiFi library
#include <PubSubClient.h>  // MQTT library for communication
#include <stdint.h>        // For uint8_t
#include "SegmentQuality.h"


// WiFi Credentials
#define ssid "Galaxy S10 Lite"
#define passwordWiFi "12345678"

// MQTT Broker details
#define host "192.168.58.103"
#define port 1883
#define user "jam"
#define password "jam"



class RabbitMQClient {
public:
    // Constructor with server, port, user, password
    RabbitMQClient()
        : _host(host), _port(port), _user(user), _password(password), _wifiClient(), _mqttClient(_wifiClient), _errorCode(0), _ssid(ssid), _passwordWiFi(passwordWiFi) {}

    // Connect to the WiFi
    void connectWiFi() {
        WiFi.begin(_ssid, _passwordWiFi);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("Connecting to WiFi...");
        }
        Serial.println("WiFi connected");
    }

    // Connect to RabbitMQ
    bool connect() {
        _mqttClient.setServer(_host, _port);
        if (!ensureConnected()) {
            _errorCode = _mqttClient.state();  // Store the error code when connection fails
            return false;
        }
        return true;
    }

    // Publish a message to a topic
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

    // Send SegmentQuality data as a JSON string to a RabbitMQ queue
    bool publishSegmentQuality(const char* topic, const SegmentQuality& segment) {
        String payload = "{\"latitude\": " + String(segment.latitude, 6) +
                         ", \"longitude\": " + String(segment.longitude, 6) +
                         ", \"quality\": " + String(segment.quality) + "}";

        return publish(topic, payload.c_str());
    }

    // Disconnect from RabbitMQ
    void disconnect() {
        _mqttClient.disconnect();
        Serial.println("Disconnected from RabbitMQ.");
    }

    // Get the error code from the last operation
    int getErrorCode() {
        return _errorCode;
    }

    // Send data in a callback function
    void sendDataCallback(SegmentQuality& segmentData) {
        // Attempt to connect to RabbitMQ if not connected
        if (connect()) {
            Serial.println("Connected to RabbitMQ");

            // Publish data to RabbitMQ
            if (publishSegmentQuality("test/topic", segmentData)) {
                Serial.println("SegmentQuality data sent successfully.");
            } else {
                Serial.println("Failed to send SegmentQuality data.");
            }
        } else {
            Serial.println("Failed to connect to RabbitMQ");
        }

        // Simulate periodic operation (e.g., send data every 2 seconds)
        delay(2000);
    }

private:
    const char* _host;
    uint16_t _port;
    const char* _user;
    const char* _password;
    const char* _ssid;
    const char* _passwordWiFi;

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

#endif
