// sharp ir distance and analog value table
int distanceMap[] = {
    580, 330, 220, 170, 135, 115, 108, 100, 93, 86, 
};

#define SHARP_IR_PIN  1

int getDistance() {
    int irSignal = 0;
    for (uint8_t i = 0; i < 5; i ++) {
        irSignal += analogRead(SHARP_IR_PIN);
        delay(2);
    }
    irSignal /= 5;
    int distance = 200;
    for (int i = 0; i < 10; i ++) {
        if (irSignal >= distanceMap[i]) {
            distance = (i + 1) * 10;
            if (distance == 90) {
                distance = 100;
            } else if (distance == 70) {
                distance = 80;
            } else if (distance == 50) {
                distance = 60;
            }
            break;
        }
    }
    return distance;
}

boolean isCloseTo(int distance) {
    static uint8_t counts = 0;
    if (getDistance() <= distance) {
        counts ++;
        if (counts >= 20) {
            counts = 0;
            return true;
        }
    } else {
        counts = 0;
    }
    return false;
}

boolean isFarAway(int distance) {
    static uint8_t counts = 0;
    if (getDistance() > distance) {
        counts ++;
        if (counts >= 20) {
            counts = 0;
            return true;
        }
    } else {
        counts = 0;
    }
    return false;
}

