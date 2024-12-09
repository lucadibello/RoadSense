#include <mbed.h>
using namespace mbed;
using namespace rtos;

// Define timing constants
#define TIME_T1 0.5
#define TIME_T2 1.0

// Event queue for task offloading
events::EventQueue evq;

// Threads to handle tasks
Thread t1;
Thread t2;
Thread t3;

// Tickers for periodic task activation
mbed::Ticker task1;
mbed::Ticker task2;

// Queue and memory pool for inter-task communication
typedef struct {
    int value;  // Example task data
} message_t;

Queue<message_t, 10> task1_queue;
Queue<message_t, 10> task2_queue;

MemoryPool<message_t, 10> task1_pool;
MemoryPool<message_t, 10> task2_pool;

// Task 1: Example processing function
void task1_function() {
    message_t *message = task1_pool.alloc();
    if (message) {
        message->value = rand() % 100;  // Simulate task data
        task1_queue.put(message);
    }
}

// Task 2: Example processing function
void task2_function() {
    message_t *message = task2_pool.alloc();
    if (message) {
        message->value = rand() % 50;  // Simulate task data
        task2_queue.put(message);
    }
}

// ISR for Ticker 1
void task1_isr() {
    evq.call(task1_function);
}

// ISR for Ticker 2
void task2_isr() {
    evq.call(task2_function);
}

// Print function to handle data from queues
void print_results() {
    while (true) {
        osEvent evt1 = task1_queue.get(TIME_T1 * 1000);
        if (evt1.status == osEventMessage) {
            message_t *msg = (message_t *)evt1.value.p;
            printf("Task1 data: %d\n", msg->value);
            task1_pool.free(msg);
        }

        osEvent evt2 = task2_queue.get(TIME_T2 * 1000);
        if (evt2.status == osEventMessage) {
            message_t *msg = (message_t *)evt2.value.p;
            printf("Task2 data: %d\n", msg->value);
            task2_pool.free(msg);
        }
    }
}

void setup() {
    // Start the periodic tasks
    task1.attach(&task1_isr, TIME_T1);  // Start ticker for Task 1
    task2.attach(&task2_isr, TIME_T2);  // Start ticker for Task 2

    // Start threads
    t1.start(callback(&evq, &events::EventQueue::dispatch_forever));
    t2.start(callback(&evq, &events::EventQueue::dispatch_forever));
    t3.start(print_results);  // Start thread to process and print data
}

void loop() {
    // Empty loop; tasks are handled via threads and tickersx
    // TODO: implement watchdog timer
}