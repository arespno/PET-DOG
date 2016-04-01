/*
 * Thank for RAPIRO
 */

#define SHIFT 7
#define TIME 10     // Column of Time
#define MAXSN 10    // Max Number of Servos
#define MAXMN 10    // Max Number of Motions
#define ERR -1     // Error

// Fine angle adjustments (degrees)
int trim[MAXSN] = {
    -20, // head
    -12, // neck
    0,  // off forefoot shin
    -8,  // off forefoot thigh
    0,  // near forefoot shin
    3,  // near forefoot thigh
    -3,  // off hind foot shin
    0,  // off hind foot thigh
    0, // near hind foot shin
    5, // near hind foot thigh
};

unsigned long startTime = 0;
unsigned long endTime = 0;
uint8_t bufferTime = 0;

uint8_t motionNumber = 0;
uint8_t frameNumber = 0;
char mode = 'M';
uint8_t theMotion = 0xFF;
char theMode = ' ';
uint8_t headAngle;
uint8_t neckAngle;
uint8_t headNeckTime;

const char *pNumber[MAXSN] = {
    " #10 P",
    " #11 P",
    " #7 P",
    " #12 P",
    " #6 P",
    " #13 P",
    " #24 P",
    " #14 P",
    " #25 P",
    " #15 P"
};

struct Motion {
    uint8_t repeat;
    uint8_t frames;
    const uint8_t* ptrFrame;
};

// 0 Stop
const uint8_t frameStop[] PROGMEM = {
     90, 90, 90, 90, 90, 90, 75, 95,105, 85, 10,
     90, 90, 90, 90, 90, 90, 75, 95,105, 85,  0,
};

// 1 Forward
const uint8_t frameForward[] PROGMEM = {
     90, 90, 90,105, 75,150, 65,110,120, 60,  3,
     90, 90,150, 75, 90,105, 40, 90, 90, 45,  3,
     90, 90,105, 30, 90, 75, 60,120,115, 70,  3,
     90, 90, 90, 75, 30,105, 90,135,140, 90,  3,
};

// 2 Back
const uint8_t frameBack[] PROGMEM = {
     90, 90, 90, 75,105, 40,115, 70, 60,120,  5,
     90, 90, 40,105, 90, 75,140, 90, 90,135,  5,
     90, 90, 75,140, 90,105,120, 60, 65,110,  5,
     90, 90, 90,105,140, 75, 90, 45, 40, 90,  5,
};

// 3 Turn left
const uint8_t frameLeft[] PROGMEM = {
     90, 90, 90,105,105, 40, 65,110, 60,120,  5,
     90, 90,150, 75, 90, 75, 40, 90, 90,135,  5,
     90, 90,105, 30, 90,105, 60,120, 65,110,  5,
     90, 90, 90, 75,140, 75, 90,135, 40, 90,  5,
};

// 4 Turn right
const uint8_t frameRight[] PROGMEM = {
     90, 90, 90, 75, 75,150,115, 70,120, 60,  5,
     90, 90, 40,105, 90,105,140, 90, 90, 45,  5,
     90, 90, 75,140, 90, 75,120, 60,115, 70,  5,
     90, 90, 90,105, 30,105, 90, 45,140, 90,  5,
};

// 5 The hands
const uint8_t frameTheHands[] PROGMEM = {
     90, 90, 90, 90, 90, 90, 75, 95,105, 85, 10,
     90, 90, 90, 90, 90, 90, 75, 95,105, 85,  0,
};

// 6 Right hand
const uint8_t frameRightHand[] PROGMEM = {
     85, 90, 90, 95, 45, 85, 70, 75,110,105,  3,
     80, 90,135,100, 90, 80, 70, 55,110,125,  3,
     90, 90, 30,  0, 90, 75, 70, 40,110,140,  5,
     90, 90, 60,  0, 90, 75, 70, 40,110,140,  5,
     90, 90, 30,  0, 90, 75, 70, 40,110,140,  5,
     90, 90, 60,  0, 90, 75, 70, 40,110,140,  5,
};

