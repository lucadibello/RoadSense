#include <mbed.h>
#include <mutex>
using namespace mbed;
using namespace rtos;

// Define timing constants
#define TIME_T1 0.5
#define TIME_T2 1.0
#define BUFFER_SIZE 10
#define WATCHDOG_TIMEOUT 3.0  // Watchdog timeout in seconds

// Event queue for task offloading
events::EventQueue evq;

// Threads to handle tasks
Thread t1;
Thread t2;
Thread t3;

// Tickers for periodic task activation
mbed::Ticker task1;
mbed::Ticker task2;

Mutex buffer_mutex;

typedef struct {
    int value;  // Example task data
} message_t;

class CircularBuffer {
public:
    CircularBuffer() : head(0), tail(0), full(false) {}

    bool put(const message_t& item) {
        std::lock_guard<Mutex> lock(buffer_mutex);
        buffer[head] = item;
        if (full) {
            tail = (tail + 1) % BUFFER_SIZE;
        }
        head = (head + 1) % BUFFER_SIZE;
        full = head == tail;
        return true;
    }

    bool get(message_t& item) {
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
    message_t buffer[BUFFER_SIZE];
    size_t head;
    size_t tail;
    bool full;
};

Queue<message_t, 10> task1_queue;
Queue<message_t, 10> task2_queue;

MemoryPool<message_t, 10> task1_pool;
MemoryPool<message_t, 10> task2_pool;

CircularBuffer task1_buffer;
CircularBuffer task2_buffer;

Watchdog &watchdog = Watchdog::get_instance();

// Task 1: Example processing function
void task1_function() {
    message_t *message = task1_pool.alloc();
    if (message) {
        message->value = rand() % 100; // simulate task 
        task1_buffer.put(*message);
        task1_pool.free(message);
    }

    watchdog.kick(); // refresh the watchdog timer
}

// Task 2: Example processing function
void task2_function() {
    message_t *message = task2_pool.alloc();
    if (message) {
        message->value = rand() % 50;  // simulate task
        task2_buffer.put(*message);
        task2_pool.free(message);
    }

    watchdog.kick(); // refresh the watchdog timer
}

// ISR for Ticker 1
void task1_isr() {
    evq.call(task1_function);
}

// ISR for Ticker 2
void task2_isr() {
    evq.call(task2_function);
}

void print_results() {
    while (true) {
        message_t msg;
        if (task1_buffer.get(msg)) {
            printf("Task1 data: %d\n", msg.value);
        }

        if (task2_buffer.get(msg)) {
            printf("Task2 data: %d\n", msg.value);
        }

        watchdog.kick(); // refresh the watchdog timer
        ThisThread::sleep_for(500ms); // no busy loop
    }
}

void setup() {
    watchdog.start(WATCHDOG_TIMEOUT * 1000);  // timeout

    // Start the periodic tasks
    task1.attach(&task1_isr, TIME_T1); 
    task2.attach(&task2_isr, TIME_T2); 

    // Start threads
    t1.start(callback(&evq, &events::EventQueue::dispatch_forever));
    t2.start(callback(&evq, &events::EventQueue::dispatch_forever));
    t3.start(print_results);  // Start thread to process and print data
}

void loop() {

}