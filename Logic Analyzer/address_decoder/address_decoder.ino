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

// Use first 16 pins for 16 lines of address
const char ADDR[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

#define SHIFT_CLOCK_PIN 16  // Pin used to sync data shifting to/from second controller
#define SHIFT_DATA_PIN 17   // Pin used to get data from to/second controller
#define GLOBAL_CLOCK_PIN 18 // Global Clock

uint8_t readData(pin_size_t dataPin, uint8_t clockPin) {
	uint8_t value = 0;
	uint8_t i;

	for (i = 0; i < 8; ++i) {
		digitalWrite(clockPin, HIGH);
		// delay(10);
		value |= digitalRead(dataPin) << i;
        digitalWrite(clockPin, LOW);
		// delay(1);
	}
	return value;
}

void setup() {
    // Configure pins used for getting address to input mode
    for (int i = 0; i < 16; i++) {
        pinMode(ADDR[i], INPUT);
    }
    
    pinMode(SHIFT_CLOCK_PIN, OUTPUT);
    pinMode(SHIFT_DATA_PIN, INPUT);

    pinMode(GLOBAL_CLOCK_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(GLOBAL_CLOCK_PIN), onClock, RISING);

    Serial.begin(57600);
}

volatile unsigned short int address = 0;
volatile unsigned char data = 0;
volatile bool ready = false;

void onClock() {
    address = 0;
    for(int i = 0; i < 16; i++) {
        address |= digitalRead(ADDR[i]) << i;
    }
    digitalWrite(SHIFT_CLOCK_PIN, LOW);
    // data = readData(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN);
    data = shiftIn(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, LSBFIRST);
    ready = true;
}

void loop() {
    if (ready) {
        Serial.println(data);
        ready = false;
    }
    // put your main code here, to run repeatedly:
}
