#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

/* =====================================================================
   HARDWARE: Arduino MEGA + I2C LCD + 4x ULN2003 + Coin Selector + Keypad
   POWER: 12V -> (1) buck -> 5V rail  |  (2) coin selector (12V)
          5V rail -> Mega(+LCD/Keypad/Detector) + 4x ULN2003
   SIGNAL: Coin PULSE open-collector -> D3 with 10k pull-up to 5V
   ===================================================================== */

// ============================== LCD ===================================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ========================= Coin Acceptor ===============================
const uint8_t COIN_PIN        = 3;      // Pin 3 on the MEGA
const int      TARGET_CENTS   = 100;    // 100 sen = 1 MYR (1 credit)
const uint8_t  P20            = 7;      // 20 sen coin pulse count
const uint8_t  P50            = 10;     // 50 sen coin pulse count
const uint8_t  TOLERANCE      = 1;      // +-1 pulse tolerance
const unsigned long DEBOUNCE_US   = 30000UL; // 30 ms ISR debounce
const unsigned long GROUP_TIMEOUT = 250UL;   // gap to “finalize” coin group

volatile uint8_t      pulseCount   = 0;
volatile unsigned long lastPulseUs = 0;
volatile unsigned long lastPulseMs = 0;

int cents   = 0;   // sen (MYR cents)
int credits = 0;   // whole MYR (1 credit = 100 sen)

