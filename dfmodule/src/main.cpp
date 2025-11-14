/*
   DFPlayer Mini demo for Arduino Uno
   Library: DFRobotDFPlayerMini (by Makuna)
   Author: ChatGPT – 2025
*/

#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// SoftwareSerial: (RX, TX)  -> Arduino receives on RX, sends on TX
SoftwareSerial softSerial(11, 10);   // Uno RX=11, Uno TX=10
DFRobotDFPlayerMini player;

void setup() {
  // 1. Start hardware serial for debugging
  Serial.begin(115200);

  // 2. Start software serial for DFPlayer
  softSerial.begin(9600);

  // 3. Initialise DFPlayer
  Serial.println(F("\n=== DFPlayer Mini Demo ==="));
  if (!player.begin(softSerial, /*ack=*/true)) {
    Serial.println(F("Unable to begin DFPlayer! Check wiring / SD card."));
    while (true) delay(100);   // halt
  }
  Serial.println(F("DFPlayer Mini online."));

  // 4. Basic settings
  player.volume(20);   // 0…30
  player.EQ(DFPLAYER_EQ_NORMAL);
  player.outputSetting(true);   // enable amplifier chip

  // 5. Play the first track to confirm everything works
  player.play(1);      // file 001.mp3 (or 001/001.mp3 etc.)
  Serial.println(F("Playing track 1"));
}

void loop() {
  // ---- Simple serial command interface ----
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();

    if (cmd == "next") {
      player.next();
      Serial.println(F("→ Next"));
    }
    else if (cmd == "prev" || cmd == "previous") {
      player.previous();
      Serial.println(F("← Previous"));
    }
    else if (cmd.startsWith("play ")) {
      int track = cmd.substring(5).toInt();
      if (track > 0 && track < 1000) {
        player.play(track);
        Serial.print(F("Playing #")); Serial.println(track);
      } else {
        Serial.println(F("Invalid track (1-999)"));
      }
    }
    else if (cmd.startsWith("vol ")) {
      int vol = cmd.substring(4).toInt();
      if (vol >= 0 && vol <= 30) {
        player.volume(vol);
        Serial.print(F("Volume = ")); Serial.println(vol);
      } else {
        Serial.println(F("Volume 0-30"));
      }
    }
    else if (cmd == "pause") {
      player.pause();
      Serial.println(F("Paused"));
    }
    else if (cmd == "start" || cmd == "resume") {
      player.start();
      Serial.println(F("Resumed"));
    }
    else if (cmd == "stop") {
      player.stop();
      Serial.println(F("Stopped"));
    }
    else if (cmd == "status") {
      printDetail(player.readType(), player.read());
    }
    else {
      Serial.println(F("Commands: next, prev, play <n>, vol <0-30>, pause, start, stop, status"));
    }
  }

  // ---- Optional: read DFPlayer feedback (errors, finish, etc.) ----
  if (player.available()) {
    printDetail(player.readType(), player.read());
  }

  delay(10);
}

/* -------------------------------------------------------------
   Helper: pretty-print DFPlayer feedback
   ------------------------------------------------------------- */
void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println(F("USB Inserted!"));
      break;
    case DFPlayerUSBRemoved:
      Serial.println(F("USB Removed!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Track Finished, number: "));
      Serial.println(value);
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError: "));
      switch (value) {
        case Busy:          Serial.println(F("Busy")); break;
        case Sleeping:      Serial.println(F("Sleeping")); break;
        case SerialWrongStack: Serial.println(F("Serial Wrong Stack")); break;
        case CheckSumNotMatch: Serial.println(F("Check Sum Not Match")); break;
        case FileIndexOut:  Serial.println(F("File Index Out of Bound")); break;
        case Advertise:     Serial.println(F("Advertise")); break;
        default:            Serial.println(value); break;
      }
      break;
    default:
      // unknown / not needed
      break;
  }
}