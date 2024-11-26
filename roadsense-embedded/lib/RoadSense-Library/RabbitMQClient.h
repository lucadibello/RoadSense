#ifndef RabbitMQClient_h
#define RabbitMQClient_h

#include <Arduino.h>
#include <WiFi.h>          // Portenta's WiFi library
#include <PubSubClient.h>  // MQTT library for communication

class RabbitMQClient {
public:
    RabbitMQClient(const char* host, uint16_t port, const char* user, const char* password);

    bool connect();
    bool publish(const char* topic, const char* message);
    void disconnect();

private:
    const char* _host;
    uint16_t _port;
    const char* _user;
    const char* _password;

    WiFiClient _wifiClient;      // Standard WiFi client for Portenta
    PubSubClient _mqttClient;    // MQTT client for communication

    bool ensureConnected();
};

#endif
