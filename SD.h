#ifndef SD_MANAGER_H
#define SD_MANAGER_H

#include <SD.h>

class SDManager {
private:
    int chipSelectPin;

public:
    SDManager(int csPin) : chipSelectPin(csPin) {}

    bool init() {
        if (!SD.begin(chipSelectPin)) {
            Serial.println("Error al inicializar la tarjeta SD");
            return false;
        }
        return true;
    }

    File openFile(const char *fileName) {
        File file = SD.open(fileName, FILE_READ);
        if (!file) {
            Serial.print("No se pudo abrir el archivo: ");
            Serial.println(fileName);
        }
        return file;
    }
};

#endif // SD_MANAGER_H
