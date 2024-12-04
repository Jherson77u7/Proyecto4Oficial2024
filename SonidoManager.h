#ifndef SONIDO_MANAGER_H
#define SONIDO_MANAGER_H

#include <SD.h>
#include <SPI.h>

class SonidoManager {
private:
    int soundPin = 25;
    int sdChipSelectPin = 5;

public:
    SonidoManager() {}

    void init() {
        if (!SD.begin(sdChipSelectPin)) {
            Serial.println("Error al inicializar la tarjeta SD");
            while (true);
        }
    }

    void playSoundBasedOnBPM(float bpm) {
        if (bpm >= 60 && bpm <= 70) {
            playWavFile("/BeeSped5.wav");
        } else if (bpm > 70 && bpm <= 110) {
            playWavFile("/d4.wav");
        } else if (bpm > 110 && bpm <= 150) {
            playWavFile("/Nocturn.wav");
        } else {
            Serial.println("BPM fuera de rango");
        }
    }

    void playWavFile(const char *fileName) {
        File wavFile = SD.open(fileName);
        if (!wavFile) {
            Serial.println("Error al abrir archivo WAV");
            return;
        }
        Serial.println("Reproduciendo archivo WAV...");
        wavFile.close();
    }
};

#endif
