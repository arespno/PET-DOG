// the Speaker Recognition API subscriptions key
// https://www.microsoft.com/cognitive-services/en-us/speaker-recognition-api
const char speakerAPIKeys[] = "your_subscriptions_key";

char identifyGUID[37] = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
char verifyGUID[37] = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";

// TODO

void initSpeakerRecognitionAPI() {
    SRAMSPO.begin(speakerAPIKeys);
    //  callback funtion for parse json information
    SRAMSPO.onParse([](const char *str, int code) {
        if (!str) {
            return;
        }
        _dbgprint("Http/");
        _dbgprintln(code);
        _dbgprintln(str);
        if (code == HTTP_CODE_OK) {
            DynamicJsonBuffer jsonBuffer;
            if (str[0] == '[') {
                JsonArray& root = jsonBuffer.parseArray(str);
                if (root.success()) {
                    root.prettyPrintTo(Serial);
                }
            } else {
                JsonObject& root = jsonBuffer.parseObject(str);
                if (root.success()) {
                    root.prettyPrintTo(Serial);
                }
            }
        }
    });
}

void startEnrollment(char *recordName, uint16_t sampleRate) {
    if (!record(recordName, sampleRate)) {
        return;
    }

    if (SRAMSPO.createProfile(verifyGUID, false, LOCALE_EN_US)) {
        _dbgprintln(verifyGUID);
    } else {
        _dbgprintln(connectErr[SRAMSPO.getError()]);
    }

    if (!SRAMSPO.listAllPhrases(LOCALE_EN_US)) {
        _dbgprintln(connectErr[SRAMSPO.getError()]);
    }

    uint8_t *voiceBuffer = NULL;
    String result = "";
    File file;
    if (recordName) {
        file = SD.open(recordName);
    } else {
        SPIRAM.open(SPIRAM_READONLY);
    }
    SRAMSPO.onParse([&result](const char *str, int code) {
        if (!str) {
            return;
        }
        _dbgprint("Http/");
        _dbgprintln(code);
        _dbgprintln(str);

        if (code == 202) {
            result = str;
        }
    });

    SRAMSPO.onData([&voiceBuffer, recordName, &file](uint8_t *&data, size_t &len) {
        if (voiceBuffer == NULL) {
            voiceBuffer = (uint8_t *) malloc(HTTP_SEND_SIZE);
            if (voiceBuffer == NULL) {
                return;
            }
        }
        size_t size;
        if (recordName) {
            size = file.read(voiceBuffer, HTTP_SEND_SIZE);
        } else {
            size = SPIRAM.read(voiceBuffer, HTTP_SEND_SIZE);
        }
        if (size > 0) {
            data = voiceBuffer;
            len = size;
        } else {
            free(voiceBuffer);
            voiceBuffer = NULL;
        }
    });

    size_t size;
    if (recordName) {
        size = file.size();
    } else {
        size = SPIRAM.size();
    }

    if (!SRAMSPO.createEnrollment(verifyGUID, false, size)) {
//    if (!SRAMSPO.verification(verifyGUID, size)) {
        _dbgprintln(connectErr[SRAMSPO.getError()]);
    }
    if (recordName) {
        file.close();
    }
    if (result.length()) {
        SRAMSPO.onParse([](const char *str, int code) {
            if (!str) {
                return;
            }
            _dbgprint("Http/");
            _dbgprintln(code);
            _dbgprintln(str);
        });
        if (!SRAMSPO.getOperationStatus(result.c_str())) {
            _dbgprintln(connectErr[SRAMSPO.getError()]);
        }
    }
}

