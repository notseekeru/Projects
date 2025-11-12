#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Create an OLED display object
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

#define BUZZER_PIN 11
#define HIT_BUTTON_PIN 2
#define STAND_BUTTON_PIN 3
#define INCREASE_BET_BUTTON_PIN 4
#define DECREASE_BET_BUTTON_PIN 5
#define START_BUTTON_PIN 6
#define RESET_BUTTON_PIN 7

const char* suits = "HDSC";
const char* values = "234567891JQKA";
int deck[52];
int playerHand[10];
int dealerHand[10];
int playerHandSize = 0;
int dealerHandSize = 0;
int playerCash = 100;
int playerBet = 10;
int currentDeckIndex = 0;
char gameMessage[20] = "";
bool gameEnded = false;
bool inMenu = true;

void initDeck() {
  for (int i = 0; i < 52; i++) {
    deck[i] = i;
  }
}

void shuffleDeck() {
  for (int i = 0; i < 52; i++) {
    int randIndex = random(0, 52);
    int temp = deck[i];
    deck[i] = deck[randIndex];
    deck[randIndex] = temp;
  }
  currentDeckIndex = 0;
}

void newGame() {
  playerHandSize = 0;
  dealerHandSize = 0;
  strcpy(gameMessage, "");
  if (currentDeckIndex + 4 > 52) {
    shuffleDeck();
  }
  playerHand[playerHandSize++] = deck[currentDeckIndex++];
  playerHand[playerHandSize++] = deck[currentDeckIndex++];
  dealerHand[dealerHandSize++] = deck[currentDeckIndex++];
  dealerHand[dealerHandSize++] = deck[currentDeckIndex++];
}

void hit() {
  if (playerHandSize < 10 && currentDeckIndex < 52) {
    playerHand[playerHandSize++] = deck[currentDeckIndex++];
    if (calculateHandValue(playerHand, playerHandSize) > 21) {
      resolveGame();
    }
  }
}

void stand() {
  while (calculateHandValue(dealerHand, dealerHandSize) < 17 && dealerHandSize < 10) {
    dealerHand[dealerHandSize++] = deck[currentDeckIndex++];
  }
  resolveGame();
}

void increaseBet() {
  if (playerBet + 10 <= playerCash) {
    playerBet += 10;
  } else {
    tone(BUZZER_PIN, 1000, 200); // Buzz if player tries to bet more than they have
  }
}

void decreaseBet() {
  if (playerBet - 10 >= 10) {
    playerBet -= 10;
  } else {
    tone(BUZZER_PIN, 1000, 200); // Buzz if player tries to bet less than the minimum
  }
}

void resolveGame() {
  int playerValue = calculateHandValue(playerHand, playerHandSize);
  int dealerValue = calculateHandValue(dealerHand, dealerHandSize);
  if (playerValue > 21 || (dealerValue <= 21 && dealerValue > playerValue)) {
    // Player loses
    playerCash -= playerBet;
    tone(BUZZER_PIN, 2000, 500); // Long buzz for losing
    strcpy(gameMessage, "Dealer Wins!");
  } else if (dealerValue > 21 || playerValue > dealerValue) {
    // Player wins
    playerCash += playerBet;
    tone(BUZZER_PIN, 1000, 500); // Short buzz for winning
    strcpy(gameMessage, "Player Wins!");
  } else {
    // Draw
    tone(BUZZER_PIN, 1500, 500); // Medium buzz for draw
    strcpy(gameMessage, "It's a Draw!");
  }
  gameEnded = true;
  displayGameState(); // Ensure the game message is displayed
}

int calculateHandValue(int* hand, int handSize) {
  int value = 0;
  int aces = 0;
  for (int i = 0; i < handSize; i++) {
    int cardValue = (hand[i] % 13) + 1;
    if (cardValue > 10) {
      cardValue = 10;
    }
    if (cardValue == 1) {
      aces++;
      cardValue = 11;
    }
    value += cardValue;
  }
  while (value > 21 && aces > 0) {
    value -= 10;
    aces--;
  }
  return value;
}

void displayCard(int card, int x, int y) {
  char suit = suits[card / 13];
  char value = values[card % 13];
  
  display.setCursor(x, y);
  display.print(value);
  display.print(suit);
}

void displayHand(int* hand, int handSize, int x, int y) {
  for (int i = 0; i < handSize; i++) {
    displayCard(hand[i], x + (i * 12), y);
  }
}

void displayGameState() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.print("Cash: $");
  display.print(playerCash);
  
  display.setCursor(0, 10);
  display.print("Bet: $");
  display.print(playerBet);

  display.setCursor(0, 20);
  display.print("Player: ");
  displayHand(playerHand, playerHandSize, 0, 30);

  display.setCursor(90, 20);
  display.print("(");
  display.print(calculateHandValue(playerHand, playerHandSize));
  display.print(")");

  display.setCursor(0, 40);
  display.print("Dealer: ");
  displayHand(dealerHand, dealerHandSize, 0, 50);

  display.setCursor(90, 40);
  display.print("(");
  display.print(calculateHandValue(dealerHand, dealerHandSize));
  display.print(")");

  // Display game message
  if (gameMessage[0] != '\0') {
    display.setCursor(0, 56);
    display.print(gameMessage);
  }
  
  display.display();
}

void displayMenu() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.print("Menu");
  
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("Press START to begin");
  
  display.setCursor(0, 30);
  display.print("Press RESET to reset");
  
  display.display();
}


void setup() {
  Serial.begin(9600);
  
  // Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(1000);
  display.clearDisplay();

  // Initialize buttons
  pinMode(HIT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STAND_BUTTON_PIN, INPUT_PULLUP);
  pinMode(INCREASE_BET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DECREASE_BET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize deck
  initDeck();
  shuffleDeck();
  
  displayMenu();
}

void loop() {
  if (inMenu) {
    if (digitalRead(START_BUTTON_PIN) == LOW) {
      tone(BUZZER_PIN, 300, 100);
      inMenu = false;
      newGame();
      displayGameState();
      delay(100);
    }
  } else {
    if (gameEnded) {
      displayGameState(); // Ensure the game message is displayed
      delay(1000); // Display game message for 1 second
      gameEnded = false;
      newGame();
      displayGameState();
      return;
    }

    if (digitalRead(HIT_BUTTON_PIN) == LOW) {
      tone(BUZZER_PIN, 300, 100);
      hit();
      displayGameState();
      delay(100);
    }
    
    if (digitalRead(STAND_BUTTON_PIN) == LOW) {
      stand();
      displayGameState();
      delay(100);
    }

    if (digitalRead(INCREASE_BET_BUTTON_PIN) == LOW) {
      tone(BUZZER_PIN, 300, 100);
      increaseBet();
      displayGameState();
      delay(100);
    }
    
    if (digitalRead(DECREASE_BET_BUTTON_PIN) == LOW) {
      tone(BUZZER_PIN, 300, 100);
      decreaseBet();
      displayGameState();
      delay(100);
    }

    if (digitalRead(RESET_BUTTON_PIN) == LOW) {
      tone(BUZZER_PIN, 300, 100);
      inMenu = true;
      displayMenu();
      playerCash = 100;
      playerBet = 10;
      delay(100);
    }
  }
}