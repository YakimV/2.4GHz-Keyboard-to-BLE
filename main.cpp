#include <Arduino.h>
#include <BleKeyboard.h>

BleKeyboard bleKeyboard("CustomKB", "keyboard", 100);

// --- MATRIX PINS FOR ESP32-C3 SUPERMINI ---
const int ROW_DATA = 0;   // DS for 595
const int ROW_CLOCK = 1;  // SH_CP for 595
const int ROW_LATCH = 2;  // ST_CP for 595
const int ROW_17_DIRECT = 6; // 17th row direct

const int COL_LOAD = 3;   // PL for 165
const int COL_CLOCK = 4;  // CP for 165
const int COL_DATA = 5;   // Q7 for 165

// 17 rows × 8 columns
char keyMap[17][8] = {
  {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'},
  {'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'},
  {'q', 'r', 's', 't', 'u', 'v', 'w', 'x'},
  {'y', 'z', '1', '2', '3', '4', '5', '6'},
  {'7', '8', '9', '0', '-', '=', '[', ']'},
  {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'},
  {'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'},
  {'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'},
  {'Y', 'Z', '!', '@', '#', '$', '%', '^'},
  {'&', '*', '(', ')', '_', '+', '{', '}'},
  {';', ':', '\'', '"', ',', '<', '.', '>'},
  {'/', '?', '\\', '|', '`', '~', 'a', 'b'},
  {'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'},
  {'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r'},
  {'s', 't', 'u', 'v', 'w', 'x', 'y', 'z'},
  {'1', '2', '3', '4', '5', '6', '7', '8'},
  {'9', '0', ' ', ' ', ' ', ' ', ' ', ' '}
};

bool prevState[17][8] = {false};

void setup() {
  Serial.begin(115200);

  pinMode(ROW_DATA, OUTPUT);
  pinMode(ROW_CLOCK, OUTPUT);
  pinMode(ROW_LATCH, OUTPUT);

  pinMode(ROW_17_DIRECT, OUTPUT);
  digitalWrite(ROW_17_DIRECT, HIGH);

  pinMode(COL_LOAD, OUTPUT);
  pinMode(COL_CLOCK, OUTPUT);
  pinMode(COL_DATA, INPUT);
  digitalWrite(COL_LOAD, HIGH);

  Serial.println("Starting Bluetooth...");
  bleKeyboard.begin();
}

void activateRow(int row) {
  digitalWrite(ROW_17_DIRECT, HIGH);

  if (row < 16) {
    uint16_t rowMask = ~(1 << row);

    digitalWrite(ROW_LATCH, LOW);

    shiftOut(ROW_DATA, ROW_CLOCK, MSBFIRST, rowMask >> 8);
    shiftOut(ROW_DATA, ROW_CLOCK, MSBFIRST, rowMask & 0xFF);

    digitalWrite(ROW_LATCH, HIGH);

  } else if (row == 16) {
    digitalWrite(ROW_LATCH, LOW);

    shiftOut(ROW_DATA, ROW_CLOCK, MSBFIRST, 0xFF);
    shiftOut(ROW_DATA, ROW_CLOCK, MSBFIRST, 0xFF);

    digitalWrite(ROW_LATCH, HIGH);

    digitalWrite(ROW_17_DIRECT, LOW);
  }
}

byte readColumns() {
  digitalWrite(COL_LOAD, LOW);
  delayMicroseconds(5);
  digitalWrite(COL_LOAD, HIGH);

  return shiftIn(COL_DATA, COL_CLOCK, MSBFIRST);
}

void loop() {
  if (bleKeyboard.isConnected()) {

    for (int r = 0; r < 17; r++) {
      activateRow(r);
      delayMicroseconds(20);

      byte cols = readColumns();
      byte pressed = ~cols;

      for (int c = 0; c < 8; c++) {
        bool isCurrentlyPressed = bitRead(pressed, c);

        if (isCurrentlyPressed != prevState[r][c]) {

          char keyToSend = keyMap[r][c];

          if (isCurrentlyPressed) {
            Serial.printf(
              "Pressed [%d][%d] -> %c\n",
              r, c, keyToSend
            );
            bleKeyboard.press(keyToSend);
          } else {
            Serial.printf(
              "Released [%d][%d] -> %c\n",
              r, c, keyToSend
            );
            bleKeyboard.release(keyToSend);
          }

          prevState[r][c] = isCurrentlyPressed;
        }
      }
    }
  }

  delay(10);
}

