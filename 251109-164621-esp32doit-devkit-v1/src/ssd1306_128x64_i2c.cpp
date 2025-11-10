#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ezButton.h> 

const uint8_t OLED_ADDR = 0x3C;

const int buttonPin = 4;
const int ledPin = 18;
const int sdaPin = 21;
const int sclPin = 22;

int count = 0;

Adafruit_SSD1306 display(128, 64, &Wire, 16);

void setup() {
  
  Serial.begin(115200);

  Wire.begin(sdaPin, sclPin);

  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay(); // clear display
  display.setTextSize(3); // set text size
  display.setTextColor(SSD1306_WHITE); // set text color
  display.setCursor(0, 0); // set text location
  display.display(); // display initial content

  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

}

// updates the display with the current count value
void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  if (count < 10) display.print("0");
  display.print(count);
  display.display();
}

void loop() {

  if (digitalRead(buttonPin) == LOW) {
    digitalWrite(ledPin, LOW);
    Serial.println("→ LED ON");
    count += 1;
    updateDisplay();
    delay(50);
  }
  if (digitalRead(buttonPin) == HIGH) {
    digitalWrite(ledPin, HIGH);
    Serial.println("→ LED OFF");
    delay(50);
  }
}