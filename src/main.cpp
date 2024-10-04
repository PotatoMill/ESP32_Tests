// Define pin assignments
#define TEC_PWM_PIN      25  // PWM pin for controlling power
#define TEC_IN1_PIN      13  // Control pin IN1
#define TEC_IN2_PIN      27  // Control pin IN2
// nEN pin is controlled via IO expander at pin 0

// Include necessary libraries
#include <Arduino.h>
#include "io_expander.h"  // Include the IO expander header

// Instantiate the PCA9534 IO expander
PCA9534 ioExpander(0x20); // Replace 0x20 with your actual I2C address if different

// Variables
uint8_t dutyCycle = 0; // Duty cycle (0 to 100)

// Function prototypes
void processCommand(String cmd);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("TEC Controller Initialized.");

  // Initialize IO Expander
  ioExpander.begin();

  // Set pin modes
  pinMode(TEC_PWM_PIN, OUTPUT);
  pinMode(TEC_IN1_PIN, OUTPUT);
  pinMode(TEC_IN2_PIN, OUTPUT);

  // Initialize pins
  digitalWrite(TEC_IN1_PIN, LOW);
  digitalWrite(TEC_IN2_PIN, LOW);
  ioExpander.setPinHigh(0); // Set nEN high to disable TEC initially

  // Configure PWM on the PWM pin
  ledcSetup(0, 5000, 8); // Channel 0, 5 kHz frequency, 8-bit resolution
  ledcAttachPin(TEC_PWM_PIN, 0);
  ledcWrite(0, 0); // Start with 0% duty cycle
}

void loop() {
  // Check if data is available on Serial
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim(); // Remove any leading/trailing whitespace
    processCommand(cmd);
  }
}

// Function to process commands received over UART
void processCommand(String cmd) {
  // Convert command to lowercase
  cmd.toLowerCase();

  if (cmd.startsWith("set duty ")) {
    // Example command: set duty 50
    int value = cmd.substring(9).toInt();
    if (value >= 0 && value <= 100 && value % 10 == 0) {
      dutyCycle = value;
      uint8_t pwmValue = map(dutyCycle, 0, 100, 0, 255);
      ledcWrite(0, pwmValue);
      Serial.println("Duty cycle set to " + String(dutyCycle) + "%");
    } else {
      Serial.println("Invalid duty cycle value. Must be 0, 10, ..., 100.");
    }
  } else if (cmd.startsWith("set in1 ")) {
    // Example command: set in1 high
    String value = cmd.substring(8);
    if (value == "high") {
      digitalWrite(TEC_IN1_PIN, HIGH);
      Serial.println("IN1 pin set to HIGH");
    } else if (value == "low") {
      digitalWrite(TEC_IN1_PIN, LOW);
      Serial.println("IN1 pin set to LOW");
    } else {
      Serial.println("Invalid IN1 value. Use 'high' or 'low'.");
    }
  } else if (cmd.startsWith("set in2 ")) {
    // Example command: set in2 high
    String value = cmd.substring(8);
    if (value == "high") {
      digitalWrite(TEC_IN2_PIN, HIGH);
      Serial.println("IN2 pin set to HIGH");
    } else if (value == "low") {
      digitalWrite(TEC_IN2_PIN, LOW);
      Serial.println("IN2 pin set to LOW");
    } else {
      Serial.println("Invalid IN2 value. Use 'high' or 'low'.");
    }
  } else if (cmd.startsWith("set nen ")) {
    // Example command: set nen high
    String value = cmd.substring(8);
    if (value == "high") {
      ioExpander.setPinHigh(0); // nEN is active low
      Serial.println("nEN pin set to HIGH (TEC Disabled)");
    } else if (value == "low") {
      ioExpander.setPinLow(0);
      Serial.println("nEN pin set to LOW (TEC Enabled)");
    } else {
      Serial.println("Invalid nEN value. Use 'high' or 'low'.");
    }
  } else {
    Serial.println("Unknown command.");
  }
}