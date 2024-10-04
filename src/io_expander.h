// PCA9534.h

#pragma once

#include <Arduino.h>
#include <Wire.h>

class PCA9534 {
public:
    PCA9534(uint8_t address, uint8_t sda = 21, uint8_t scl = 22);

    void begin();
    void togglePin(uint8_t pin);
    void setPinHigh(uint8_t pin);
    void setPinLow(uint8_t pin);
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);

private:
    uint8_t _address;
    uint8_t _sda;
    uint8_t _scl;
    uint8_t _outputState;  // Tracks the current output state of all pins
    
    // Define the PCA9534 registers
    static constexpr uint8_t INPUT_PORT = 0x00;
    static constexpr uint8_t OUTPUT_PORT = 0x01;
    static constexpr uint8_t POLARITY_INV = 0x02;
    static constexpr uint8_t CONFIG = 0x03;
};