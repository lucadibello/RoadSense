#ifndef RabbitMQClient_h
#define RabbitMQClient_h

#include <Arduino.h>
#include <WiFiClient.h>
#include <AMQPClient.h>  // From the arduino-rabbitmq library

class RabbitMQClient {
public:
    RabbitMQClient(const char* host, uint16_t port, const char* user, const char* password);
    bool connect();
    bool sendToQueue(const char* queueName, const char* message);
    void disconnect();

private:
    const char* _host;
    uint16_t _port;
    const char* _user;
    const char* _password;

    AMQPClient _amqpClient;
    WiFiClient _wifiClient;
};

#endif