// ============================== Keypad =================================
const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  { '1','2','3','A' }, 
  { '4','5','6','B' },
  { '7','8','9','C' },
  { '*','0','#','D' }
};
byte rowPins[ROWS] = { 4, 5, 6, 7 };
byte colPins[COLS] = { 8, 9, 10, 11 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ========================= Products / Steppers =========================
// Keypad Keys used to vend each slot
const char PRODUCT_KEYS[4]   = { '4', '5', '7', '8' };

// Names & prices (sen)
const char* PRODUCT_NAMES[4] = { "Product 1", "Product 2", "Product 3", "Product 4" };
int productPrice[4]          = { 20, 50, 70, 100 };  // 0.20 / 0.50 / 0.70 / 1.00 MYR

// ULN2003 
const uint8_t M_PINS[4][4] = {
  {23, 25, 27, 29},  // Motor 1
  {31, 33, 35, 37},  // Motor 2
  {39, 41, 43, 45},  // Motor 3
  {47, 49, 51, 53}   // Motor 4
};

// Per-motor controls
int8_t  MOTOR_DIR[4]              = { +1, +1, -1, +1 };   // +1 forward, -1 reverse
unsigned VEND_STEPS_PER_MOTOR[4]  = { 4096, 4096, 4096, 4096 };

// 28BYJ-48 half-step sequence (8 phases)
const uint8_t HALFSTEP[8][4] = {
  {1,0,0,0},
  {1,1,0,0},
  {0,1,0,0},
  {0,1,1,0},
  {0,0,1,0},
  {0,0,1,1},
  {0,0,0,1},
  {1,0,0,1}
};

// Global tuning knobs
const unsigned STEP_DELAY_MS  = 2;     // lower = faster (but less torque)

// ========================= Display (idle marquee) ======================
unsigned long lastRotateMillis = 0;
const unsigned long ROTATE_INTERVAL = 800;       // How fast the text scrolls
const int ROTATE_STEPS = 2;                      // How many chars per scroll tick
int rotatePosition = 0;
const char ROTATE_MSG[] = " INSERT YOUR COIN  "; // scrolling text

// =========================== STEPPERS ===========================
void motorHalfStep(uint8_t m, uint8_t phase) {
  for (uint8_t i=0; i<4; i++) {
    digitalWrite(M_PINS[m][i], HALFSTEP[phase][i] ? HIGH : LOW);
  }
}

void motorIdle(uint8_t m) {
  for (uint8_t i=0; i<4; i++) digitalWrite(M_PINS[m][i], LOW);
}

// Core motor move: 'steps' half-steps, 'dir' = +1 forward or -1 reverse
void motorRotate(uint8_t m, long steps, int dir) {
  int phase = 0;
  long rem = (steps < 0) ? -steps : steps;
  int stepDir = (dir >= 0) ? +1 : -1;
  while (rem--) {
    phase += stepDir;
    if (phase < 0) phase = 7;
    if (phase > 7) phase = 0;
    motorHalfStep(m, phase);
    delay(STEP_DELAY_MS);
  }
  motorIdle(m);  // De-energize coils after movement
}

// ============================= FUNDS ===========================
long totalFundsCents() { return (long)credits * TARGET_CENTS + cents; }

void setFundsFromTotal(long total) {
  if (total < 0) total = 0;
  credits = total / TARGET_CENTS;
  cents   = total % TARGET_CENTS;
}

void addFundsCents(long add)            { setFundsFromTotal(totalFundsCents() + add); }
void deductFundsCents(long amt) {
  long total = totalFundsCents();
  if (amt > total) amt = total;
  setFundsFromTotal(total - amt);
}

// ============================== LCD UI =================================
String makeSpaces(uint8_t n) {
  String s;
  s.reserve(n);
  for (uint8_t i = 0; i < n; i++) s += ' ';
  return s;
}

// Fit to 16 columns
String fit16(const String& s, bool center=false) {
  String t = s;
  if (t.length() > 16) t = t.substring(0, 16);

  if (!center) {
    return t + makeSpaces(16 - t.length());  // left-justify, pad right
  }

  uint8_t spaces = 16 - t.length();
  uint8_t left   = spaces / 2;
  uint8_t right  = spaces - left;
  return makeSpaces(left) + t + makeSpaces(right);
}

String fmtMoneyCents(long centsTotal) {
  if (centsTotal < 0) centsTotal = 0;
  long whole = centsTotal / 100;
  long frac  = centsTotal % 100;
  if (frac < 0) frac = -frac;
  char buf[10];
  snprintf(buf, sizeof(buf), "MYR%ld.%02ld", whole, frac);
  return String(buf);
}

String fmtPriceOnly(int cents) {
  char buf[10];
  int whole = cents / 100;
  int frac  = cents % 100;
  snprintf(buf, sizeof(buf), "%d.%02d", whole, frac);
  return String(buf);
}

void showIdleScreen() {
  lcd.clear();
  // Top line: balance
  lcd.setCursor(0,0);
  long total = totalFundsCents();
  char balanceLine[17];
  snprintf(balanceLine, sizeof(balanceLine), "Balance: %s", fmtMoneyCents(total).c_str());
  lcd.print(fit16(balanceLine));    // fills entire line

  // Bottom line will be filled by the rotating text
  rotatePosition = 0;
}

void rotateInsertCoin() {
  lcd.setCursor(0,1);
  String displayMsg = String(ROTATE_MSG);
  displayMsg += ROTATE_MSG;  // double up for smooth wrap
  String segment = displayMsg.substring(rotatePosition, rotatePosition + 16);
  lcd.print(segment);        // exactly 16 chars
  rotatePosition = (rotatePosition + ROTATE_STEPS) % strlen(ROTATE_MSG);
}

void lcdShowCoinAdded(int amountCents) {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print(fit16("Coin accepted", true));
  lcd.setCursor(0,1);
  String s = (amountCents >= 100) ? ("+" + fmtMoneyCents(amountCents))
                                  : ("+" + String(amountCents) + " sen");
  lcd.print(fit16(s, true));
  delay(800);
  showIdleScreen();
}

void lcdShowUnknownCoin(uint8_t pulses) {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print(fit16("Unknown coin", true));
  //lcd.setCursor(0,1);
  //char buf[17]; snprintf(buf, sizeof(buf), "pulses:%u", pulses);
  //lcd.print(fit16(String(buf), true));
  delay(1500);
  showIdleScreen();
}

void lcdShowInsufficient(int /*priceCents*/) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(fit16("Insufficient", true));
  lcd.setCursor(0, 1); lcd.print(fit16("Balance", true));
  delay(1500);
  showIdleScreen();
}

void lcdShowVendStart(uint8_t m, int priceCents) {
  lcd.clear();
  lcd.setCursor(0,0);
  char top[32]; snprintf(top, sizeof(top), "Vending %s", PRODUCT_NAMES[m]);
  lcd.print(fit16(String(top), true));
  lcd.setCursor(0,1);
  String paid = "Paid: " + fmtMoneyCents(priceCents);
  lcd.print(fit16(paid, true));
}

