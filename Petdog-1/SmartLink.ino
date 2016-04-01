/*
 * MKR1000 can smart config
 * http://www.atmel.com/Images/Atmel-42636-ATWINC1500-AP-Provision-Mode_ApplicationNote_AT12181.pdf
 * http://www.atmel.com/Images/Atmel-42642-ATWINC1500-WiFi-Network-Controller-HTTP-Provision-Mode_ApplicationNote_AT12265.pdf
 */

// use on borad led
#define LED_PIN  6

const char beginSmartLink[] PROGMEM = "\r\nWait for the smart connection...";
const char endSmartLink[] PROGMEM = "\r\nSmart connection has been canceled.";
const char continueSmartLink[] PROGMEM = "\r\nHas been connected to the network, is to obtain an IP address...";
const char successSmartLink[] PROGMEM = "\r\nWell, we started chatting.";

void displaySmartLink(const char *str) {
#if defined(OLED)
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    OLEDprintln();
    OLEDprintln(str);
#else
    Serial.println(str);
#endif
}

void startSmartLink() {
    pinMode(LED_PIN, OUTPUT);
    
//    WiFi.init();
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char ssid[14] = "MKR1000_";
    sprintf(&ssid[8], "%02X:%02X", mac[1], mac[0]);
#if defined(OLED)
    OLEDprint("Start HTTP Provision Mode, SSID: ");
    OLEDprintln(ssid);
#else
    Serial.print("Start HTTP Provision Mode, SSID: ");
    Serial.println(ssid);
#endif
    // begin provision mode
    WiFi.beginProvision(ssid, "mkr1000config.com");
    
    displaySmartLink(beginSmartLink);
    while (!WiFi.provisioned() || WiFi.status() != WL_CONNECTED) {
        // must call to handle events
        WiFi.refresh();
        digitalWrite(LED_PIN, HIGH);
        delay(1000);
        digitalWrite(LED_PIN, LOW);
        delay(1000);
    }
    displaySmartLink(continueSmartLink);
    while (!WiFi.localIP()) {
        // must call to handle events
        WiFi.refresh();
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
        delay(500);
    }
    displaySmartLink(successSmartLink);
}

