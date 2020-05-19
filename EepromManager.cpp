#include <EEPROM.h>
#include <WString.h>
#include "Relay.cpp"

const short int EEPROM_START_BIT = 0;
const short int EEPROM_END_BIT = 512;

const short int SSID_START_BIT = 32;
const short int PASSWORD_START_BIT = 64;
const short int BACKEND_ADRESS_START_BIT = 128;
const short int BACKEND_USERNAME_START_BIT = 160;
const short int BACKEND_PASSWORD_START_BIT = 192;
const short int AP_PASSWORD_START_BIT = 256;
const short int RELAYS_STATE_START_BIT = 480;

bool saveUid(String uid) {
    if(uid.length() <= 0) return false;

    for (int i = 0; i < uid.length(); ++i) {
        EEPROM.write(i + EEPROM_START_BIT, uid[i]);
    }

    EEPROM.commit();

    return true;
}

bool saveSsid(String ssid) {
    if(ssid.length() <= 0) return false;

    for (int i = 0; i < ssid.length(); ++i) {
        EEPROM.write(i + SSID_START_BIT, ssid[i]);
    }

    EEPROM.commit();

    return true;
}

bool savePassword(String password) {
    if(password.length() <= 0) return false;

    for (int i = 0; i < password.length(); ++i) {
        EEPROM.write(i + PASSWORD_START_BIT, password[i]);
    }
    
    EEPROM.commit();

    return true;
}

bool saveBackendAddress(String backendAddress) {
    if(backendAddress.length() <= 0) return false;

    for (int i = 0; i < backendAddress.length(); ++i) {
        EEPROM.write(i + BACKEND_ADRESS_START_BIT, backendAddress[i]);
    }
    
    EEPROM.commit();

    return true;
}

bool saveBackendUsername(String backendUsername) {
    if(backendUsername.length() <= 0) return false;

    for (int i = 0; i < backendUsername.length(); ++i) {
        EEPROM.write(i + BACKEND_USERNAME_START_BIT, backendUsername[i]);
    }
    
    EEPROM.commit();

    return true;
}

bool saveBackendPassword(String backendPassword) {
    if(backendPassword.length() <= 0) return false;

    for (int i = 0; i < backendPassword.length(); ++i) {
        EEPROM.write(i + BACKEND_PASSWORD_START_BIT, backendPassword[i]);
    }

    EEPROM.commit();

    return true;
}

bool saveApPassword(String apAddress) {
    if(apAddress.length() <= 0) return false;

    for (int i = 0; i < apAddress.length(); ++i) {
        EEPROM.write(i + AP_PASSWORD_START_BIT, apAddress[i]);
    }
    
    EEPROM.commit();

    return true;
}

void saveRelayState(uint8_t relayIndex, bool relayState) {
    EEPROM.write(RELAYS_STATE_START_BIT + relayIndex, relayState);
    EEPROM.commit();
}

void saveRelayDefaultState(uint8_t relayIndex, bool defaultState) {
    EEPROM.write(RELAYS_STATE_START_BIT + Relay::RELAY_COUNT + relayIndex, defaultState);
    EEPROM.commit();
}

String getUid() {
    EEPROM.begin(512);
    String uid = "";
    for (int i = EEPROM_START_BIT; i < SSID_START_BIT; ++i){
        if(EEPROM.read(i) != 0) {
            uid += char(EEPROM.read(i));
        } else {
            return uid;
        }   
    }

    return uid;
}

String getSsid() {
    EEPROM.begin(512);
    String ssid = "";
    for (int i = SSID_START_BIT; i < PASSWORD_START_BIT; ++i){
        if(EEPROM.read(i) != 0) {
            ssid += char(EEPROM.read(i));
        } else {
            return ssid;
        }
    }

    return ssid;
}

String getPassword() {
    EEPROM.begin(512);
    String password = "";
    for (int i = PASSWORD_START_BIT; i < BACKEND_ADRESS_START_BIT; ++i){
        if(EEPROM.read(i) != 0) {
            password += char(EEPROM.read(i));
        } else {
            return password;
        }
    }

    return password;
}

String getBackendAddress() {
    EEPROM.begin(512);
    String backendAddress = "";
    for (int i = BACKEND_ADRESS_START_BIT; i < BACKEND_USERNAME_START_BIT; ++i) {
        if(EEPROM.read(i) != 0) {
            backendAddress += char(EEPROM.read(i));
        } else {
            return backendAddress;
        }
    }

    return backendAddress;
}

String getBackendUsername() {
    EEPROM.begin(512);
    String backendUsername = "";
    for (int i = BACKEND_USERNAME_START_BIT; i < BACKEND_PASSWORD_START_BIT; ++i) {
        if(EEPROM.read(i) != 0) {
            backendUsername += char(EEPROM.read(i));
        } else {
            return backendUsername;
        }
    }

    return backendUsername;
}

String getBackendPassword() {
    EEPROM.begin(512);
    String backendUsername = "";
    for (int i = BACKEND_PASSWORD_START_BIT; i < AP_PASSWORD_START_BIT; ++i) {
        if(EEPROM.read(i) != 0) {
            backendUsername += char(EEPROM.read(i));
        } else {
            return backendUsername;
        }
    }

    return backendUsername;
}

String getApPassword() {
    EEPROM.begin(512);
    String apPassword = "";
    for (int i = AP_PASSWORD_START_BIT; i < 288; ++i) {
        if(EEPROM.read(i) != 0) {
            apPassword += char(EEPROM.read(i));
        } else {
            return apPassword;
        }
    }

    return apPassword;
}

bool getRelayState(uint8_t relayIndex) {
    EEPROM.begin(512);
    return (bool) EEPROM.read(RELAYS_STATE_START_BIT + relayIndex);
}

bool getRelayDefaultState(uint8_t relayIndex) {
    EEPROM.begin(512);
    return (bool) EEPROM.read(RELAYS_STATE_START_BIT + Relay::RELAY_COUNT + relayIndex);
}

void clearAllEEPROM() {
    EEPROM.begin(512);
    for (int i = SSID_START_BIT; i < 512; ++i) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}
