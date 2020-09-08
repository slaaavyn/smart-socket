#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

#include "EepromManager.h"
#include "PushButton.cpp"
#include "Relay.cpp"
#include "ConnectionManager.cpp"

#define LED_BUILTIN 2

#define RELAY_0 9
#define RELAY_1 13
#define RELAY_2 12
#define RELAY_3 14
#define RELAY_4 16

#define SWITHC_0 10
#define SWITHC_1 15
#define SWITHC_2 3
#define SWITHC_3 4
#define SWITHC_4 5

#define BUTTONS_COUNT 5

PushButton buttons[BUTTONS_COUNT] = {
    PushButton {SWITHC_0},
    PushButton {SWITHC_1},
    PushButton {SWITHC_2},
    PushButton {SWITHC_3},
    PushButton {SWITHC_4}
};

Relay relays[Relay::RELAY_COUNT] = {
    Relay{RELAY_0, 0},
    Relay{RELAY_1, 1},
    Relay{RELAY_2, 2},
    Relay{RELAY_3, 3}, 
    Relay{RELAY_4, 4}
};

String uid = "";
String ssid = "";
String password = "";
String backendAddress = "";
String backendLogin = "";
String backendPassword = "";
String apPassword = "";

String wsDevicePath = "/ws/device";

long lastMsg = 0;
long lastBlink = 0;
bool isWebSocketConnected = false;

WiFiClient espClient;
ESP8266WebServer server(80);
WebSocketsClient webSocket;

ConnectionManager connectionManager;

void setup(){
    Serial.begin(115200);
    delay(10);

    pinMode(LED_BUILTIN, OUTPUT);

    // init relay pins
    pinMode(RELAY_0, OUTPUT);
    pinMode(RELAY_1, OUTPUT);
    pinMode(RELAY_2, OUTPUT);
    pinMode(RELAY_3, OUTPUT);
    pinMode(RELAY_4, OUTPUT);

    // init buttons
    pinMode(SWITHC_0, INPUT);
    pinMode(SWITHC_1, INPUT);
    pinMode(SWITHC_2, INPUT);
    pinMode(SWITHC_3, INPUT);
    pinMode(SWITHC_4, INPUT);

    for (size_t i = 0; i < 5; i++) {
        relays[i].init();
    }

    Serial.println("Startup");

    Serial.println("Reading EEPROM uid");
    uid = getUid();
    if(uid.length() == 0 || uid[0] == 0) {
        uid = String(random(0xffffff), HEX);
        saveUid(uid);
    }
    Serial.println("UID: " + uid + "\n");
    delay(10);

    Serial.println("Reading EEPROM ssid");
    ssid = getSsid();
    Serial.println("SSID: " + ssid + "\n");
    delay(10);

    Serial.println("Reading EEPROM pass");
    password = getPassword();
    Serial.println("PASS: " + password + "\n");
    delay(10);

    Serial.println("Reading EEPROM backendAddress");
    backendAddress = getBackendAddress();
    Serial.println("backendAdress: " + backendAddress + "\n");
    delay(10);

    Serial.println("Reading EEPROM backendUsername");
    backendLogin = getBackendUsername();
    Serial.println("backendUsername: " + backendLogin + "\n");
    delay(10);

    Serial.println("Reading EEPROM backendPassword");
    backendPassword = getBackendPassword();
    Serial.println("backendPassword: " + backendPassword + "\n");
    delay(10);

    Serial.println("Reading EEPROM apPassword");
    apPassword = getApPassword();
    Serial.println("apPasswprd: " + apPassword + "\n");
    delay(10);

    setupSTA(ssid, password);

    if (testWifi() && connectionManager.connect(backendAddress, backendLogin, backendPassword)){
        Serial.println("Succesfully Connected!!!");
        connectWebSocket();
        return;
    } else {
        Serial.println("Turning the HotSpot On");
        setupAP();
        launchWeb();
    }

    Serial.println("Waiting for http client...");
    while((WiFi.status() != WL_CONNECTED)){
        delay(100);
        server.handleClient();
        buttonRelaySwitcher();
    }
}

void loop() {
    long now = millis();

    if (WiFi.status() == WL_CONNECTED) {
        if (now - lastBlink > 2000) {
            lastBlink = now;
            digitalWrite(LED_BUILTIN, LOW);
        } else if(now - lastBlink > 1000) {
            digitalWrite(LED_BUILTIN, HIGH);
        }

        if (connectionManager.isTokenExpired() && isWebSocketConnected) {
            connectionManager.auth();
        }
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }

    webSocket.loop();

    buttonRelaySwitcher();
}

bool testWifi() {
    Serial.println("Waiting for Wifi to connect");
    int c = 0;
    while ( c < 20 ) {
        if(WiFi.status() == WL_CONNECTED ) {
            return true;
        }
        delay(500);
        Serial.print("*");
        c++;
    }
    Serial.println("WIFI Connect timed out");
    return false;
}

void connectWebSocket() {
    webSocket.begin(backendAddress, 8080, wsDevicePath);
    webSocket.setExtraHeaders("message: dont work without this header: not remove");
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void setupAP() {
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_AP);

    if(apPassword.length() == 0 || apPassword[0] == 0) {
        WiFi.softAP("slavik-socket " + uid);
    } else {
        WiFi.softAP("slavik-socket " + uid, apPassword);
    }
}

void setupSTA(String ssid, String password) {
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
}

void launchWeb() {
    Serial.println("");
    if(WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected");
    }
    Serial.println("Local IP: " + WiFi.localIP().toString());
    Serial.println("SoftAP IP: " + WiFi.softAPIP().toString());
    createWebServer();
    server.begin();
    Serial.println("Server started");
}

