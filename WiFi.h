#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WiFiManager {
private:
    const char *ssid;
    const char *password;

public:
    WiFiManager(const char *networkSSID, const char *networkPassword)
        : ssid(networkSSID), password(networkPassword) {}

    bool connect() {
        WiFi.begin(ssid, password);
        Serial.print("Conectando a WiFi");
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
        }
        Serial.println("\nConectado a WiFi");
        return true;
    }

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }
};

#endif // WIFI_MANAGER_H
