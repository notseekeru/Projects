#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <cstdlib>
#include <ctime>
#include "esp_system.h"

#define BUTTON_PIN_18 18    // START GAME
#define BUTTON_PIN_19 19    // DECREASE BET (changed from 5)
#define BUTTON_PIN_4  4     // HIT
#define BUTTON_PIN_2  23    // STAND (changed from 2)
#define BUTTON_PIN_15 15    // INCREASE BET
#define BUZZER_PIN    5     // BUZZEN PIN

Adafruit_SSD1306 display(128, 64, &Wire, -1);

// VARIABLES
int playerMoney = 30;
int playerBet = 10;
int randNumber;
int playerTotal = 0;
int dealerTotal = 0;


int curSt_1, curSt_2, curSt_3, curSt_4, curSt_5;
int lastSt_1, lastSt_2, lastSt_3, lastSt_4, lastSt_5 = LOW;
int result;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 800;


bool inMenu = true;
bool inGame = false;
bool inPlay = false;
bool gameOver = false;


// FUNCTIONS

void centerText(const char* text, int y) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h); // Calculate text bounds (width and height)
  // Calculate the starting X position for center alignment
  int x = (128 - w) / 2;
  // Set the cursor position
  display.setCursor(x, y);
  // Print the text
  display.println(text);
}

int drawCard() {
    int card = random(1, 13);
    if (card > 10) card = 10;
    return card;
}

void seedCard() {
    playerTotal = drawCard() + drawCard();
    dealerTotal = drawCard();
}

int waitForButton() {

    curSt_1 = digitalRead(BUTTON_PIN_18);
    curSt_2 = digitalRead(BUTTON_PIN_19);
    curSt_3 = digitalRead(BUTTON_PIN_4);
    curSt_4 = digitalRead(BUTTON_PIN_2);
    curSt_5 = digitalRead(BUTTON_PIN_15);

    result = -1;

    if (lastSt_1 == HIGH && curSt_1 == LOW) {
        result = 1;
    }
    else if (lastSt_2 == HIGH && curSt_2 == LOW) {
        result = 2;
    }
    else if (lastSt_3 == HIGH && curSt_3 == LOW) {
        result = 3;
    }
    else if (lastSt_4 == HIGH && curSt_4 == LOW) {
        result = 4;
    }
    else if (lastSt_5 == HIGH && curSt_5 == LOW) {
        result = 5;
    }

    if (result != -1) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
            lastDebounceTime = millis();
        } else {
            result = -1;  // Ignore button press within debounce period
        }
    }

    lastSt_1 = curSt_1;
    lastSt_2 = curSt_2;
    lastSt_3 = curSt_3;
    lastSt_4 = curSt_4;
    lastSt_5 = curSt_5;

    return result;
}

void displayStartMenu() {
    display.clearDisplay();

    centerText("=== START ===", 2);
    centerText("PLAY?", 25);
    centerText("C = YES | NO = D", 50);
    
    display.display();
}

int renderGame() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("Money:$");
    display.print(playerMoney);
    display.print(" ");
    display.print("Bet:$");
    display.println(playerBet);

    display.println("");
    display.print("Player Total: ");
    display.println(playerTotal);
    display.print("Dealer Total: ");
    display.print(dealerTotal);
    display.print(" + ??");

    display.setCursor(0, 40);
    display.println("2:+ Bet | 3:Hit");
    display.setCursor(0, 50);
    display.println("4:Stand | 5: + Bet");
    
    display.display();
    return 0;
}

void renderGameOver() {
    delay(1000);
    display.clearDisplay();
    centerText("You lost all", 10);
    centerText("your money!", 20);
    centerText("=== GAME OVER ===", 40);
    centerText("Returning to Menu...", 50);
    display.display();
    delay(1500);
    inMenu = true;
    inGame = false;
    playerBet = 10;
    playerMoney = 30;
    playerTotal = 0;
    dealerTotal = 0;
    displayStartMenu();
}

void renderPlay() {
    while (dealerTotal < 17) {
        display.clearDisplay();
        dealerTotal += drawCard();
        centerText("Player:", 10);
        centerText(String(playerTotal).c_str(), 20);
        centerText("Dealer:", 30);
        centerText(String(dealerTotal).c_str(), 40);
        display.display();
        delay(1000);
    }
    
    if (dealerTotal > 21 && playerTotal <= 21) {
        display.clearDisplay();
        centerText("You win! Dealer Bust!", 30);
        display.display();
        playerMoney += playerBet;
        delay(1000);
        
    } else {
        if (playerTotal > dealerTotal) {
            display.clearDisplay();
            centerText("You WIN by Totals!",30);
            centerText(":)))",40);
            display.display();
            playerMoney += playerBet;
            delay(1500);
            
        }
        if (playerTotal < dealerTotal) {
            display.clearDisplay();
            centerText("You LOST by Totals!",30);
            centerText(":(((",40);
            display.display();
            playerMoney -= playerBet;
            delay(1500);
        }
    }
    inGame = true;
    if (playerBet > playerMoney) {
        playerBet = 10;
    }
    playerTotal = 0;
    dealerTotal = 0;
    seedCard();
    renderGame();
}

// ACTUAL LOGIC OF THE ESP32
void setup() {
    Serial.begin(115200);

    pinMode(BUTTON_PIN_18, INPUT_PULLUP);
    pinMode(BUTTON_PIN_19, INPUT_PULLUP);
    pinMode(BUTTON_PIN_4, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);
    pinMode(BUTTON_PIN_15, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
    for(;;); } // ACTIVATES OLED DISPLAY
    
    displayStartMenu();
}

void loop() {
    int ans = waitForButton();

    if (ans == 1){
        digitalWrite(BUZZER_PIN, HIGH);
        delay(10000);
        digitalWrite(BUZZER_PIN,LOW);
    }

    // MENU STATE
    if (inMenu && !inGame) {
        if (ans == 3) {
            inMenu = false;
            inGame = true;
            seedCard();
            renderGame();
        }
        else if (ans == 4) {
            display.clearDisplay();
            centerText("GOODBYE!", 30);
            display.display();
            delay(1000);
            digitalWrite(BUZZER_PIN, HIGH);
            while(true) {
                delay(1000);
            }
        }
    }

    // IF IN GAME
    if (inGame && !inMenu) {
        if (ans == 2) {
            if (playerBet >= 20) {
                playerBet -= 10;
            } else {
                digitalWrite(BUZZER_PIN, HIGH);
                delay(100);
                digitalWrite(BUZZER_PIN,LOW);
            }
            renderGame();
        }
        if (ans == 3) {
            Serial.println("3");
            playerTotal += drawCard();
            renderGame();

            if (playerTotal > 21) {
                delay(500);
                display.clearDisplay();
                playerMoney -= playerBet;
                centerText("You busted!", 30);
                display.display();
                playerTotal = 0;
                dealerTotal = 0;
                delay(1200);
                seedCard();
                renderGame();
            }
        }
        if (ans == 4) {
            Serial.println("4");
            inPlay = true;
            inGame = false;
            renderPlay();
            delay(500);
            renderGame();
        }
        if (ans == 5) {
            if (playerBet >= playerMoney) {
                digitalWrite(BUZZER_PIN, HIGH);
                delay(100);
                digitalWrite(BUZZER_PIN,LOW);
            } else {
                playerBet += 10;
                renderGame();
            }
        }
    }

    if (playerMoney <= 0) { // ENDS GAME
        renderGameOver();
    }
}