// 7 Sit down
const uint8_t frameSitDown[] PROGMEM = {
     85, 90,125, 95, 90, 85, 70, 75,110,105,  3,
     80, 90, 90,100, 55, 80, 70, 55,110,125,  3,
     75, 90, 90,105, 90, 75, 70, 40,110,140,  3,
     75, 90, 90,105, 90, 75, 70, 40,110,140, 20,
};

// 8 Left hand
const uint8_t frameLeftHand[] PROGMEM = {
     85, 90,135, 95, 90, 85, 70, 75,110,105,  3,
     80, 90, 90,100, 45, 80, 70, 55,110,125,  3,
     90, 90, 90,105,150,180, 70, 40,110,140,  5,
     90, 90, 90,105,120,180, 70, 40,110,140,  5,
     90, 90, 90,105,150,180, 70, 40,110,140,  5,
     90, 90, 90,105,120,180, 70, 40,110,140,  5,
};

// 9 Go prone
const uint8_t frameGoProne[] PROGMEM = {
     95, 90, 85, 70, 95,110, 75, 75,105,105,  3,
    100, 90, 80, 50,100,130, 70, 55,110,125,  3,
    105, 90, 75, 30,105,150, 70, 35,110,145,  3,
    110, 90, 70, 15,110,165, 70, 20,110,160,  3,
    110, 90, 70, 15,110,165, 70, 20,110,160, 20,
};

Motion motions[MAXMN] = {
    { 0, 2, frameStop },
    { 1, 4, frameForward },
    { 1, 4, frameBack },
    { 1, 4, frameLeft },
    { 1, 4, frameRight },
    { 0, 2, frameTheHands },
    { 0, 6, frameRightHand },
    { 0, 4, frameSitDown },
    { 0, 6, frameLeftHand },
    { 0, 5, frameGoProne },
};

uint8_t bufferAngle[MAXSN] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

const char headLeftCommand[] PROGMEM = "#head-left%";
const char headRightCommand[] PROGMEM = "#head-right%";
const char headForwardCommand[] PROGMEM = "#head-forward%";
const char headUpCommand[] PROGMEM = "#head-up%";
const char headDownCommand[] PROGMEM = "#head-down%";
const char stopCommand[] PROGMEM = "#stop%";
const char forwardCommand[] PROGMEM = "#forward%";
const char backCommand[] PROGMEM = "#back%";
const char leftCommand[] PROGMEM = "#left%";
const char rightCommand[] PROGMEM = "#right%";
const char handRightCommand[] PROGMEM = "#hand-right%";
const char sitdownCommand[] PROGMEM = "#sit-down%";
const char handLeftCommand[] PROGMEM = "#hand-left%";
const char goproneCommand[] PROGMEM = "#go-prone%";

// if a frame finished, return true, else rturn false
boolean servoLoop() {
    boolean ret = false;

    if (endTime > millis()) {
    } else if (mode == 'M') {
        if (theMotion != 0xFE) {
            nextFrame();
        }
        if (!frameNumber) {
            if (theMotion < 0xFE) {
                motionNumber = theMotion;
                theMotion = 0xFF;
            } else if (!motions[motionNumber].repeat) {
                theMotion = 0xFE;
                ret = true;
            }
            if (theMode != ' ') {
                bufferAngle[0] = headAngle;
                bufferAngle[1] = neckAngle;
                bufferTime = headNeckTime;
                mode = theMode;
                theMode = ' ';
            }
        }
    } else if (mode == 'P') {
        nextPose();
        ret = true;
    }
    return ret;
}

// Motion Play
void nextFrame() {
    for (int i = 0; i < MAXSN; i ++) {
        bufferAngle[i] = pgm_read_byte(&(motions[motionNumber].ptrFrame[frameNumber * (MAXSN + 1) + i]));
    }
    bufferTime = pgm_read_byte(&(motions[motionNumber].ptrFrame[frameNumber * (MAXSN + 1) + TIME]));

    nextPose();

    frameNumber ++;
    if (frameNumber >= motions[motionNumber].frames) {
        frameNumber = 0;
    }
}

// Make a pose
void nextPose() {
    uint32_t time = (uint32_t)bufferTime * 100;
    if (bufferTime > 0) {
        String str = "";
        for (int i = 0; i < MAXSN; i ++) {
            int32_t degree;
            degree = (((int32_t)((uint32_t)bufferAngle[i])) + trim[i]) * 2000 / 180 + 500;
            str += pNumber[i];
            str += degree;
        }
        str += " T";
        str += time;
        Serial1.println(str);
    }
    startTime = millis();
    endTime = startTime + time;
}

