// PCA9534.cpp

#include "io_expander.h"

PCA9534::PCA9534(uint8_t address, uint8_t sda, uint8_t scl)
: _address(address), _sda(sda), _scl(scl) {}

void PCA9534::begin() {
    Wire.begin(_sda, _scl);
    // Set all pins as outputs (0 means output, 1 means input)
    writeRegister(CONFIG, 0x00);
    // Set all outputs to low initially
    writeRegister(OUTPUT_PORT, 0x00);
}

void PCA9534::togglePin(uint8_t pin) {
    if (pin > 7) return; // PCA9534 has only 8 GPIOs, from 0 to 7
    
    uint8_t currentOutput = readRegister(OUTPUT_PORT);
    currentOutput ^= (1 << pin); // Toggle the specific pin
    
    writeRegister(OUTPUT_PORT, currentOutput);
}

void PCA9534::setPinHigh(uint8_t pin) {
    if (pin > 7) return; // PCA9534 has only 8 GPIOs, from 0 to 7

    _outputState |= (1 << pin); // Set the specific pin high
    writeRegister(OUTPUT_PORT, _outputState);
}

void PCA9534::setPinLow(uint8_t pin) {
    if (pin > 7) return; // PCA9534 has only 8 GPIOs, from 0 to 7

    _outputState &= ~(1 << pin); // Set the specific pin low
    writeRegister(OUTPUT_PORT, _outputState);
}

void PCA9534::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t PCA9534::readRegister(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom((int)_address, 1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}