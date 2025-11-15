#include <ezButton.h>

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

  if (button.isPressed()) {
    Serial.println("The button is pressed");
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
  }

  if (button.isReleased()) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
  }
  Serial.print("x = ");
  Serial.print(xValue);
  Serial.print(", y = ");
  Serial.print(yValue);
  Serial.print(" : button = ");
  Serial.print(bValue);
  Serial.print(" : isAtRest = ");
  Serial.println(isAtRest ? "YES" : "NO");
}