void headTurnLeft() {
    bufferAngle[1] = 120;
    bufferTime = 5;
    if (motions[motionNumber].repeat) {
        headAngle = bufferAngle[0];
        neckAngle = bufferAngle[1];
        headNeckTime = bufferTime;
        motionNumber = 0;
        frameNumber = 0;
        theMotion = 0xFF;
        mode = 'M';
        theMode = 'P';
    } else {
        mode = 'P';
    }
}

void headTurnRight() {
    bufferAngle[1] = 60;
    bufferTime = 5;
    if (motions[motionNumber].repeat) {
        headAngle = bufferAngle[0];
        neckAngle = bufferAngle[1];
        headNeckTime = bufferTime;
        motionNumber = 0;
        frameNumber = 0;
        theMotion = 0xFF;
        mode = 'M';
        theMode = 'P';
    } else {
        mode = 'P';
    }
}

void headTurnForward() {
    bufferAngle[0] = 90;
    bufferAngle[1] = 90;
    bufferTime = 5;
    if (motions[motionNumber].repeat) {
        headAngle = bufferAngle[0];
        neckAngle = bufferAngle[1];
        headNeckTime = bufferTime;
        motionNumber = 0;
        frameNumber = 0;
        theMotion = 0xFF;
        mode = 'M';
        theMode = 'P';
    } else {
        mode = 'P';
    }
}

void headLiftUp() {
    bufferAngle[0] = 120;
    bufferTime = 5;
    if (motions[motionNumber].repeat) {
        headAngle = bufferAngle[0];
        neckAngle = bufferAngle[1];
        headNeckTime = bufferTime;
        motionNumber = 0;
        frameNumber = 0;
        theMotion = 0xFF;
        mode = 'M';
        theMode = 'P';
    } else {
        mode = 'P';
    }
}

void headDown() {
    bufferAngle[0] = 60;
    bufferTime = 5;
    if (motions[motionNumber].repeat) {
        headAngle = bufferAngle[0];
        neckAngle = bufferAngle[1];
        headNeckTime = bufferTime;
        motionNumber = 0;
        frameNumber = 0;
        theMotion = 0xFF;
        mode = 'M';
        theMode = 'P';
    } else {
        mode = 'P';
    }
}

void petdogAction(uint8_t action) {
    if (MAXMN > action) {
        // test frame to first
        if (motionNumber != action) {
            if (motionNumber != 0) {
                motionNumber = 0;
                theMotion = action;
            } else {
                motionNumber = action;
                theMotion = 0xFF;
            }
            frameNumber = 0;
        }
        mode = 'M';
        _dbgprint("#M");
        _dbgprintln(motionNumber);
    }
}

void petdog(const char *str) {
    // message struct -- "#command%"
    if (!str) {
        return;
    }
    if (!strcmp_P(str, headLeftCommand)) {
        headTurnLeft();
    } else if (!strcmp_P(str, headRightCommand)) {
        headTurnRight();
    } else if (!strcmp_P(str, headForwardCommand)) {
        headTurnForward();
    } else if (!strcmp_P(str, headUpCommand)) {
        headLiftUp();
    } else if (!strcmp_P(str, headDownCommand)) {
        headDown();
    } else if (!strcmp_P(str, stopCommand)) {
        petdogAction(0);
    } else if (!strcmp_P(str, forwardCommand)) {
        petdogAction(1);
    } else if (!strcmp_P(str, backCommand)) {
        petdogAction(2);        
    } else if (!strcmp_P(str, leftCommand)) {
        petdogAction(3);
    } else if (!strcmp_P(str, rightCommand)) {
        petdogAction(4);
    } else if (!strcmp_P(str, handRightCommand)) {
        petdogAction(6);
    } else if (!strcmp_P(str, sitdownCommand)) {        
        petdogAction(7);
    } else if (!strcmp_P(str, handLeftCommand)) {
        petdogAction(8);
    } else if (!strcmp_P(str, goproneCommand)) {
        petdogAction(9);
    }
}

