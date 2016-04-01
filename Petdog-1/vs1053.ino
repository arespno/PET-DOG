// record voice data to sd card with recordName, or to spi ram when recordName is nullptr
boolean record(char *recordName, uint16_t sampleRate) {
#if defined(OLED)
    OLEDprintln("Start record...");
#else
    Serial.println("Start record...");
#endif
    File file;
    if (recordName) {
        SD.remove(recordName);
        file = SD.open(recordName, FILE_WRITE);
        if (!file) {
#if defined(OLED)
            OLEDprintln("Create file error.");
#else
            Serial.println("Create file error.");
#endif
            return false;
        }
        *((uint32_t *) &riff[0x18]) = sampleRate;
        *((uint32_t *) &riff[0x1c]) = sampleRate * 2;
        file.write(riff, 44);

        VSSound.initRecord(sampleRate);

        while (!isFarAway(60)) {
            uint16_t hdat1 = VSSound.sciRead(SCI_HDAT1);
            if (hdat1 >= 256 && hdat1 < 896) {
                uint16_t val[256];
                for (uint16_t i = 0; i < 256; i ++) {
                    val[i] = VSSound.sciRead(SCI_HDAT0);
                }
                if (file.write((uint8_t *) val, 512) < 512) {
                    break;
                }
            }
        }

        VSSound.finishRecord();

        uint32_t position = file.position();
        position -= 0x08;
        file.seek(0x04);
        file.write(position & 0xff);
        file.write((position >> 0x08) & 0xff);
        file.write((position >> 0x10) & 0xff);
        file.write((position >> 0x18) & 0xff);
        position -= 0x24;
        file.seek(0x28);
        file.write(position & 0xff);
        file.write((position >> 0x08) & 0xff);
        file.write((position >> 0x10) & 0xff);
        file.write((position >> 0x18) & 0xff);
        file.close();

    } else {
        SPIRAM.open(SPIRAM_OVERRIDE);
        *((uint32_t *) &riff[0x18]) = sampleRate;
        *((uint32_t *) &riff[0x1c]) = sampleRate * 2;
        SPIRAM.write(riff, 44);

        VSSound.initRecord(sampleRate);

        while (!isFarAway(60)) {
            uint16_t hdat1 = VSSound.sciRead(SCI_HDAT1);
            if (hdat1 >= 256 && hdat1 < 896) {
                uint16_t val[256];
                for (uint16_t i = 0; i < 256; i ++) {
                    val[i] = VSSound.sciRead(SCI_HDAT0);
                }
                if (SPIRAM.write((uint8_t *) val, 512) < 512) {
                    break;
                }
            }
        }
    
        VSSound.finishRecord();

        uint32_t position = SPIRAM.position();
        position -= 0x08;
        SPIRAM.seek(0x04);
        SPIRAM.write(position & 0xff);
        SPIRAM.write((position >> 0x08) & 0xff);
        SPIRAM.write((position >> 0x10) & 0xff);
        SPIRAM.write((position >> 0x18) & 0xff);
        position -= 0x24;
        SPIRAM.seek(0x28);
        SPIRAM.write(position & 0xff);
        SPIRAM.write((position >> 0x08) & 0xff);
        SPIRAM.write((position >> 0x10) & 0xff);
        SPIRAM.write((position >> 0x18) & 0xff);

    }
#if defined(OLED)
    OLEDprintln("Record finished.");
#else
    Serial.println("Record finished.");
#endif

//    play(recordName);
    return true;
}

// play voice data in sd card file with playName, or in spi ram when playName is nullptr
void play(char *playName) {
    if (playName) {
        File file = SD.open(playName);
        if (!file) {
#if defined(OLED)
            OLEDprint("Can't open ");
            OLEDprintln(playName);
#else
            Serial.print("Can't open ");
            Serial.println(playName);
#endif
            return;
        }
        delay(10);
        while (file.available()) {
            uint8_t val[32];
            uint8_t len = file.read(val, 32);
            VSSound.sdiWrite(val, len);
        }
        file.close();
    } else {
        SPIRAM.open(SPIRAM_READONLY);
        _dbgprintln(SPIRAM.position());
        _dbgprintln(SPIRAM.size());
        while (SPIRAM.available()) {
            uint8_t val[32];
            VSSound.sdiWrite(val, SPIRAM.read(val, 32));
        }
    }
}

