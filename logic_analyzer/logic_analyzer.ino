/*
    Logic analyzer is built using 3 SN74HC165N (parallel to serial) shift registers
    Waveshare RP2040-Zero is used as main controller
*/

// Enum with all pin definition
enum Pins {
    SERIAL_CS_O     =   8,
    SERIAL_CLK_O    =   7,
    SERIAL_DATA_I   =   14,
    GLOBAL_CLOCK_I  =   15,
};

// Because of some SN74HC165N specifics first bit is ready before first clock. Clock should be set HIGH after getting pin
uint8_t shift_in(pin_size_t dataPin, uint8_t clockPin, BitOrder bitOrder) {
    uint8_t value = 0;
    uint8_t i;
    for (i = 0; i < 8; ++i) {
        if (bitOrder == LSBFIRST)
            value |= digitalRead(dataPin) << i;
        else
            value |= digitalRead(dataPin) << (7 - i);
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
    return value;
}

void setup() {
    // Serial interface
    pinMode(SERIAL_CS_O, OUTPUT);
    pinMode(SERIAL_CLK_O, OUTPUT);
    pinMode(SERIAL_DATA_I, INPUT);
    
    // Clock
    pinMode(GLOBAL_CLOCK_I, INPUT);
    attachInterrupt(digitalPinToInterrupt(GLOBAL_CLOCK_I), onClock, RISING);
    
    // Start UART for monitoring events 
    Serial.begin(57600); 
}

void onClock() {
    // Prepare variables
    uint16_t addr = 0;
    uint8_t data = 0;

    // Set Chip Select Low for shift registers to parallel load
    digitalWrite(SERIAL_CS_O, LOW);
    digitalWrite(SERIAL_CS_O, HIGH);

    // Firstly get upper byte of address and secondly get low byte of address
    addr = shift_in(SERIAL_DATA_I, SERIAL_CLK_O, MSBFIRST) << 8;
    addr += shift_in(SERIAL_DATA_I, SERIAL_CLK_O, MSBFIRST);
    data = shift_in(SERIAL_DATA_I, SERIAL_CLK_O, MSBFIRST);

    // Print monitoring output and send it to PC
    char buf[32];
    sprintf(buf, "Got data: %hhX at address: %hX\n", data, addr);
    Serial.print(buf);
}

// Do nothing since logic analyzer operates at interrupt
void loop() {
}
