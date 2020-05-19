#define H23 82800000

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

class ConnectionManager {
    private:
        String serverAddress;
        String serverLogin;
        String serverPassword;

        HTTPClient http;

        String token;
        long tokenExpiredTime = 0;
        StaticJsonDocument<640> jsonDoc;

    public:
        ConnectionManager() {}

        bool connect(String backendAddress, String backendLogin, String backendPassword) {
            serverAddress = backendAddress;
            serverLogin = backendLogin;
            serverPassword = backendPassword;

            bool isAuthenticated = auth();

            return isAuthenticated;
        }

        bool auth() {
            http.begin("http://" + serverAddress + ":8080/auth");
            http.addHeader("Content-Type", "application/json");

            int httpCode = http.POST("{ \"username\": \"" + serverLogin + "\", \"password\": \"" + serverPassword + "\"}");

            deserializeJson(jsonDoc, http.getString());
            token = jsonDoc["token"].as<String>();

            http.end();

            if (httpCode != 200 || token.length() == 0) return false;
                
            tokenExpiredTime = millis() + H23;
            return true;
        }
        
        bool isTokenExpired() {
            return millis() > tokenExpiredTime;
        }

        String getToken() {
            return token;
        }
};