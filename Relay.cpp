#include <Arduino.h>
#include "EepromManager.h"
#include <ArduinoJson.h>

class Relay {
    private:
        uint8_t pin;
        uint8_t index;
        bool state = getRelayDefaultState(index);
        bool defaultState = getRelayDefaultState(index);

    public:
        constexpr static uint8_t RELAY_COUNT = 5;
        
        Relay(int8_t pin, uint8_t index) : pin(pin), index(index) {}

        void init() {
            digitalWrite(pin, state);
        }

        bool getState() {
            return state;
        }

        bool getDefaultState() {
            return defaultState;
        }

        void setDefaultState(bool state) {
            defaultState = state;
            saveRelayDefaultState(index, defaultState);
        }

        void setState(bool incomingState) {
            state = incomingState;
            saveRelayState(index, state);
            digitalWrite(pin, state);
        }

        void switchRelay() {
            state = !state;
            saveRelayState(index, state);
            digitalWrite(pin, state);
        }

        StaticJsonDocument<128> toJsonObj() {
            StaticJsonDocument<128> doc;
            doc["type"] = "RELAY";
            doc["index"] = index;
            doc["defaultStatus"] = defaultState;
            doc["status"] = state;

            return doc;
        }
};