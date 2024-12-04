#ifndef WIFI_CLIENT_SECURE_MANAGER_H
#define WIFI_CLIENT_SECURE_MANAGER_H

#include <WiFiClientSecure.h>

class WiFiClientSecureManager {
private:
    WiFiClientSecure client;

public:
    WiFiClientSecureManager() {}

    void setCertificates(const char *cert, const char *key, const char *rootCA) {
        client.setCertificate(cert);
        client.setPrivateKey(key);
        client.setCACert(rootCA);
    }

    WiFiClientSecure &getClient() {
        return client;
    }
};

#endif // WIFI_CLIENT_SECURE_MANAGER_H
