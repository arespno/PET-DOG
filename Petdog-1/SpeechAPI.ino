// the Speech API subscriptions key
// https://www.microsoft.com/cognitive-services/en-us/speech-api
const char speechAPIKeys[] = "your_subscriptions_key";

// WOW!!! LUIS programmatic APIS are out now! but 3-31 to closed
// the LUIS subscriptions key
// https://www.microsoft.com/cognitive-services/en-us/language-understanding-intelligent-service-luis
const char luisSubscriptionKey[] = "your_subscriptions_key";
// the LUIS application keys
const char luisAppId_en_US[] = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
const char luisAppId_zh_CN[] = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";

const char *luisAppId = culture == CULTURE_EN_US ? luisAppId_en_US : luisAppId_zh_CN;

#define STARTACTIVITY  0
#define STOPACTIVITY  1
#define CALCULATE  2

char speechAppId[37] = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
char speechClientId[37] = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";

void initSpeechAPI() {
    // get speech AccessToken need guid no include dashes
    CODE.cguid(speechAppId, false);
    CODE.cguid(speechClientId, false);
    SAMSPO.begin(speechAppId, speechClientId, speechAPIKeys);
}

void startSpeech(char *recordName, uint16_t sampleRate) {
    if (!record(recordName, sampleRate)) {
        return;
    }
    
    uint8_t *voiceBuffer = NULL;
    String result = "";
    File file;
    if (recordName) {
        file = SD.open(recordName);
        if (!file) {
#if defined(OLED)
            display.print("Can't open ");
            display.println(recordName);
            display.display();
#else
            Serial.print("Can't open ");
            Serial.println(recordName);
#endif
            return;
        }
    } else {
        SPIRAM.open(SPIRAM_READONLY);
    }
    // the callback funtion when need speech data
    SAMSPO.onDataRecognize([&voiceBuffer, recordName, &file](uint8_t *&data, size_t &len) {
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
    // the callback function when speech recognize return to parse result
    SAMSPO.onParseRecognize([&result](const char* str, int code) {
        if (!str) {
            return;
        }
        if (code == HTTP_CODE_OK) {
            DynamicJsonBuffer jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject(str);
            if (root.success()) {
                const char *status = (const char *) root["header"]["status"];
                if (status) {
                    if (strstr(status, "success")) {
                        result = (const char *) root["results"][0]["name"];
                        _dbgprintln(result);
                    }
                }
            }
        }
    });
    size_t size;
    if (recordName) {
        size = file.size();
    } else {
        size = SPIRAM.size();
    }
    while (!SAMSPO.recognize(size, sampleRate, lang)) {
        if (SAMSPO.getError() == ERR_HTTPREQUEST_NOKEY) {
            if (!SAMSPO.acquireAccessToken()) {
                _dbgprintln(connectErr[SAMSPO.getError()]);
            } else {
                _dbgprintln(SAMSPO.getAccessToken());
                continue;
            }    
        } else {
            _dbgprintln(connectErr[SAMSPO.getError()]);
        }
        break;
    }
    if (recordName)  {
        file.close();
    }
    if (result.length()) {
        if (culture == CULTURE_EN_US) {
            if (strstr(result.c_str(), "How old i am")) {
                doCapture();
                return;
            }
        } else if (culture == CULTURE_ZH_CN) {
            if (strstr(result.c_str(), "我几岁")) {
                doCapture();
                return;
            }
        }
    }

    // the callback function when language understanding intelligent service reutrn result
    // now we have three intents
    SAMSPO.onParseLUIS([&result](const char* str, int code) {
        result = "";
        if (!str) {
            return;
        }
        if (code == HTTP_CODE_OK) {
            DynamicJsonBuffer jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject(str);
            if (root.success()) {
                const char *ptr = (const char *) root["intents"][0]["intent"];
                int intentIndex = findString_P(intents, ptr);
                if (intentIndex == -1) {
                    _dbgprint("Unknown intent: ");
                    _dbgprintln(ptr);
                    return;
                }
                size_t size = root["intents"][0]["actions"].size();
                if (!size) {
                    _dbgprintln("Not capture actions!");
                }
                switch (intentIndex) {
                    case STARTACTIVITY:
                        if (size) {
                            size = root["intents"][0]["actions"][0]["parameters"].size();
                            if (size) {
                                const char *activity = root["intents"][0]["actions"][0]["parameters"][0]["value"].size() > 0 ? 
                                                    (const char *) root["intents"][0]["actions"][0]["parameters"][0]["value"][0]["entity"] : NULL;
                                const char *way = root["intents"][0]["actions"][0]["parameters"][1]["value"].size() > 0 ? 
                                                    (const char *) root["intents"][0]["actions"][0]["parameters"][1]["value"][0]["entity"] : NULL;
                                const char *parts = root["intents"][0]["actions"][0]["parameters"][2]["value"].size() > 0 ? 
                                                    (const char *) root["intents"][0]["actions"][0]["parameters"][2]["value"][0]["entity"] : NULL;
                                startActivity(activity, way, parts);
                            }
                        }
                        break;
            
                    case STOPACTIVITY:
                        stopActivity();
                        break;
            
                    case CALCULATE:
                        if (size) {
                            size = root["intents"][0]["actions"][0]["parameters"].size();
                            if (size) {
                                size_t numSize = root["intents"][0]["actions"][0]["parameters"][0]["value"].size();
                                size_t signSize = root["intents"][0]["actions"][0]["parameters"][1]["value"].size();
                                if (numSize > 1 && signSize > 0) {
                                    const char **num = new const char *[numSize + 1];
                                    const char **sign = new const char *[signSize + 1];
                                    for (size_t i = 0; i < numSize; i ++) {
                                        num[i] = (const char *) root["intents"][0]["actions"][0]["parameters"][0]["value"][i]["entity"];
                                    }
                                    num[numSize] = NULL;
                                    for (size_t i = 0; i < signSize; i ++) {
                                        sign[i] = (const char *) root["intents"][0]["actions"][0]["parameters"][1]["value"][i]["entity"];
                                    }
                                    sign[signSize] = NULL;
                                    result = calculate(num, sign);
                                    delete []sign;
                                    delete []num;
                                }
                            }
                        }
                        break;
                }
            }
        }
    });
    if (!result.equals("")) {
        if (!SAMSPO.LUIS(luisAppId, luisSubscriptionKey, result.c_str())) {
            result = "";
            _dbgprintln(connectErr[SAMSPO.getError()]);
        }
    }
    if (result.length()) {
        textToSpeech(result.c_str(), 16000);
    }
}

void textToSpeech(const char *str, uint16_t sampleRate) {
    VSSound.softReset();
    // callback funtion when receive text to speech voice data
    SAMSPO.onDataSynthesize([](uint8_t *data, size_t len) {
        if (!data || !len) {
            return;
        }
        // simple to play
        VSSound.sdiWrite(data, len);
    });
    // callback funtion when text to speech failed
    SAMSPO.onParseSynthesize([](const char* str, int code) {
        if (!str) {
            return;
        }
#if defined(OLED)
        display.print("Http/");
        display.println(code);
        display.println(str);
        display.display();
#else
        Serial.print("Http/");
        Serial.println(code);
        Serial.println(str);
#endif
    });
    while (!SAMSPO.synthesize(str, sampleRate, false, lang)) {
        if (SAMSPO.getError() == ERR_HTTPREQUEST_NOKEY) {
            if (!SAMSPO.acquireAccessToken()) {
                _dbgprintln(connectErr[SAMSPO.getError()]);
            } else {
                _dbgprintln(SAMSPO.getAccessToken());
                continue;
            }    
        } else {
            _dbgprintln(connectErr[SAMSPO.getError()]);
        }
        break;
    }
    VSSound.softReset();
}

