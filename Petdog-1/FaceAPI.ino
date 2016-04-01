// the Face API subscriptions key
// https://www.microsoft.com/cognitive-services/en-us/face-api
const char faceAPIKeys[] = "your_subscriptions_key";

void initFaceAPI() {
    FAMSPO.begin(faceAPIKeys);
    // Face API callback function for get image data
    FAMSPO.onData([](uint8_t *&data, size_t &len) {
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

void startFaceDetect(uint32_t len) {
    int statusCode = -1;
    String error_code = "";
    String message = "";
    String result = "";
    //  callback funtion for parse json information
    FAMSPO.onParse([&statusCode, &error_code, &message, &result](const char* str, int code) {
        if (!str) {
            return;
        }
        DynamicJsonBuffer jsonBuffer;
        if (code != HTTP_CODE_OK) {
            JsonObject& root = jsonBuffer.parseObject(str);
            if (root.success()) {
                switch (code) {
                    case 400:
                    case 408:
                    case 415:
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
                    char buf[48];
                    sprintf(buf, "\r\nage: %d, gender: %s", 
                            (int) root[i]["faceAttributes"]["age"],
                            (const char *) root[i]["faceAttributes"]["gender"]);
                    if (((JsonObject&) root[i]["faceAttributes"]).containsKey("smile")) {
                        sprintf(&buf[strlen(buf)], ", smile: %f", 
                                (float) root[i]["faceAttributes"]["smile"], 1, 3, &buf[strlen(buf)]);
                    }
                    result = buf;
#if defined(OLED)
                    OLEDprintln(buf);
#else
                    Serial.println(buf);
#endif
                }
            } else {
                _dbgprintln("Can't parse json!");
            }
        }
    });
    if (!FAMSPO.detectFace(len)) {
        _dbgprintln(connectErr[FAMSPO.getError()]);
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
            textToSpeech(result.c_str(), 16000);
        }
    }
}