void lcdShowVendDone(uint8_t m) {
  lcd.clear();
  String topMsg = String(PRODUCT_NAMES[m]) + " dispensed";


  String scrollText = topMsg + "   "; 
  int len = scrollText.length();

  unsigned long start = millis();
  while (millis() - start < 5000) {  // scroll for ~5 seconds
    for (int pos = 0; pos <= len - 1; pos++) {
      lcd.setCursor(0, 0);
      String segment = scrollText.substring(pos) + scrollText.substring(0, pos);
      if (segment.length() > 16) segment = segment.substring(0, 16);
      else segment += String(' ', 16 - segment.length()); // pad

      lcd.print(segment);  // top line scroll
      lcd.setCursor(0, 1);
      lcd.print(fit16("Thank you!", true)); // bottom line "fixed"

      delay(800); // scrolling speed
      if (millis() - start >= 3000) break;
    }
  }

  showIdleScreen();
}

// ============================== VENDING ================================
bool tryVend(uint8_t motorIdx) {
  int price = productPrice[motorIdx];
  if (totalFundsCents() < price) {         // Not enough funds?
    lcdShowInsufficient(price);
    return false;
  }

  deductFundsCents(price);                  // Charge before movement
  lcdShowVendStart(motorIdx, price);

  // Per-motor control
  unsigned steps = VEND_STEPS_PER_MOTOR[motorIdx];
  int dir        = MOTOR_DIR[motorIdx];

  motorRotate(motorIdx, steps, dir);
  lcdShowVendDone(motorIdx);
  return true;
}

// ============================ SETUP / LOOP =============================
void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  showIdleScreen();  // Show balance + “INSERT YOUR COIN”

  pinMode(COIN_PIN, INPUT_PULLUP);  // Idle HIGH; pulses to GND
  attachInterrupt(digitalPinToInterrupt(COIN_PIN), coinISR, FALLING);

  // Prepare stepper pins
  for (uint8_t m=0; m<4; m++) {
    for (uint8_t i=0; i<4; i++) {
      pinMode(M_PINS[m][i], OUTPUT);
      digitalWrite(M_PINS[m][i], LOW);
    }
  }
  Serial.println("System ready");
}

void loop() {
  unsigned long now = millis();

  // Finalize coin groups after a quiet gap
  if (pulseCount > 0 && now - lastPulseMs >= GROUP_TIMEOUT) {
    noInterrupts();
      uint8_t pulses = pulseCount;
      pulseCount = 0;
    interrupts();

    if (abs(int(pulses) - P50) <= TOLERANCE) {
      addFundsCents(50);
      Serial.println("→ Detected 50 sen");
      lcdShowCoinAdded(50);
    } else if (abs(int(pulses) - P20) <= TOLERANCE) {
      addFundsCents(20);
      Serial.println("→ Detected 20 sen");
      lcdShowCoinAdded(20);
    } else {
      Serial.print("→ Unknown coin: "); Serial.print(pulses); Serial.println(" pulses");
      lcdShowUnknownCoin(pulses);
    }
  }

  // ---------------------------- Keypad ----------------------------
  char key = keypad.getKey();
  if (key) {
    int motorIdx = -1;
    for (int i=0; i<4; i++) if (key == PRODUCT_KEYS[i]) { motorIdx = i; break; }

    if (motorIdx >= 0) {
      tryVend(motorIdx);                   // Vend the selected product
    } else {
      if (key == 'D') {                    // Debug print
        Serial.print("Funds = ");
        Serial.print(credits); Serial.print(" MYR, ");
        Serial.print(cents);   Serial.println(" sen");
      }
      showIdleScreen();
    }
  }

  // Rotate “INSERT YOUR COIN”
  if (millis() - lastRotateMillis >= ROTATE_INTERVAL) {
    rotateInsertCoin();
    lastRotateMillis = millis();
  }
}

// =============================== ISR ===================================
void coinISR() {
  unsigned long nowUs = micros();
  if (nowUs - lastPulseUs >= DEBOUNCE_US) { // Debounce pulses in microseconds
    pulseCount++;
    lastPulseUs = nowUs;
    lastPulseMs = millis();                 // mark last pulse time (for grouping)
  }
}
