#ifndef OLEDY_SENSORY_BUZZER_MANAGER_H
#define OLEDY_SENSORY_BUZZER_MANAGER_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>

class OledySensoryBuzzerManager {
private:
    Adafruit_SSD1306 display;
    int redPin = 21;
    int irPin = 22;
    int buzzerPin = 12;
    long lastBeatTime = 0;
    float beatsPerMinute = 0;
    float bpmSum = 0;
    int bpmCount = 0;

public:
    OledySensoryBuzzerManager() : display(128, 64, &Wire, -1) {}

    void init() {
        if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
            Serial.println("No se pudo inicializar la pantalla OLED");
            while (true);
        }
        display.clearDisplay();
    }

    float getHeartRate() {
        bpmSum = 0;
        bpmCount = 0;
        long startTime = millis();
        while (millis() - startTime < 10000) {
            int redValue = analogRead(redPin);
            int irValue = analogRead(irPin);

            if (redValue > 10000 || irValue > 10000) {
                long currentTime = millis();
                long delta = currentTime - lastBeatTime;
                if (delta > 300) {
                    lastBeatTime = currentTime;
                    beatsPerMinute = 60 / (delta / 1000.0);
                    bpmSum += beatsPerMinute;
                    bpmCount++;
                    activateBuzzer(1000, 100);
                }
            }
            updateBpmDisplay(beatsPerMinute);
            delay(500);
        }
        return bpmCount > 0 ? bpmSum / bpmCount : 0;
    }

    void updateBpmDisplay(float bpm) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("BPM: ");
        display.println(bpm);
        display.display();
    }

    void activateBuzzer(int frequency, int duration) {
        tone(buzzerPin, frequency, duration);
    }
};

#endif
