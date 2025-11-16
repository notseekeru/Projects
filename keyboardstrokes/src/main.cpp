#include <BleKeyboard.h>
#include <Arduino.h>

BleKeyboard bleKeyboard("ESP_KEYBOARD"); // Set the device name

const int buttonPin = 13; // Button connected to GPIO13
boolean oldPinState = LOW; // Track previous button state

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleKeyboard.begin(); // Start BLE keyboard advertising
  pinMode(buttonPin, INPUT_PULLDOWN); // Configure button pin with pull-down resistor
}

void loop() {
  if (bleKeyboard.isConnected()) {
    if (digitalRead(buttonPin) == HIGH) {
      if (oldPinState == LOW) {
        bleKeyboard.print(" "); // Send a spacebar when button is pressed
      }
      oldPinState = HIGH;
    } else {
      oldPinState = LOW;
    }
  }
  delay(10);
}   