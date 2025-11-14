#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"

HardwareSerial DFPlayerSerial(2);
DFRobotDFPlayerMini player;

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);

  Serial.println(F("\n=== DFPlayer Mini + ESP32 ==="));

  // === STEP 1: Start UART2 ===
  DFPlayerSerial.begin(9600, SERIAL_8N1, 27, 26);
  Serial.println(F("UART2 started (RX=27, TX=26)"));

  // === STEP 2: WAIT LONGER ===
  Serial.println(F("Waiting 3 seconds for DFPlayer boot..."));
  delay(3000);  // <--- INCREASED TO 3 SECONDS!

  // === STEP 3: FLUSH ANY GARBAGE ===
  while (DFPlayerSerial.available()) {
    Serial.printf("Flushing: 0x%02X\n", DFPlayerSerial.read());
  }

  // === STEP 4: INITIALIZE DFPLAYER ===
  Serial.println(F("Initializing DFPlayer..."));
  if (!player.begin(DFPlayerSerial, true, true)) {  // isACK=true, doReset=true
    Serial.println(F("DFPlayer FAILED!"));
    Serial.println(F("  → Check: VCC=5V, proper wiring, SD card inserted"));
    while (true) delay(100);
  }

  Serial.println(F("DFPlayer ONLINE!"));

  delay(500);  // Wait after initialization

  // === STEP 5: Settings ===
  player.volume(20);
  player.outputDevice(DFPLAYER_DEVICE_SD);

  // === STEP 6: PLAY YOUR FILE ===
  player.playFolder(1, 1);  // folder 01 → 001.mp3
  Serial.println(F("Playing 01/001.mp3"));
}

void loop() {
  if (player.available()) {
    uint8_t type = player.readType();
    int value = player.read();

    switch (type) {
      case DFPlayerPlayFinished:
        Serial.print(F("Finished: ")); Serial.println(value);
        break;
      case DFPlayerError:
        Serial.print(F("ERROR: "));
        if (value == FileIndexOut) Serial.println(F("File not found"));
        else Serial.println(value);
        break;
      case DFPlayerCardOnline:
        Serial.println(F("SD Card Online"));
        break;
    }
  }
}