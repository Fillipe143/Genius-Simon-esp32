#include <Arduino.h>
#include <cstdlib>
#include <cstring>

using namespace std;

#define START_LED            4
#define HIT_LED              15

#define RED_LED              27
#define GREEN_LED            14
#define BLUE_LED             12
#define YELLOW_LED           13

#define RED_BTN              35
#define GREEN_BTN            32
#define BLUE_BTN             33
#define YELLOW_BTN           25

#define LED_ON_TIME          400
#define LED_OFF_TIME         500
#define MIN_BTN_PRESSED_TIME 500

const char colors[] = "RGBY";
String colorSequence;
String userSequence;
int sequenceIndex;

int previusTime = -LED_OFF_TIME;

bool hasPressedRedBtn;
bool hasPressedGreenBtn;
bool hasPressedBlueBtn;
bool hasPressedYellowBtn;

int redBtnPreviusTime = -MIN_BTN_PRESSED_TIME;
int greenBtnPreviusTime = -MIN_BTN_PRESSED_TIME;
int blueBtnPreviusTime = -MIN_BTN_PRESSED_TIME;
int yellowBtnPreviusTime = -MIN_BTN_PRESSED_TIME;

char getRandomColor() {
  srand((unsigned)time(0));
  return colors[rand() % strlen(colors)];
}

int getLedPinFromColor(char color) {
  switch (color) {
    case 'R': return RED_LED;
    case 'G': return GREEN_LED;
    case 'B': return BLUE_LED;
    case 'Y': return YELLOW_LED;
    default: return 0;
  }
}

char getColorFromBtnPin(int pin) {
  switch (pin) {
    case RED_BTN: return 'R';
    case GREEN_BTN: return 'G';
    case BLUE_BTN: return 'B';
    case YELLOW_BTN: return 'Y';
    default: return 'N';
  }
}

void nextLevel() {
  userSequence = "";
  sequenceIndex = 0;
  colorSequence += getRandomColor();
}

void startNewGame() {
  userSequence = "";
  colorSequence = "";
  nextLevel();

  digitalWrite(START_LED, HIGH);
  delay(1000);
  digitalWrite(START_LED, LOW);
}

void showColorSequence() {
  const int currentTime = millis();
  const int deltaTime = currentTime - previusTime;
  
  const int ledPin = getLedPinFromColor(colorSequence[sequenceIndex]);
  const int ledStatus = digitalRead(ledPin);

  if (ledStatus == LOW && deltaTime >= LED_OFF_TIME) {
    previusTime = currentTime;
    digitalWrite(ledPin, HIGH);
  } else if (ledStatus == HIGH && deltaTime >= LED_ON_TIME) {
    previusTime = currentTime;
    digitalWrite(ledPin, LOW);

    sequenceIndex++;
  }

}

void onClick(int btnPin, int ledPin, bool &hasPressed, int &btnPreviusTime) {
  const int currentTime = millis();
  const int btnStatus = digitalRead(btnPin) ? LOW : HIGH;

  if (btnStatus == HIGH) {
    hasPressed = true;
    
    digitalWrite(ledPin, HIGH);
  } else if (hasPressed && currentTime - btnPreviusTime >= MIN_BTN_PRESSED_TIME) {
    previusTime = currentTime;
    hasPressed = false;
    
    digitalWrite(ledPin, LOW);
    userSequence += getColorFromBtnPin(btnPin);

    if (!colorSequence.startsWith(userSequence)) startNewGame();
    else if (colorSequence == userSequence) {
      digitalWrite(HIT_LED, HIGH);
      delay(1000);
      digitalWrite(HIT_LED, LOW);
      nextLevel();
    }
  }
}

void readUserSequence() {
  onClick(RED_BTN, RED_LED, hasPressedRedBtn, redBtnPreviusTime);
  onClick(GREEN_BTN, GREEN_LED, hasPressedGreenBtn, greenBtnPreviusTime);
  onClick(BLUE_BTN, BLUE_LED, hasPressedBlueBtn, blueBtnPreviusTime);
  onClick(YELLOW_BTN, YELLOW_LED, hasPressedYellowBtn, yellowBtnPreviusTime);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(START_LED, OUTPUT);
  pinMode(HIT_LED, OUTPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  
  
  pinMode(RED_BTN, INPUT);
  pinMode(GREEN_BTN, INPUT);
  pinMode(BLUE_BTN, INPUT);
  pinMode(YELLOW_BTN, INPUT);

  startNewGame();
}

void loop() {
  if (sequenceIndex < colorSequence.length()) showColorSequence();
  else readUserSequence();
}