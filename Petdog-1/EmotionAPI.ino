// the Emotion API subscriptions key
// https://www.microsoft.com/cognitive-services/en-us/emotion-api
const char emotionAPIKeys[] = "your_subscriptions_key";

void initEmotionAPI() {
    EAMSPO.begin(emotionAPIKeys);
    // Emotion API callback function for get image data
    EAMSPO.onData([](uint8_t *&data, size_t &len) {
        static uint8_t *buf = NULL;
        if (buf == NULL) {
            buf = (uint8_t *) malloc(HTTP_SEND_SIZE);
            if (buf == NULL) {
                return;
            }
        }
        if (imageDataCounts --) {
            SPI.transfer(buf, HTTP_SEND_SIZE);
            len = HTTP_SEND_SIZE;
            data = buf;
        } else {
            free(buf);
            buf = NULL;
        }
    });
}

void startEmotionRecognize(uint32_t len) {
    int statusCode = -1;
    String error_code = "";
    String message = "";
    String result = "";
    //  callback funtion for parse json information
    EAMSPO.onParse([&statusCode, &error_code, &message, &result](const char* str, int code) {
        if (!str) {
            return;
        }
        DynamicJsonBuffer jsonBuffer;
        if (code != HTTP_CODE_OK) {
            JsonObject& root = jsonBuffer.parseObject(str);
            if (root.success()) {
                switch (code) {
                    case 400:
                        error_code = (const char *) root["error"]["code"];
                        message = (const char *) root["error"]["message"];
                        break;

                    case 401:
                    case 403:
                    case 429:
                        statusCode = root["error"]["statusCode"];
                        message = (const char *) root["error"]["message"];
                        break;

                    case 404:
                        statusCode = root["statusCode"];
                        message = (const char *) root["message"];
                        break;
                    
                }
            }
        } else {
            JsonArray& root = jsonBuffer.parseArray(str);
            if (root.success()) {
                int size = root.size();
                for (int i = 0; i < size; i ++) {
                    double value1 = 0, value2;
                    static const char *emotions[] PROGMEM = {
                        "anger", "contempt", "disgust", "fear", "happiness", "neutral", "sadness", "surprise"
                    };
                    int index = 0;
                    if (strstr(root[i]["scores"]["anger"], "E-") == NULL) {
                        value1 = (double) root[i]["scores"]["anger"];
                    }
                    if (strstr(root[i]["scores"]["contempt"], "E-") == NULL) {
                        value2 = (double) root[i]["scores"]["contempt"];
                        if (value1 < value2) {
                            value1 = value2;
                            index = 1;
                        }
                    }
                    if (strstr(root[i]["scores"]["disgust"], "E-") == NULL) {
                        value2 = (double) root[i]["scores"]["disgust"];
                        if (value1 < value2) {
                            value1 = value2;
                            index = 2;
                        }
                    }
                    if (strstr(root[i]["scores"]["fear"], "E-") == NULL) {
                        value2 = (double) root[i]["scores"]["fear"];
                        if (value1 < value2) {
                            value1 = value2;
                            index = 3;
                        }
                    }
                    if (strstr(root[i]["scores"]["happiness"], "E-") == NULL) {
                        value2 = (double) root[i]["scores"]["happiness"];
                        if (value1 < value2) {
                            value1 = value2;
                            index = 4;
                        }
                    }
                    if (strstr(root[i]["scores"]["neutral"], "E-") == NULL) {
                        value2 = (double) root[i]["scores"]["neutral"];
                        if (value1 < value2) {
                            value1 = value2;
                            index = 5;
                        }
                    }
                    if (strstr(root[i]["scores"]["sadness"], "E-") == NULL) {
                        value2 = (double) root[i]["scores"]["sadness"];
                        if (value1 < value2) {
                            value1 = value2;
                            index = 6;
                        }
                    }
                    if (strstr(root[i]["scores"]["surprise"], "E-") == NULL) {
                        value2 = (double) root[i]["scores"]["surprise"];
                        if (value1 < value2) {
                            value1 = value2;
                            index = 7;
                        }
                    }
                    result = emotions[index];
#if defined(OLED)
                    OLEDprintln(emotions[index]);
#else
                    Serial.println(emotions[index]);
#endif
                }
            } else {
                _dbgprintln("Can't parse json!");
            }
        }
    });
    if (!EAMSPO.recognizeEmotion(len)) {
        _dbgprintln(connectErr[EAMSPO.getError()]);
    } else {
        if (statusCode != -1) {
            _dbgprintln(statusCode);
        } else if (!error_code.equals("")) {
            _dbgprintln(error_code);
        }
        if (!message.equals("")) {
#if defined(OLED)
            OLEDprintln(message.c_str());
#else
            Serial.println(message.c_str());
#endif
        }
        if (result.length()) {
            Firmata.sendString(result.c_str());
            textToSpeech(result.c_str(), 16000);
        }
    }
}

