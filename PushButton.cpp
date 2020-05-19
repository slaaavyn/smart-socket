#include <Arduino.h>

class PushButton {
public:
    PushButton(int8_t pin) : pin(pin) {}

    bool isPressed() {
        bool pressed = false;
        bool state = digitalRead(pin);
        int8_t stateChange = state - previousState;

        if (stateChange == falling) {
            if (millis() - previousBounceTime >= debounceTime) {
                pressed = true;
            }
        } else if (stateChange == rising) {
            previousBounceTime = millis();
        }

        previousState = state;

        return pressed;
    }

private:
    uint8_t pin;
    bool previousState = LOW;
    unsigned long previousBounceTime = 0;

    constexpr static unsigned long debounceTime = 25;
    constexpr static int8_t rising = HIGH - LOW;
    constexpr static int8_t falling = LOW - HIGH;
};