#include <Arduino.h>       // Arduino classes, including Print and Stream
#include <WiFi.h>          // Arduino WiFi
#include "SegmentQuality.h"
#include "RabbitMQClient.h" // includes PubSubClient.h which uses Arduino::Stream
#include "roadqualifier.h"  // roadqualifier code

#include <mbed.h>
#include <rtos.h>
#include <mutex>
#include <events/mbed_events.h>

// Avoid any macro collisions
#undef Stream

using namespace rtos;

#define TIME_T2 1.0
#define BUFFER_SIZE 10
#define WATCHDOG_TIMEOUT 3.0  // Watchdog timeout in seconds
#define SERIAL_BAUD 115200    // Serial baud rate


rtos::Thread t1;
rtos::Thread t2;
mbed::Ticker task2;
rtos::Mutex buffer_mutex;

RoadQualifier roadQualifier;
RabbitMQClient rabbitMQClient;

class MyCircularBuffer {
public:
    MyCircularBuffer() : head(0), tail(0), full(false) {}

    bool put(const SegmentQuality& item) {
        std::lock_guard<Mutex> lock(buffer_mutex); // mutex gets released automatically when lock goes out of scope
        buffer[head] = item;
        if (full) {
            tail = (tail + 1) % BUFFER_SIZE;
        }
        head = (head + 1) % BUFFER_SIZE;
        full = head == tail;
        return true;
    }

    bool get(SegmentQuality& item) {
        std::lock_guard<Mutex> lock(buffer_mutex);
        if (isEmpty()) {
            return false;
        }
        item = buffer[tail];
        full = false;
        tail = (tail + 1) % BUFFER_SIZE;
        return true;
    }

    bool isEmpty() const {
        return (!full && (head == tail));
    }

    bool isFull() const {
        return full;
    }

private:
    SegmentQuality buffer[BUFFER_SIZE];
    size_t head;
    size_t tail;
    bool full;
};


MyCircularBuffer circular_buffer;

Watchdog &watchdog = Watchdog::get_instance();

// Task 1: run the road qualifier
void task1_function() {
    SegmentQuality segmentQuality;

    while (true) {
        // Qualify a road segment
        if (roadQualifier.qualifySegment()) {
            segmentQuality = roadQualifier.getSegmentQuality();

            // Add data to the buffer (overwriting oldest data if full)
            circular_buffer.put(segmentQuality);

            Serial.print("Added to buffer segment quality: ");
            Serial.print(segmentQuality.latitude);
            Serial.print(", ");
            Serial.print(segmentQuality.longitude);
            Serial.print(", ");
            Serial.println(segmentQuality.quality);
        } else {
            Serial.println("Failed to qualify segment.");
        }

        ThisThread::sleep_for(1000); // 100 ms
    }
}

// Task 2: send data over RabbitMQ
void task2_function() {
    SegmentQuality segmentQuality;

    while (true) {
        // Connect to WiFi
        rabbitMQClient.connectWiFi();

        while (rabbitMQClient.isConnectedWiFi()) {
            if (circular_buffer.get(segmentQuality)) {

                rabbitMQClient.sendDataCallback(segmentQuality);
                
                Serial.print("Sent segment quality: ");
                Serial.print(segmentQuality.latitude);
                Serial.print(", ");
                Serial.print(segmentQuality.longitude);
                Serial.print(", ");
                Serial.println(segmentQuality.quality);
            } else {
                Serial.println("Buffer is empty. Waiting for data.");
            }

        } 
    }
    
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    while (!Serial);

    // Initialize the road qualifier
    if (!roadQualifier.begin()) {
        Serial.print("Failed to initialize RoadQualifier.\n");
        return;
    }

    // t1.start(task1_function);
    t2.start(task2_function);    
}

void loop() {
    // Empty loop, tasks handled by threads
}