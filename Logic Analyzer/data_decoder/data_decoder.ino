/*
    - Logic analyzer consists of 2 controllers, communicating over software serial interface(kinda)
        - First being used for decoding address
        - Second used for decoding data)
        - Raspberry PI Pico is used for address decoder module as master device
        - Waveshare RP2040-Zero is used for data decoder module as slave device
    - On each global clock (supplied by clock gen used for MPU, EEPROM, RAM and other 6502-related peripherals):
        - Both modules will dump their related states of MPU
        - Master module will wait for slave module to supply it with additional data
*/

#include <SPI.h>

// Use first 16 pins for 16 lines of address
const char DATA[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

// #define SHIFT_CLOCK_PIN 15  // Pin used to sync data shifting to  controller
// #define SHIFT_DATA_PIN 14   // Pin used to get data from to controller
#define GLOBAL_CLOCK_PIN 8 // Global Clock

void setup() {
    // Configure pins used for getting address to input mode
    for (int i = 0; i < 8; i++) {
        pinMode(DATA[i], INPUT);
    }
    
    // Attach interrupt to global clock running while system
    pinMode(GLOBAL_CLOCK_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(GLOBAL_CLOCK_PIN), onGlobalClock, RISING);

    SPI.begin();

    // Start USB Serial interface to dump info to serial monitor
    Serial.begin(57600);
}

volatile unsigned char data = 0;
volatile uint8_t bit_index = 0;
volatile bool ready = false;

void onGlobalClock() {
    data = 0;
    for(int i = 0; i < 8; i++) {
        data |= digitalRead(DATA[i]) << i;
    }
    ready = true;
}

// void onShiftClock() {
//     digitalWrite(SHIFT_DATA_PIN, data & (1 << bit_index));
//     bit_index++;
//     if (bit_index == 8) {
//         // detachInterrupt(digitalPinToInterrupt(SHIFT_CLOCK_PIN));
//         bit_index = 0;
//         data = 0;
//     }
//     ready = true;
// }

void loop() {
    if (ready) {
        Serial.print(data);
        Serial.print(" ");
        Serial.println(bit_index);
        ready = false;
    }
    // Serial.println("-");
}