void createWebServer() {
    server.on("/", []() {
        String content = "<html lang=\"en\">";

        // META
        content += "<meta charset=\"UTF-8\">";
        content += "<meta name=\"viewport\" content=\"width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0\">";
        content += "<meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">";
        
        content += "<head> <title>Slavic Socket</title>";
        
        // CSS
        content += "<style>";
        content += ".root { margin-left: auto; margin-right: auto; display: flex; flex-direction: column; align-items: center; }";
        content += ".textSpan { display: flex; justify-content: space-between; width: 20em; margin-top: 0.5em; margin-bottom: 0.5em; };";
        content += "input { width: 9em; }";
        content += "@media screen and (max-width: 350px) { .textSpan{ width: 17em; } input { width: 7em; } }";
        content += "</style>";

        content += "</head>";

        // BODY
        content += "<body> <div class=\"root\">";
        content += "<p>Slavik Socket " + uid + "</p>";
        content += "<form method='get' action='setting'>";
        content += "<span class=\"textSpan\"><span>* SSID: </span><input name='ssid' value='" + ssid + "' length=32></span>";
        content += "<span class=\"textSpan\"><span>* WIFI PASSWORD: </span><input name='pass' value='" + password + "' length=64></span>";
        content += "<span class=\"textSpan\"><span>* BACKEND SERVER: </span><input name='backendAddress' value='" + backendAddress + "' length=32></span>";
        content += "<span class=\"textSpan\"><span>* BACKEND LOGIN: </span><input name='backendUsername' value='" + backendLogin + "' length=32></span>";
        content += "<span class=\"textSpan\"><span>* BACKEND PASSWORD: </span><input name='backendPassword' value='" + backendPassword + "' length=64></span>";
        content += "<span class=\"textSpan\"><span>AP PASSWORD: </span><input name='apPassword' value='" + apPassword + "' length=64></span> <br>";
        content += "<input type='submit'>";
        content += "</form>";
        content += "<div class=\"root\"> </body>";

        content += "</html>";

        server.send(200, "text/html", content);
    });

    server.on("/setting", []() {
        String rssid = server.arg("ssid");
        String rpass = server.arg("pass");
        String rBackendAddress = server.arg("backendAddress");
        String rBackendUsername = server.arg("backendUsername");
        String rBackendPassword = server.arg("backendPassword");
        String rApPassword = server.arg("apPassword");
        
        if (rssid.length() > 0 && rpass.length() > 0 && rBackendAddress.length() > 0 
                        && rBackendUsername.length() > 0 && rBackendPassword.length() > 0) {
            clearAllEEPROM();
            Serial.println("writing eeprom ssid");
            saveSsid(rssid);
            
            Serial.println("writing eeprom pass");
            savePassword(rpass);
            
            Serial.println("writing eeprom backendAddress");
            saveBackendAddress(rBackendAddress);
            
            Serial.println("writing eeprom backendLogin");
            saveBackendUsername(rBackendUsername);
            
            Serial.println("writing eeprom backendPassword");
            saveBackendPassword(rBackendPassword);

            if(rApPassword.length() > 0) {
                Serial.println("writing eeprom ApPassword");
                saveBackendPassword(rApPassword);
            }
            
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "application/json", "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}");
            ESP.reset();
        } else {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(404, "application/json", "{\"Error\":\"404 not found\"}");
        }
    });
}


// handling physical keystrokes 
void buttonRelaySwitcher() {

    // switch relays
    for (size_t i = 0; i < BUTTONS_COUNT; i++) {
        if(buttons[i].isPressed()) {
            relays[i].switchRelay();
            sendDeviceJson();
        }
    }

    //clear EEPROM
    if(digitalRead(SWITHC_0) == HIGH && digitalRead(SWITHC_2) == HIGH && digitalRead(SWITHC_4) == HIGH) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);

        for (size_t i = 0; i < 5; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(250);
            digitalWrite(LED_BUILTIN, LOW);
            delay(250);
        }
        
        clearAllEEPROM();
        ESP.reset();
    }
}


// handle incoming commands from websocket
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            if (isWebSocketConnected) {
                for (size_t i = 0; i < Relay::RELAY_COUNT; i++) {
                    relays[i].setState(relays[i].getDefaultState());
                }
            }

            isWebSocketConnected = false;
            break;

        case WStype_CONNECTED: 
            isWebSocketConnected = true;
            break;

        case WStype_TEXT:
            Serial.println((char*) payload);

            StaticJsonDocument<512> jsonCommand;
            deserializeJson(jsonCommand, (char*) payload);

            String commandType = jsonCommand["type"].as<String>();
            String commandUid = jsonCommand["uid"].as<String>();
            
            if (commandType.equals("ACK")) {
                sendDeviceJson();
            }

            if (commandUid.equals(uid) && commandType.equals("RELAY")) {
                uint8_t index = jsonCommand["index"].as<uint8_t>();
                bool state = jsonCommand["status"].as<bool>();
                bool defaultStatus = jsonCommand["defaultStatus"].as<bool>();

                relays[index].setState(state);
                relays[index].setDefaultState(defaultStatus);
                sendDeviceJson();
            }

            break;
    }
}

// serialize json string and send to server
void sendDeviceJson() {
    StaticJsonDocument<640> jsonComonentsArray;
    JsonArray array = jsonComonentsArray.to<JsonArray>();
    for (size_t i = 0; i < Relay::RELAY_COUNT; i++) {
        array.add(relays[i].toJsonObj());
    }

    StaticJsonDocument<1024> deviceJson;
    deviceJson["uid"] = uid;
    deviceJson["token"] = connectionManager.getToken();
    deviceJson["components"] = jsonComonentsArray;

    String serializedJson = "";
    serializeJson(deviceJson, serializedJson);

    webSocket.sendTXT(serializedJson);
}
