#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <MAX30105.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "driver/dac.h"
#include <Audio.h> // Biblioteca para reproducción de audio

// ***** Configuración para Wi-Fi y AWS IoT *****
const char *ssid = "Prueba";             // Tu red Wi-Fi
const char *password = "12345678";       // Tu contraseña Wi-Fi

const char *host = "a25ex5q24thysh-ats.iot.us-east-1.amazonaws.com";  // Cambia esto según tu endpoint
const int port = 8883;                    // Puerto seguro MQTT

WiFiClientSecure net;
PubSubClient client(net);

// Certificados de AWS IoT
// Certificados de AWS IoT
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
// ***** Configuración para sensor y pantalla OLED *****
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C
#define SDA_PIN 21
#define SCL_PIN 22
#define BUZZER_PIN 12

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
MAX30105 pulseSensor;

long lastBeatTime = 0;  // Tiempo del último latido
float beatsPerMinute = 0;
float bpmSum = 0;
int bpmCount = 0;

const long samplingDuration = 10000;  // Duración de 30 segundos para calcular BPM promedio

// ***** Configuración para reproducción de audio *****
#define DAC1_PIN 25 // Salida DAC1
#define CS_PIN 5    // Chip Select para el módulo microSD

const char *lowBpmFile = "/d4.wav";
const char *mediumBpmFile = "/d4.wav";
const char *highBpmFile = "/BeeSped5.wav";

void setup() {
  Serial.begin(115200);

  // Configuración Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  // Configuración de AWS IoT
  net.setCACert(rootCA);
  net.setCertificate(cert);
  net.setPrivateKey(key);
  client.setServer(host, port);

  // Inicializar pantalla OLED
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Fallo al inicializar la pantalla SSD1306");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Iniciando...");
  display.display();

  delay(2000);

  // Inicializar sensor MAX30105
  if (!pulseSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("Fallo al inicializar el sensor MAX30105");
    while (1);
  }
  pulseSensor.setup();
  pulseSensor.setPulseAmplitudeRed(0x0A);
  pulseSensor.setPulseAmplitudeIR(0x0A);

  // Configurar buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  // Configurar microSD y reproducción de audio
  if (!SD.begin(CS_PIN)) {
    Serial.println("Fallo al inicializar la tarjeta SD");
    while (1);
  }
  Serial.println("Tarjeta SD inicializada correctamente");

  // Habilitar la salida DAC
  dac_output_enable(DAC_CHANNEL_1); 

  Serial.println("Sistema listo");
}
float avgBPM = 0;

void loop() {
  // Mantener la conexión MQTT activa
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  bpmSum = 0;
  bpmCount = 0;
  long startTime = millis();

  // Tomar datos durante 60 segundos
  while (millis() - startTime < samplingDuration) {
    int redValue = pulseSensor.getRed();
    int irValue = pulseSensor.getIR();

    // Solo considerar valores cuando hay suficiente señal de pulsos
    if (redValue > 10000 || irValue > 10000) {
      long currentTime = millis();
      long delta = currentTime - lastBeatTime;

      // Si el tiempo entre latidos es razonable, calcula el BPM
      if (delta > 300) {  // 300ms es el mínimo para evitar lecturas erróneas
        lastBeatTime = currentTime;
        beatsPerMinute = 60 / (delta / 1000.0);

        // Actualiza el cálculo de BPM
        updateBpmCalculation(beatsPerMinute);

        // Activar buzzer cuando se detecta un latido válido
        tone(BUZZER_PIN, 1000, 100);  // Sonido de 1000 Hz durante 100 ms
      }
    }

    // Mostrar BPM en la pantalla OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Leyendo...");
    display.print("BPM Real: ");
    display.println(beatsPerMinute);
    display.display();

    delay(500);  // Esperar medio segundo entre lecturas
  }

  // Calcular el promedio de BPM una vez al final
  avgBPM = bpmCount > 0 ? bpmSum / bpmCount : 0;
  
  Serial.print("BPM Promedio: ");
  Serial.println(avgBPM);

  // Enviar el BPM promedio a AWS IoT
  publishData(avgBPM);

// Seleccionar y reproducir el archivo correspondiente
  if (avgBPM >= 40 && avgBPM <= 60) {
    playWavFile(lowBpmFile);
  } else if (avgBPM >= 60 && avgBPM <= 90) {
    playWavFile(mediumBpmFile);
  } else if (avgBPM >= 91 && avgBPM <= 150) {
    playWavFile(highBpmFile);
  } else {
    Serial.println("BPM fuera de rango");
  }

  // Mostrar el promedio de BPM en la pantalla OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Promedio: ");
  display.println(avgBPM);
  display.display();

  delay(60000);  // Esperar antes de la próxima lectura
}

void updateBpmCalculation(float bpm) {
  bpmSum += bpm;  // Sumar el BPM detectado
  bpmCount++;     // Incrementar el contador de latidos
}

void publishData(float bpm) {
  // Crear el objeto JSON con la estructura que necesita AWS IoT
  String payload = "{\"state\": {\"reported\": {\"bpm\": " + String(bpm) + "}}}";

  // Publicar el payload en el tema adecuado
  if (client.publish("$aws/things/Esp32-sensor/shadow/update", payload.c_str())) {
    Serial.println("Datos enviados a AWS IoT");
  } else {
    Serial.println("Error al enviar datos a AWS IoT");
  }
}

void reconnect() {
  // Conectar al broker MQTT de AWS IoT
  while (!client.connected()) {
    Serial.print("Intentando conectar al broker MQTT...");
    if (client.connect("Esp32-sensor")) {
      Serial.println("Conectado al broker MQTT de AWS IoT");
    } else {
      Serial.print("Error de conexión: ");
      Serial.print(client.state());
      delay(5000);  // Reintentar cada 5 segundos
         }
    }
}

void playWavFile(const char *fileName) {
  File wavFile = SD.open(fileName, FILE_READ);
  if (!wavFile) {
    Serial.println("No se pudo abrir el archivo WAV");
    return;
  }

  uint8_t header[4];
  wavFile.read(header, 4);
  if (strncmp((char *)header, "RIFF", 4) != 0) {
    Serial.println("No es un archivo WAV válido");
    wavFile.close();
    return;
  }

  wavFile.seek(24);
  uint32_t sampleRate = wavFile.read() | (wavFile.read() << 8);

  wavFile.seek(34);
  uint16_t bitsPerSample = wavFile.read();
  if (bitsPerSample > 8) {
    Serial.println("El archivo tiene más de 8 bits por muestra");
    wavFile.close();
    return;
  }

  Serial.println("Reproduciendo WAV...");
  while (wavFile.available()) {
    uint8_t sample = wavFile.read();
    dac_output_voltage(DAC_CHANNEL_1, sample);
    delayMicroseconds(1000000 / sampleRate);  // Controlar el ritmo de reproducción
  }

  wavFile.close();
  Serial.println("Reproducción terminada");
}
