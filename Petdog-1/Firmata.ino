/*
 * FIRMATA SETUP
 */

/*
 * Firmata -- receive string callback function
 */
void stringCallback(char *string) {
    if (string) {
        _dbgprintln(string);
        petdog(string);
    }
}

void initFirmata() {
    Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);
    // we receive string from windows 10 remote for arduino
    Firmata.attach(STRING_DATA, stringCallback);
    Firmata.begin(stream);
}

