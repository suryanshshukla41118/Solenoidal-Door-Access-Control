#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// ---------------- LCD Setup (16x2, parallel) ----------------
// Wiring: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ---------------- Keypad Setup (4x4) ----------------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// NOTE: Adjust these pins to match your wiring / free pins on the Nano.
// Avoid pins already used by the LCD above (2,3,4,5,11,12).
byte rowPins[ROWS] = {A0, A1, A2, A3};   // R1, R2, R3, R4
byte colPins[COLS] = {6, 7, 8, 9};       // C1, C2, C3, C4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------------- Output Pins ----------------
const int RELAY_PIN   = 10;   // Drives relay -> solenoid lock (Active LOW relay module typical)
const int BUZZER_PIN  = A4;
const int GREEN_LED   = A5;
const int RED_LED     = 13;

// ---------------- Password / Security Settings ----------------
const byte PASSWORD_LENGTH = 4;      // 4-digit PIN
char storedPassword[PASSWORD_LENGTH + 1] = "1234"; // default PIN
char inputPassword[PASSWORD_LENGTH + 1];
byte inputIndex = 0;

const unsigned long UNLOCK_DURATION = 4000;   // ms solenoid stays unlocked
const byte MAX_ATTEMPTS = 3;
byte failedAttempts = 0;
const unsigned long LOCKOUT_DURATION = 15000; // ms lockout after max failed attempts

const int EEPROM_ADDR = 0; // where password is stored in EEPROM

// ---------------- Setup ----------------
void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  lockDoor(); // ensure locked state at startup (safety default)

  lcd.begin(16, 2);
  loadPasswordFromEEPROM();

  showIdleScreen();
}

// ---------------- Main Loop ----------------
void loop() {
  char key = keypad.getKey();

  if (key) {
    tone(BUZZER_PIN, 1000, 50); // short beep on keypress

    if (key == '#') {
      // '#' = submit / check password
      checkPassword();
    }
    else if (key == '*') {
      // '*' = clear current entry
      resetInput();
      showIdleScreen();
    }
    else if (key == 'A') {
      // 'A' = enter password-change mode
      changePasswordFlow();
    }
    else {
      // Regular digit entry
      if (inputIndex < PASSWORD_LENGTH) {
        inputPassword[inputIndex] = key;
        inputIndex++;
        inputPassword[inputIndex] = '\0';
        updatePasswordDisplay();
      }
    }
  }
}

// ---------------- Core Functions ----------------

void checkPassword() {
  inputPassword[inputIndex] = '\0';

  if (strcmp(inputPassword, storedPassword) == 0) {
    accessGranted();
  } else {
    accessDenied();
  }
  resetInput();
}

void accessGranted() {
  failedAttempts = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Granted");
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  tone(BUZZER_PIN, 1500, 200);

  unlockDoor();
  delay(UNLOCK_DURATION);
  lockDoor();

  digitalWrite(GREEN_LED, LOW);
  showIdleScreen();
}

void accessDenied() {
  failedAttempts++;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Denied");
  digitalWrite(RED_LED, HIGH);
  tone(BUZZER_PIN, 400, 400);
  delay(1000);
  digitalWrite(RED_LED, LOW);

  if (failedAttempts >= MAX_ATTEMPTS) {
    lockoutSequence();
  } else {
    showIdleScreen();
  }
}

void lockoutSequence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Too many tries");
  lcd.setCursor(0, 1);
  lcd.print("Wait 15s...");

  for (int i = 0; i < 5; i++) {
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER_PIN, 300, 150);
    delay(150);
    digitalWrite(RED_LED, LOW);
    delay(150);
  }

  delay(LOCKOUT_DURATION - 1500 > 0 ? LOCKOUT_DURATION - 1500 : 0);
  failedAttempts = 0;
  showIdleScreen();
}

void unlockDoor() {
  digitalWrite(RELAY_PIN, HIGH); // Active-HIGH trigger; invert to LOW if using active-LOW relay module
}

void lockDoor() {
  digitalWrite(RELAY_PIN, LOW);  // default locked state
}

void resetInput() {
  inputIndex = 0;
  inputPassword[0] = '\0';
}

void showIdleScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
}

void updatePasswordDisplay() {
  lcd.setCursor(0, 1);
  for (byte i = 0; i < inputIndex; i++) {
    lcd.print('*'); // mask entered digits
  }
  lcd.print("    "); // clear trailing chars
}

// ---------------- Password Change Flow ----------------
void changePasswordFlow() {
  resetInput();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Old Password:");
  lcd.setCursor(0, 1);

  char oldPass[PASSWORD_LENGTH + 1];
  byte idx = 0;

  while (idx < PASSWORD_LENGTH) {
    char k = keypad.getKey();
    if (k && k >= '0' && k <= '9') {
      oldPass[idx++] = k;
      lcd.print('*');
      tone(BUZZER_PIN, 1000, 50);
    }
  }
  oldPass[idx] = '\0';

  if (strcmp(oldPass, storedPassword) != 0) {
    lcd.clear();
    lcd.print("Wrong Password");
    tone(BUZZER_PIN, 400, 400);
    delay(1500);
    showIdleScreen();
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("New Password:");
  lcd.setCursor(0, 1);

  char newPass[PASSWORD_LENGTH + 1];
  idx = 0;
  while (idx < PASSWORD_LENGTH) {
    char k = keypad.getKey();
    if (k && k >= '0' && k <= '9') {
      newPass[idx++] = k;
      lcd.print('*');
      tone(BUZZER_PIN, 1000, 50);
    }
  }
  newPass[idx] = '\0';

  strcpy(storedPassword, newPass);
  savePasswordToEEPROM();

  lcd.clear();
  lcd.print("Password Set!");
  tone(BUZZER_PIN, 1500, 200);
  delay(1500);
  showIdleScreen();
}

// ---------------- EEPROM Helpers ----------------
void savePasswordToEEPROM() {
  for (byte i = 0; i < PASSWORD_LENGTH; i++) {
    EEPROM.update(EEPROM_ADDR + i, storedPassword[i]);
  }
}

void loadPasswordFromEEPROM() {
  bool valid = true;
  char temp[PASSWORD_LENGTH + 1];

  for (byte i = 0; i < PASSWORD_LENGTH; i++) {
    char c = EEPROM.read(EEPROM_ADDR + i);
    if (c < '0' || c > '9') {
      valid = false;
      break;
    }
    temp[i] = c;
  }
  temp[PASSWORD_LENGTH] = '\0';

  if (valid) {
    strcpy(storedPassword, temp);
  } else {
    // First run: store the default password
    savePasswordToEEPROM();
  }
}
