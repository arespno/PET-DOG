#include <ArduinoJson.h>
#include <Code.h>
#include <EmotionAPI.h>
#include <FaceAPI.h>
#include <FaceRecognition.h>
#include <HttpRequest.h>
#include <SpeakerRecognitionAPI.h>
#include <SpeechAPI.h>
#include <SpiRam.h>
#include <VS1053.h>

#include <Firmata.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <Wire.h>

// this include is for camera
#include <ArduCAM.h>

#include "memorysaver.h"
#include "wiring_private.h"

// follow the instructions in wifiConfig.h to configure your particular hardware
#include "wifiConfig.h"

#include "Petdog.h"

#if defined(OLED)
// oled display
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(DISPLAY_DC, DISPLAY_RST, DISPLAY_CS);
#endif

void setup() {
    // Serial is usb serial
    Serial.begin(115200);
    // Serial1 is rx, tx
    Serial1.begin(115200);
    // Firmata stream begin connect to WiFi
    stream.begin(SERVER_PORT);
    unsigned long t = millis() + 500;
    while (!Serial && t > millis()) {
    }
/*
 * Thank for lady ada
 * https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports?view=all
 * modified C:\Users\(yourName)\AppData\Local\Arduino15\packages\arduino-mkr1000\hardware\samd\1.6.4-mkr01\variants\mkr1000\variant.h
// SPI Interfaces
// --------------
#define SPI_INTERFACES_COUNT 3 <- 2
 * and add below lines
// SPI2: Connected to external SRAM
#define PIN_SPI2_MISO  (7u)
#define PIN_SPI2_MOSI  (0u)
#define PIN_SPI2_SCK   (1u)
#define PIN_SPI2_SS    (2u)
#define PERIPH_SPI2    sercom3
#define PAD_SPI2_TX    SPI_PAD_0_SCK_1
#define PAD_SPI2_RX    SERCOM_RX_PAD_3
static const uint8_t SS2   = PIN_SPI2_SS;
static const uint8_t MOSI2 = PIN_SPI2_MOSI;
static const uint8_t MISO2 = PIN_SPI2_MISO;
static const uint8_t SCK2  = PIN_SPI2_SCK;
 */
    SPI2.begin();
    // Assign pins 0, 1 & 7 SERCOM functionality
    pinPeripheral(MOSI2, PIO_SERCOM);
    pinPeripheral(SCK2, PIO_SERCOM);
    pinPeripheral(MISO2, PIO_SERCOM_ALT);

#if defined(OLED)
    // initialize oled
    display.begin(SSD1306_SWITCHCAPVCC);
    // init done

    // Show image buffer on the display hardware.
    // Since the buffer is intialized with an Adafruit splashscreen
    // internally, this will display the splashscreen.
    display.display();
#endif

    // initialize camera, spi ram, vs1053, sd card
    initCamera();
    SPIRAM.begin(SPIRAM_CS);
    VSSound.begin(XCS, XDCS, XRESET, DREQ);
    VSSound.setVolume(0x3030);

    if (!SD.begin(SD_CS)) {
#if defined(OLED)
        OLEDprintln("SD card initialize failed!");
#else
        Serial.println("SD card initialize failed!");
#endif
    }

    initEmotionAPI();
    initFaceAPI();
    initSpeakerRecognitionAPI();
    initSpeechAPI();

#if defined(OLED)
    display.dim(true);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.display();
#endif
}

void loop() {
    static boolean wifiOK = false;
    static unsigned long wifiTime = millis() + 5000;

    if (!wifiOK) {
        if (!WiFi.provisioned() || WiFi.status() != WL_CONNECTED || !WiFi.localIP()) {
#if defined(OLED)
            OLEDprint('.');
#else
            Serial.print('.');
#endif
            delay(500);
            if (wifiTime < millis()) {
                wifiTime = 0xffffffff;
                startSmartLink();
            }
            return;
        }
        wifiOK = true;
        printWiFiStatus();
        initFirmata();
    }
    while (Firmata.available()) {
        Firmata.processInput();
    }
    servoLoop();
    if (isCloseTo(60)) {
        petdogAction(STOPACTION);
        while (!servoLoop()) {
        }
        startSpeech(NULL, 8000);
    }
    stream.maintain();
}

void printWiFiStatus() {
    IPAddress ip = WiFi.localIP();
#if defined(OLED)
    OLEDprint("ssid: ");
    OLEDprintln(WiFi.SSID());
    OLEDprint("ip: ");
    OLEDprintln(ip);
#else
    Serial.print("ssid: ");
    Serial.println(WiFi.SSID());
    Serial.print("ip: ");
    Serial.println(ip);
#endif
}

