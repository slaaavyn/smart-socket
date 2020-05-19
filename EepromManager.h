bool saveUid(String);

bool saveSsid(String);

bool savePassword(String);

bool saveBackendAddress(String);

bool saveBackendUsername(String);

bool saveBackendPassword(String);

bool saveApPassword(String apAddress);

void saveRelayState(uint8_t relayIndex, bool relayState);

void saveRelayDefaultState(uint8_t relayIndex, bool defaultState);

String getUid();

String getSsid();

String getPassword();

String getBackendAddress();

String getBackendUsername();

String getBackendPassword();

String getApPassword();

bool getRelayState(uint8_t relayIndex);

bool getRelayDefaultState(uint8_t relayIndex);

void clearAllEEPROM();