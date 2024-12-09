#include <mbed.h>
using namespace mbed;
using namespace rtos;
using namespace std::literals::chrono_literals;
//#include <mbed-events.h>

#include <Arduino_HTS221.h>
#include <Arduino_APDS9960.h>

#define TIME_MULTIPLIER 10
#define TIME_T1 0.5
#define TIME_T2 TIME_T1 *TIME_MULTIPLIER

events::EventQueue evq; //sets up the queue for events

Thread t1;
Thread t2;
Thread t3;

Semaphore bus_i2c(1);

mbed::Ticker task1; //declares the tickers, one for every periodic thread
mbed::Ticker task2;

typedef struct {
  int value;
} message_t;

Queue<message_t, TIME_MULTIPLIER> t1_data;
Queue<message_t, TIME_MULTIPLIER> t2_data;

MemoryPool<message_t, TIME_MULTIPLIER> mpool1;
MemoryPool<message_t, 1> mpool2;


void f1() {
  int k = 0;
  message_t *message = mpool1.alloc();
  int r, g, b;
  bool coloravailable = false;


  coloravailable = false;
  while (!coloravailable) {
    bus_i2c.acquire();
    coloravailable = APDS.colorAvailable();
    bus_i2c.release();
    ThisThread::sleep_for(10);
  }
  bus_i2c.acquire();
  APDS.readColor(r, g, b);
  bus_i2c.release();
  message->value = r + g + b;
  t1_data.put(message);
  //Serial.println("t1");
}

void f2() {
  message_t *message = mpool2.alloc();
  bus_i2c.acquire();
  message->value = HTS.readTemperature();
  bus_i2c.release();
  t2_data.put(message);
  //Serial.println("t2");
}

/*  Tickers are periodic interrupts --> functions activated by tickers are ISR and they cannot contain blocking instructions (e.g., Serial.println, delay).
    We need to offload the computation to a specific thread, that is activated by means of an event that is added to the event queue
*/ 
void if1() {
  evq.call(&f1); //create an event for activating f1
}

void if2() {
  evq.call(&f2); //create an event for activating f1
}

void print() {
  int number_t1 = 0;
  float avg_t1 = 0;
  int val_t2 = 0;
  bool t2_valid = false;
  osEvent evt1, evt2;
  message_t *message1;
  message_t *message2;

  while (1) {
    if (!t2_valid) {
      osEvent evt2 = t2_data.get(TIME_T2);
      if (evt2.status == osEventMessage) {
        message2 = (message_t *)evt2.value.p;
        val_t2 = message2->value;
        mpool2.free(message2);
        t2_valid = true;
      }
    }

    evt1 = t1_data.get(TIME_T1);
    while (evt1.status == osEventMessage && number_t1 < TIME_MULTIPLIER) {
      message1 = (message_t *)evt1.value.p;
      avg_t1 += message1->value;
      mpool1.free(message1);
      number_t1++;
      evt1 = t1_data.get(TIME_T1 / 2);
    }

    if (number_t1 >= TIME_MULTIPLIER && t2_valid) {
      Serial.print(millis());
      Serial.print(": ");
      Serial.print(avg_t1 / number_t1);
      Serial.print(", ");
      Serial.println(val_t2);
      avg_t1 = 0;
      number_t1 = 0;
      t2_valid = false;
    }
  }
}

void setup() {
  Serial.begin(19200);

  if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1)
      ;
  }

  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor!");
    while (1)
      ;
  }

  task1.attach(&if1, TIME_T1); //starts the tickers: period is in seconds
  task2.attach(&if2, TIME_T2);
  t1.start(callback(&evq, &events::EventQueue::dispatch_forever)); //attaches the t1 thread to the eventqueue
  t2.start(callback(&evq, &events::EventQueue::dispatch_forever)); //attaches the t1 thread to the eventqueue
  t3.start(print);
}

void loop() {
  // Serial.println("alive");
  //delay(5000);
}
