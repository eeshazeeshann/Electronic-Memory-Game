#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int buttons[4] = {12, 14, 27, 26};
int leds[4]    = {18, 19, 21, 5};
#define BUZZER 25

#define MAX_LEVEL 100
int pattern[MAX_LEVEL + 1];
int userInput[MAX_LEVEL + 1];

int level = 1;
int difficulty = 2;
int soundPack = 1;
int lives = 3;

int baseSpeed[] = {0, 700, 500, 300, 120};

void playTone(int n) {
  int freq = 0;
 
  switch(soundPack) {
    case 1: freq = 600 + (n * 120); break;
    case 2: freq = 400 + (n * 200); break;
    case 3: freq = 200 + (n * 300); break;
    case 4: freq = 900 + (n * 90);  break;
  }

  long delayValue = 1000000 / freq / 2; 
  long numCycles = freq * 150 / 1000; 
  
  for (long i = 0; i < numCycles; i++) {
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(delayValue);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(delayValue);
  }
  delay(50); 
}

void playSimpleTone(int freq, int duration) {
  long delayValue = 1000000 / freq / 2;
  long numCycles = freq * duration / 1000;
  
  for (long i = 0; i < numCycles; i++) {
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(delayValue);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(delayValue);
  }
}

void introAnimation() {
  lcd.clear();

  for (int k = 0; k < 2; k++) {
    for (int i = 0; i < 4; i++) {
      digitalWrite(leds[i], HIGH);
      playSimpleTone(300 + (i * 120), 60);
      digitalWrite(leds[i], LOW);
    }
    for (int i = 3; i >= 0; i--) {
      digitalWrite(leds[i], HIGH);
      playSimpleTone(300 + (i * 120), 60);
      digitalWrite(leds[i], LOW);
    }
  }

  for (int t = 0; t < 2; t++) {
    lcd.clear();
    lcd.setCursor(1,0); lcd.print(">>> MEMORY <<<");
    lcd.setCursor(3,1); lcd.print("   GAME    ");
    delay(200);
 
    lcd.clear();
    delay(150);
  }

  lcd.clear();
  lcd.setCursor(1,0); lcd.print(">>> MEMORY <<<");
  lcd.setCursor(3,1); lcd.print("   GAME    ");
  delay(800);
 
  lcd.clear();
}

void levelUpFX() {
  for(int i=0; i<4; i++){
    digitalWrite(leds[i], HIGH);
    delay(70);
    digitalWrite(leds[i], LOW);
  }
}

void outroAnimation() {
  lcd.clear();

  for (int blast = 0; blast < 3; blast++) {
    for (int j = 0; j < 4; j++) digitalWrite(leds[j], HIGH);
    playSimpleTone(200 + blast * 40, 300);
 
    for (int j = 0; j < 4; j++) digitalWrite(leds[j], LOW);
    delay(200);
  }

  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("GAME OVER");
  lcd.setCursor(0,1);
  lcd.print("Restarting...");

  for (int fade = 0; fade < 3; fade++) {
    for (int i = 0; i < 4; i++) digitalWrite(leds[i], HIGH);
    playSimpleTone(150, 400);
 
    for (int i = 0; i < 4; i++) digitalWrite(leds[i], LOW);
    delay(300);
  }
 
  lcd.clear();
  delay(600);
}

void generatePattern() {
  pattern[level] = random(0, 4);
}

bool readUserInput() {
  int pos = 1;
 
  while(pos <= level){
    for(int i=0; i<4; i++){
      if(digitalRead(buttons[i]) == LOW) {
 
        userInput[pos] = i;
 
        digitalWrite(leds[i], HIGH);
        playTone(i);
        digitalWrite(leds[i], LOW);
 
        while(digitalRead(buttons[i]) == LOW) {
          delay(10);
        }
 
        if(userInput[pos] != pattern[pos]) return false;
 
        pos++;
      }
    }
    delay(1);
  }
  return true;
}

void showPattern() {
  int spd = baseSpeed[difficulty] - (level * 3);
  if(spd < 80) spd = 80;
 
  for(int i=1; i<=level; i++){
    int idx = pattern[i];
    digitalWrite(leds[idx], HIGH);
    playTone(idx);
    digitalWrite(leds[idx], LOW);
    delay(spd);
  }
}

void chooseDifficulty() {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Select Difficulty");
  lcd.setCursor(0,1); lcd.print("  1E 2M 3H 4VH");
 
  while(true){
    for(int i=0; i<4; i++){
      if(digitalRead(buttons[i]) == LOW) {
        difficulty = i+1;
        delay(250);
        return;
      }
    }
    delay(1);
  }
}
 
void chooseSoundPack() {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Select SoundPack");
  lcd.setCursor(0,1); lcd.print("  1C 2A 3B 4SF");
 
  while(true){
    for(int i=0; i<4; i++){
      if(digitalRead(buttons[i]) == LOW) {
        soundPack = i+1;
        delay(250);
        return;
      }
    }
    delay(1);
  }
}

void setup() {
  Serial.begin(115200); 

  Wire.begin(23, 22); 
  
  lcd.init();
  lcd.backlight();
 
  for(int i=0; i<4; i++){
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(leds[i], OUTPUT);
  }
  pinMode(BUZZER, OUTPUT);

  randomSeed(analogRead(34)); 
  
  //this to fix hearts display
  byte heart[8] = {0b00000,0b01010,0b11111,0b11111,0b11111,0b01110,0b00100,0b00000};
  lcd.createChar(3, heart);

  introAnimation();
 
  chooseDifficulty();
  chooseSoundPack();
}

void loop() {
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Level ");
  lcd.print(level);
 
  lcd.setCursor(0,1);
  lcd.print("Lives:");
  for(int i=0;i<lives;i++) lcd.print((char)3);
 
  delay(700);
 
  generatePattern();
  showPattern();
 
  if(readUserInput()) {
    levelUpFX();
    level++;
    if(level > MAX_LEVEL) level = 1;
  }
  else {
    lives--;
 
    if(lives <= 0){
      outroAnimation();     
      level = 1;
      lives = 3;
    } else {
      lcd.clear();
      lcd.setCursor(4,0); lcd.print("WRONG!");
      lcd.setCursor(2,1); lcd.print("Life Lost!");
      playSimpleTone(150, 350);
      delay(1000);
    }
  }
}