#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "oledySensoryBuzzerManager.h"
#include "SonidoManager.h"
#include <SD.h>

// ***** Configuración para Wi-Fi y AWS IoT *****
const char *ssid = "Prueba";
const char *password = "12345678";

const char *host = "a25ex5q24thysh-ats.iot.us-east-1.amazonaws.com";
const int port = 8883;

WiFiClientSecure net;
PubSubClient client(net);

const char *cert = R"(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUSIX+wu2nuSCgoeYoU/2YogPxilwwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTEwNjIyNTA0
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMAJW6S40jPEWnYFMSsQ
O9EcgHLHeq02kSDdIeeqGKgw5/Cez1cDL9xYagg2nEjoNbbsBvUMMs89lg3alkyh
RHO1XgFn4cJfn6CU8lG7nnXpWEPQokeFG47Zqb2YVRYNYB7j5ZM2rBlAwM3GigFb
+TAPM1XLcLgJasZVQdvYQS05Ul//gbQ3rghzQ81AsLiL1s1KlSwmLwfOjYYX72tW
eteMeW6IWPfgvwa7gy4KsE8sbI0fcHxRKx4kbi/1L1ROB1uAwpxBVMjq+vYWZmNL
VBd21H1DopSFNiqCOiS9EDBzXf/DavX86eBspCkJ0xXDpi0AXxUz20+KgT3vFrN7
S8UCAwEAAaNgMF4wHwYDVR0jBBgwFoAUaUpkMOomc3Kd3A5MBTrHyusf4x0wHQYD
VR0OBBYEFFueXKZeHy5gqz8ZprdCRbsNl81NMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQDF29F/cgZOLUEnaciAKj8KRknM
4nHs8lGpwNkgE2f4rAC73B+4RiXVYSxdCb2KOgk6xoWmGNAvaT2n+zf0nXNp1T3H
3lTTgDnFt6fazkTz2DlbcJ+Q1F+dXVZSUWpyK3PbM2viHrckAP/TLGWJ+U0+cFvs
N1V2GOV+UMXz8Q0WcKa3KDtwNnZneiENxl2JaEGGxkv9yTrUVDegBVIUF3hOEY4j
MBti/jngI3LEqWPlx3ukk2F6vJCDSnpBJUlJ6DFuAYXWlbWiebxdAupjfhxGMa8z
UBCF1H1Y2RHFwdcywjXJPwd1YQgAgTAuK/QV3B6FcMMaI5y2FjAHu3dMPD3j
-----END CERTIFICATE-----
)";
// Clave privada (copiar y pegar el contenido de tu archivo .private.pem.key)
const char *key = R"(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAwAlbpLjSM8RadgUxKxA70RyAcsd6rTaRIN0h56oYqDDn8J7P
VwMv3FhqCDacSOg1tuwG9Qwyzz2WDdqWTKFEc7VeAWfhwl+foJTyUbuedelYQ9Ci
R4UbjtmpvZhVFg1gHuPlkzasGUDAzcaKAVv5MA8zVctwuAlqxlVB29hBLTlSX/+B
tDeuCHNDzUCwuIvWzUqVLCYvB86Nhhfva1Z614x5bohY9+C/BruDLgqwTyxsjR9w
fFErHiRuL/UvVE4HW4DCnEFUyOr69hZmY0tUF3bUfUOilIU2KoI6JL0QMHNd/8Nq
9fzp4GykKQnTFcOmLQBfFTPbT4qBPe8Ws3tLxQIDAQABAoIBAQCqHOAQyCR09SCJ
YcoPcr76W7wxcSHsJOVk7l7lgFl9TG16SsQzOUCBedTb17yUHRfQJb93/clYqh6t
wQ/ZbBZeRbCzoBlt/RVDxA3wdzpFC2NEF6JZpsElxw1FRZdCBLE8S/eHdCv55tGk
t5Zy482XVC/OdXwxmy356XaA0E3UR2LHn+gmYAM0TWEXoSy2pdkfg0pia8l963jD
fkVk9RcDhbQ12jdK7quhfIGDKVyggt3UsedjpUS8jAQLEN0fgAgGeybfcAcHPiNj
N2k3+YyPsxVLCT6//DWDOAtsRTqvftoyyyyRvaIN50FYMehL9i1+83cX3kQwlF8V
j/42+JrVAoGBAPeX921a1K2Ccz40zKr1ADgwaaP7ddvHi0Uj/1aKxXShewRaz4cG
aFVL+C694RNSbMKQE3sPdZarDgZXsgILkVe/383LaKuc7uv76WX4wUG/k1I++UXA
abLFIYr6Sn9A9edzf8E6XKMWy0dZ+JXoARDhON4B1AYkHkv0oSXvimC/AoGBAMaO
gDCB+NnwHWR9hcUKVJWQDJ6rhIrkjmkjT9dehUAvosS4ksp8ZlXk+KD6K7ROToMK
UP/maQH2jEbAElpTow4Twk39IMqMBnQeczxqgm6NNheNX96F5PWzN9StTwZefIi0
LtEzBjShgYLE63JJ55CIdu1bTEV6i9k9iy6h5TB7AoGAASjqFM4PHvz0cz8mEkrx
hFHdvNHpvGzT3zAK+ynNQP2cqOmP/U7vYC7p67Yh/sgtEEBChxoMb/c/KpJwmAfb
13BjqQAh7mPezqYwcrTWmus90m3PFx+OnzLDNHZ1sODqBfqJh6sxUeZCb4QhYPpk
PuL/NDMH/jtfFPYBZz8XA9sCgYBZU8rAB/kYsznF9+Kyns8m6kODLs7WXbTkAArv
bYqtMcEcl50Sk5zGj9h7M5Ft8LzJn16JgfND+tNpEQgwsufscV89DuAtv4k3vGtD
hvdfHRB00BmNVteG4jX/7GhdCS0lo0pcu4PTMcW+OzFhnXYcRAidCDHsIBHCjjmL
pTIi+QKBgQCrUkdwu/H6UQuQS1awzjO4pqavlj17rXXNcIzy3HAGtDNXqLGOB5CX
jB4/aNAxC/m2LWfKrfl8HPumsfmjC9lqTHpfktOj4y1wqUIEbSbiddYppW73pWaG
eyDc9DZ4UhkqpwXGcRC0qOp39XRjOc9ymiuV4cw6tP61mdXFZEv8zA==
-----END RSA PRIVATE KEY-----
)";

