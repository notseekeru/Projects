#include <ezButton.h>
#include <Arduino.h>
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;

#define VRX_PIN  39
#define VRY_PIN  36
#define SW_PIN   17

int led1 = 19;
int led2 = 21;
int led3 = 22;
int led4 = 23;

ezButton button(SW_PIN);

int xValue= 0;
int yValue = 0;
int bValue = 0;

void setup() {
  Serial.begin(115200);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  analogSetAttenuation(ADC_11db);
  button.setDebounceTime(50);
  bleKeyboard.begin();
}

void loop() {
  button.loop();

  yValue= analogRead(VRX_PIN);
  xValue = analogRead(VRY_PIN);

  bValue = button.getState();

  int centerX = 2047;
  int centerY = 2047;
  int xDiff = abs(xValue - centerX);
  int yDiff = abs(yValue - centerY);

  bool isAtRest = (xDiff < 300 && yDiff < 300);

  if (!isAtRest) {
    if (yValue >= 2100) {
      bleKeyboard.press(KEY_UP_ARROW);
      Serial.println("W was pushed");
    }
    else if (xValue <= 1500) {
      bleKeyboard.press(KEY_LEFT_ALT);
      Serial.println("A was pushed");
    }
    else if (yValue <= 1500) {
      bleKeyboard.press(KEY_DOWN_ARROW);
      Serial.println("S was pushed");
    }
    else if (xValue >= 2100) {
      bleKeyboard.press(KEY_RIGHT_ARROW);
      Serial.println("D was pushed");
    }
    else {
      bleKeyboard.releaseAll();
    }
  } else {
    // Joystick at rest - release all keys
    bleKeyboard.releaseAll();
  }
}