

#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define TimeOut 0
#define WrongStack 1
#define DFPlayerCardInserted 2
#define DFPlayerCardRemoved 3
#define DFPlayerCardOnline 4
#define DFPlayerPlayFinished 5
#define DFPlayerError 6
#define DFPlayerUSBInserted 7
#define DFPlayerUSBRemoved 8
#define DFPlayerUSBOnline 9
#define DFPlayerCardUSBOnline 10
#define DFPlayerFeedBack 11

#define Busy 1
#define Sleeping 2
#define SerialWrongStack 3
#define CheckSumNotMatch 4
#define FileIndexOut 5
#define Advertise 7

void printDFPlayerDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println("DFPlayer: Time Out!"); break;
    case WrongStack:
      Serial.println("DFPlayer: Stack Wrong!"); break;
    case DFPlayerCardInserted:
      Serial.println("DFPlayer: Card Inserted!"); break;
    case DFPlayerCardRemoved:
      Serial.println("DFPlayer: Card Removed!"); break;
    case DFPlayerCardOnline:
      Serial.println("DFPlayer: Card Online!"); break;
    case DFPlayerPlayFinished:
      Serial.print("DFPlayer: Track Finished, number: "); Serial.println(value); break;
    case DFPlayerError:
      Serial.print("DFPlayer: Error: ");
      switch (value) {
        case Busy: Serial.println("Busy"); break;
        case Sleeping: Serial.println("Sleeping"); break;
        case SerialWrongStack: Serial.println("Serial Wrong Stack"); break;
        case CheckSumNotMatch: Serial.println("Check Sum Not Match"); break;
        case FileIndexOut: Serial.println("File Index Out of Bound"); break;
        case Advertise: Serial.println("Advertise"); break;
        default: Serial.println(value); break;
      }
      break;
    default:
      Serial.print("DFPlayer: Unknown status type="); Serial.print(type); Serial.print(", value="); Serial.println(value); break;
  }
}
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial mySerial(10, 11);        // RX, TX for DFPlayer Mini
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);                 // Set serial baud rate for DFPlayer Mini

  if (!myDFPlayer.begin(mySerial)) {    // Check if DFPlayer Mini is connected
    Serial.println("Unable to begin DFPlayer Mini.");
    while (true);
  }
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(30);                // Set volume to 20 (0-30 range)
  myDFPlayer.outputSetting(true, 0x1F); // Enable onboard amplifier, max gain
  delay(200);                           // Short delay for stability
  myDFPlayer.play(1);                   // Play first file once at startup
  Serial.println("DFPlayer: Playing track 1");
}

void loop() {
  // Print DFPlayer status/errors if available
  if (myDFPlayer.available()) {
    uint8_t type = myDFPlayer.readType();
    int value = myDFPlayer.read();
    printDFPlayerDetail(type, value);
    if (type == TimeOut) {
      Serial.println("DFPlayer: Retrying playback...");
      myDFPlayer.play(1);
      delay(200);
    }
  }
  delay(100);
} 