#include <Arduino.h>

const int LED_PIN = 2;
const int BUTTON_PIN = 3;

const long BLINK_INTERVAL = 500;
unsigned long previousMillis = 0;
unsigned long buttonPressStart = 0;
const long LONG_PRESS_DURATION = 500;

enum LedState {
    BLINKING,
    OFF,
    SOLID_ON
};

LedState currentLedState = BLINKING;
bool lastButtonState = HIGH;
bool buttonState = HIGH;
bool buttonPressed = false;
bool waitingForRelease = false;


void handleBlinking() {
    unsigned long currentMillis = millis();

    if (currentLedState == BLINKING) {
        if (currentMillis - previousMillis >= BLINK_INTERVAL) {
            previousMillis = currentMillis;

            int ledStatus = digitalRead(LED_PIN);
            digitalWrite(LED_PIN, !ledStatus);
            return;
        }
    }
    
    if (currentLedState == OFF) {
        digitalWrite(LED_PIN, LOW);
        return;
    }
    
    if (currentLedState == SOLID_ON) {
        digitalWrite(LED_PIN, HIGH);
        return;
    }
}

void handleButton() {
    buttonState = digitalRead(BUTTON_PIN);

    if (buttonState == LOW && lastButtonState == HIGH) {
        buttonPressed = true;
        buttonPressStart = millis();
    }

    bool long_press = buttonPressed && buttonState == LOW && millis() - buttonPressStart >= LONG_PRESS_DURATION;
    if (long_press) {
        if (currentLedState != SOLID_ON) {
            currentLedState = SOLID_ON;
        }
            
        waitingForRelease = true;
        buttonPressed = false;
    }

    if (buttonState == HIGH && lastButtonState == LOW) {
        unsigned long pressDuration = millis() - (buttonPressStart > 0 ? buttonPressStart : 0);

        if (!waitingForRelease && pressDuration < LONG_PRESS_DURATION) {
            if (currentLedState == BLINKING) {
                currentLedState = OFF;
            } else if (currentLedState == OFF) {
                currentLedState = BLINKING;
                previousMillis = millis();
            }
        }
        
        if (currentLedState == SOLID_ON) {
            currentLedState = OFF;
        }

        buttonPressed = false;
        waitingForRelease = false;
        buttonPressStart = 0;
    }

    lastButtonState = buttonState;
}

void setup() {
    Serial.begin(115200);
    
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP); 

    digitalWrite(LED_PIN, LOW);
}

void loop() {
    handleBlinking();
    handleButton();
}