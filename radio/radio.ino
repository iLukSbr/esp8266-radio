#include <Wire.h>

// Pin assignments for the rotary encoder
const int PIN_DT = 12;  // Data (DT)
const int PIN_CLK = 13; // Clock (CLK)
const int PIN_SW = 14;  // Switch (SW)

// TEA5767 I2C Address
const int TEA5767_ADDR = 0x60;

// FM Frequency Range
const float FM_MIN = 88.0;
const float FM_MAX = 108.0;

// State Variables
float currentFreq = FM_MIN;
float lastFreq = 0.0;
bool lastAState;

void setup() {
    Wire.begin();
    Serial.begin(9600);

    pinMode(PIN_DT, INPUT);
    pinMode(PIN_CLK, INPUT);
    pinMode(PIN_SW, INPUT_PULLUP);

    lastAState = digitalRead(PIN_DT);
    
    Serial.println("TEA5767 FM Radio Initialized.");
    updateRadioFrequency();
}

void loop() {
    handleRotaryEncoder();
    handleSelection();
}

// Reads the rotary encoder and adjusts the frequency
void handleRotaryEncoder() {
    bool currentAState = digitalRead(PIN_DT);
    bool currentBState = digitalRead(PIN_CLK);

    if (currentAState != lastAState) {
        if (currentBState != currentAState) {
            currentFreq += 0.1;
            if (currentFreq > FM_MAX) {
                currentFreq = FM_MIN;
            }
        } else {
            currentFreq -= 0.1;
            if (currentFreq < FM_MIN) {
                currentFreq = FM_MAX;
            }
        }
        Serial.print("Set Frequency: ");
        Serial.println(currentFreq);
    }
    lastAState = currentAState;
}

// Handles the button press to confirm and set frequency
void handleSelection() {
    if (digitalRead(PIN_SW) == LOW && lastFreq != currentFreq) {
        updateRadioFrequency();
        Serial.print("Radio Playing at: ");
        Serial.println(currentFreq);
        lastFreq = currentFreq;
    }
}

// Converts frequency to TEA5767 format and sends it via I2C
void updateRadioFrequency() {
    int freqB = 4 * ((currentFreq * 1000000 + 225000) / 32768);

    byte data[5];
    data[0] = (freqB >> 8) & 0xFF; // High byte
    data[1] = freqB & 0xFF;        // Low byte
    data[2] = 0x10;                // PLL Mode
    data[3] = 0x10;                // Default setting
    data[4] = 0x40;                // Enable high-cut control

    Wire.beginTransmission(TEA5767_ADDR);
    Wire.write(data, 5);
    Wire.endTransmission();
}
