#include <mbed.h>
#include <mutex>
#include "roadqualifier.h"
using namespace mbed;
using namespace rtos;

#define TIME_T2 1.0
#define BUFFER_SIZE 10
#define WATCHDOG_TIMEOUT 3.0  // Watchdog timeout in seconds
#define SERIAL_BAUD 115200    // Serial baud rate


// Event queue for task offloading
events::EventQueue evq;

// Threads to handle tasks
Thread t1;
Thread t2;

// Tickers for periodic task activation
mbed::Ticker task2;

// mutex for buffer access
Mutex buffer_mutex;

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
    RoadQualifier roadQualifier;
    SegmentQuality segmentQuality;

    // Initialize the road qualifier
    if (!roadQualifier.begin()) {
        Serial.print("Failed to initialize RoadQualifier.\n");
        return;
    }

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
        if (circular_buffer.get(segmentQuality)) {
            // TODO: Send data over RabbitMQ
            Serial.print("Sent segment quality: ");
            Serial.print(segmentQuality.latitude);
            Serial.print(", ");
            Serial.print(segmentQuality.longitude);
            Serial.print(", ");
            Serial.println(segmentQuality.quality);
        } else {
            Serial.println("Buffer is empty. Waiting for data.");
        }

        ThisThread::sleep_for(1000); // 100 ms
    }
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    while (!Serial);

    t1.start(task1_function);
    t2.start(task2_function);    
}

void loop() {
    // Empty loop, tasks handled by threads
}