// Certificado raíz de Amazon (copiar y pegar el contenido de tu archivo .pem)
const char *rootCA = R"(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)";

// Instancias de clases
OledySensoryBuzzerManager oledyManager;
SonidoManager sonidoManager;

float avgBPM = 0;

void setup() {
    Serial.begin(115200);

    // Configuración Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi");

    // Configuración AWS IoT
    net.setCACert(rootCA);
    net.setCertificate(cert);
    net.setPrivateKey(key);
    client.setServer(host, port);

    // Inicializar clases
    oledyManager.init();
    sonidoManager.init();

    Serial.println("Sistema listo");
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    avgBPM = oledyManager.getHeartRate();  // Captura la frecuencia cardíaca promedio

    Serial.print("BPM Promedio: ");
    Serial.println(avgBPM);

    publishData(avgBPM);

    sonidoManager.playSoundBasedOnBPM(avgBPM);

    delay(20000);  // Esperar antes de la próxima lectura
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Intentando conectar al broker MQTT...");
        if (client.connect("Esp32-sensor")) {
            Serial.println("Conectado");
        } else {
            Serial.print("Error: ");
            Serial.println(client.state());
            delay(5000);
        }
    }
}

void publishData(float bpm) {
    String payload = "{\"state\": {\"reported\": {\"bpm\": " + String(bpm) + "}}}";
    if (client.publish("$aws/things/Esp32-sensor/shadow/update", payload.c_str())) {
        Serial.println("Datos enviados");
    } else {
        Serial.println("Error al enviar datos");
    }
}
