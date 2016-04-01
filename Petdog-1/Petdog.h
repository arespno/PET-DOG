#ifndef _PETDOG_H
#define _PETDOG_H

// To see debug information, leave the #define below uncommented.
//#define __DEBUG

// If use OLED, leave the #define below uncommented.
#define OLED

#if defined(__DEBUG)
#if defined(OLED)
#define _dbgprint(...)  { if (display.getCursorY() >= display.height()) { display.clearDisplay(); display.setCursor(0, 0); } display.setTextSize(1); display.setTextColor(WHITE, BLACK); display.print(__VA_ARGS__); display.display(); }
#define _dbgprintln(...)  { if (display.getCursorY() >= display.height()) { display.clearDisplay(); display.setCursor(0, 0); } display.setTextSize(1); display.setTextColor(WHITE, BLACK); display.println(__VA_ARGS__); display.display(); }
#else
#define _dbgprint(...)  Serial.print(__VA_ARGS__)
#define _dbgprintln(...)  Serial.println(__VA_ARGS__)
#endif
#else
#define _dbgprint(...)
#define _dbgprintln(...)
#endif

// display ssid, ip
void printWiFiStatus();
// MRK1000 smart config
void startSmartLink();
// get distance from sharp ir
int getDistance();
boolean isCloseTo(int distance);
boolean isFarAway(int distance);

/*
 * VS1053 config
 */
// vs1053 pins
#define DREQ  19
#define XCS  17
#define XDCS  18
#define XRESET  6

/*
 * SD config
 */
// SD card cs pin
#define SD_CS  4

/*
 * SPIRAM config
 */
// SPI ram cs pin 
#define SPIRAM_CS  2

/*
 * Display config
 */
// display pins
#define DISPLAY_CS  5
#define DISPLAY_DC  21
#define DISPLAY_RST  20

/*
 * Camera config
 */
// Camera cs pin
#define CAMERA_CS  3
void initCamera();
void doCapture();
void faceDetect(ArduCAM);
void emotionRecognize(ArduCAM);

/*
 * Firmata config
 */
void initFirmata();

/*
 * Face API
 */
void initFaceAPI();
void startFaceDetect(uint32_t);

/*
 * Emotion API
 */
void initEmotionAPI();
void startEmotionRecognize(uint32_t);

/*
 * Speech API
 */
void initSpeechAPI();
// speech to text
void startSpeech(char *, uint16_t);
// text to speech
void textToSpeech(const char *, uint16_t);

/*
 * Speaker Recognition API
 */
void initSpeakerRecognitionAPI();
void startEnrollment(char *, uint16_t);

/*
 * Response
 */
// find a string in string array
int findString_P(const char **, const char *);
// when get a instructions, pet dog do it
void startActivity(const char *, const char *, const char *);
void stopActivity();
// now it can calculate
uint32_t calculate(const char **, const char **);

/*
 * pet dog servo control
 */
#define STOPACTION  0
#define FORWARDACTION  1
#define BACKACTION  2
#define LEFTACTION  3
#define RIGHTACTION  4
#define RIGHTHANDACTION  6
#define SITDOWNACTION  7
#define LEFTHANDACTION  8
#define GOPRONEACTION  9
boolean servoLoop();
void petdog(const char *);
void petdogAction(uint8_t);
void headTurnLeft();
void headTurnRight();
void headTurnForward();
void headLiftUp();
void headDown();

/*
 * vs1053
 */
boolean record(char *, uint16_t);
void play(char *);
 
/*
 * Message for HTTP(s) connect
 */
const char* connectErr[] PROGMEM = {
    "Success", "No key", "Connect failed", "Response out time", "Not http 200 OK", "Out of memory", "Params error",
};

/*
 * WAVE file header
 */
uint8_t riff[] = {
    'R', 'I', 'F', 'F', 0x20, 0x20, 0x20, 0x20, 'W', 'A', 'V', 'E', 'f', 'm', 't', 0x20,
    0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x40, 0x1f, 0x00, 0x00, 0x80, 0x3e, 0x00, 0x00,
    0x02, 0x00, 0x10, 0x00,
    'd', 'a', 't', 'a', 0x20, 0x20, 0x20, 0x20,
};

/*
 * language understanding intelligent service intents and entities
 */
const char *intents[] PROGMEM = {
    "StartActivity",
    "StopActivity",
    "Calculate",
    "None",
    "",
};

const char *entities[] PROGMEM = {
    "ActivityType",
    "WayType",
    "PartsType",
    "SignType",
    "builtin.number",
    "",
};

#define CULTURE_EN_US  0
#define CULTURE_ZH_CN  1

uint8_t culture = CULTURE_EN_US;

uint8_t lang = culture == CULTURE_EN_US ? SPEECHAPI_en_US : SPEECHAPI_zh_CN;

#endif  // _PETDOG_H
