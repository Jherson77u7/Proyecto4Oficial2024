#ifndef PUB_SUB_CLIENT_MANAGER_H
#define PUB_SUB_CLIENT_MANAGER_H

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

class PubSubClientManager {
private:
    PubSubClient client;

public:
    PubSubClientManager(WiFiClientSecure &wifiClient) : client(wifiClient) {}

    void setServer(const char *host, int port) {
        client.setServer(host, port);
    }

    bool connect(const char *clientId) {
        if (client.connect(clientId)) {
            Serial.println("Conectado al broker MQTT");
            return true;
        } else {
            Serial.print("Error al conectar al broker MQTT: ");
            Serial.println(client.state());
            return false;
        }
    }

    bool publish(const char *topic, const char *message) {
        if (client.publish(topic  , message)) {
            Serial.println("Mensaje publicado exitosamente");
            return true;
        } else {
            Serial.println("Error al publicar el mensaje");
            return false;
        }
    }

    void subscribe(const char *topic) {
        client.subscribe(topic);
    }

    void loop() {
        client.loop();
    }

    bool isConnected() {
        return client.connected();
    }
};

#endif // PUB_SUB_CLIENT_MANAGER_H
