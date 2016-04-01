/*
 * Response for language understanding intelligent service
 */

#define ACTIVITYTYPE  0
#define WAYTYPE  1
#define PARTSTYPE  2
#define SIGNTYPE  3
#define NUMBER  4

int findString_P(const char **src, const char *dst) {
    if (src == NULL || dst == NULL) {
        return -1;
    }
    int index = 0;
    while (strcmp_P(src[index], "")) {
        if (!strcmp_P(dst, src[index])) {
            return index;
        }
        index ++;
    }
    return -1;
}

void startActivity(const char *activity, const char *way, const char *parts) {
    _dbgprintln("Capture StartActivity");
    static const char *activity_en_US[] PROGMEM = {
        "walk", "go", "turn", "lift", "raise", "sit", "stand", "look", "",
    };
    static const char *activity_zh_CN[] PROGMEM = {
        "走", "退", "转", "抬", "举", "坐", "趴", "站", "低", "看", "",
    };
    static const char *way_en_US[] PROGMEM = {
        "forward", "back", "left", "right", "up", "down", "",
    };
    static const char *way_zh_CN[] PROGMEM = {
        "前", "后", "左", "右", "上", "下", "", 
    };
    static const char *parts_en_US[] PROGMEM = {
        "hand", "foot", "head", "",
    };
    static const char *parts_zh_CN[] PROGMEM = {
        "手", "脚", "头", "",
    };
    const char **_activity = culture == CULTURE_EN_US ? activity_en_US : activity_zh_CN;
    const char **_way = culture == CULTURE_EN_US ? way_en_US : way_zh_CN;
    const char **_parts = culture == CULTURE_EN_US ? parts_en_US : parts_zh_CN;

    int activityIndex = findString_P(_activity, activity);
    if (activityIndex == -1) {
        return;
    }
    if (culture == CULTURE_EN_US) {
        // I am bad at english, not implemented yet.
    } else {
        switch (activityIndex) {
            case 0:
                _dbgprintln("Capture walk forward.");
                petdogAction(FORWARDACTION);
                break;

            case 1:
                _dbgprintln("Capture move back.");
                petdogAction(BACKACTION);
                break;

            case 2:
                {
                    int wayIndex = findString_P(_way, way);
                    if (wayIndex == 0 || wayIndex == 2 || wayIndex == 3) {
                        int partsIndex = findString_P(_parts, parts);
                        if (partsIndex != 2) {
                            _dbgprint("Capture turn ");
                            _dbgprintln(way);
                            if (wayIndex == 2) {
                                petdogAction(LEFTACTION);
                            } else if (wayIndex == 3) {
                                petdogAction(RIGHTACTION);
                            }
                        } else {
                            _dbgprint("Capture turn head to the ");
                            _dbgprintln(way);
                            if (wayIndex == 0) {
                                headTurnForward();
                            } else if (wayIndex == 2) {
                                headTurnLeft();
                            } else if (wayIndex == 3) {
                                headTurnRight();
                            }
                        }
                    }
                }
                break;

            case 3:
                {
                    int partsIndex = findString_P(_parts, parts);
                    if (partsIndex == 1 || partsIndex == 2) {
                        _dbgprint("Capture lift ");
                        _dbgprint(parts);
                        _dbgprintln(" up");
                        if (partsIndex == 2) {
                            headLiftUp();
                        }
                    }
                }
                break;
                
            case 4:
                {
                    int partsIndex = findString_P(_parts, parts);
                    if (partsIndex == 0) {
                        int wayIndex = findString_P(_way, way);
                        if (wayIndex == 2 || wayIndex == 3) {
                            _dbgprint("Capture raise ");
                            _dbgprint(parts);
                            _dbgprintln(" up");
                            if (wayIndex == 2) {
                                petdogAction(LEFTHANDACTION);
                            } else if (wayIndex == 3) {
                                petdogAction(RIGHTHANDACTION);
                            }
                        }
                    }
                }
                break;

            case 5:
                _dbgprintln("Capture sit down.");
                petdogAction(SITDOWNACTION);
                break;

            case 6:
                _dbgprintln("Capture lie down.");
                petdogAction(GOPRONEACTION);
                break;

            case 7:
                _dbgprintln("Capture stand up.");
                petdogAction(STOPACTION);
                break;

            case 8:
                {
                    int partsIndex = findString_P(_parts, parts);
                    if (partsIndex == 2) {
                        _dbgprint("Capture ");
                        _dbgprint(parts);
                        _dbgprintln(" down.");
                        headDown();
                    }
                }
                break;
                
            case 9:
                {
                    int wayIndex = findString_P(_way, way);
                    if (wayIndex != 1 && wayIndex != -1) {
                        _dbgprint("Capture look ");
                        _dbgprintln(way);
                        if (wayIndex == 0) {
                            headTurnForward();
                        } else if (wayIndex == 2) {
                            headTurnLeft();
                        } else if (wayIndex == 3) {
                            headTurnRight();
                        } else if (wayIndex == 4) {
                            headLiftUp();
                        } else if (wayIndex == 5) {
                            headDown();
                        }
                    }
                }
                break;
                
            default:
                break;
                
        }
        return;
    }
}

void stopActivity() {
    _dbgprintln("Capture StopActivity");
    petdogAction(STOPACTION);
}

uint32_t calculate(const char **num, const char **sign) {
    _dbgprintln("Capture Calculate");
    static const char *num_en_US[] PROGMEM = {
        "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "hundred", "thousand", "million", "",
    };
    static const char *num_zh_CN[] PROGMEM = {
        "零", "一", "二", "三", "四", "五", "六", "七", "八", "九", "十", "百", "千", "万", "亿", "",
    };
    static const char *sign_en_US[] PROGMEM = {
        "+", "-", "x", "/", "plus", "minus", "multiplied", "divided", "",
    };
    static const char *sign_zh_CN[] PROGMEM = {
        "+", "-", "x", "/", "加", "减", "乘", "除", "",
    };
    const char **_num = culture == CULTURE_EN_US ? num_en_US : num_zh_CN;
    const char **_sign = culture == CULTURE_EN_US ? sign_en_US : sign_zh_CN;

    if (culture == CULTURE_EN_US) {
        // I am bad at english, not implemented yet.
        return 0;
    } else {
        size_t i = 0, j = 0;
        uint32_t val1 = 0, val2 = 0;
        while (1) {
            if (num[i] == NULL || sign[j] == NULL) {
                break;
            }
            char ptr[4];
            ptr[0] = num[i][0];
            ptr[1] = num[i][1];
            ptr[2] = num[i][2];
            ptr[3] = '\0';
            if (findString_P(_num, ptr) == -1) {
                if (!i) {
                    val1 = atoi(num[i]);
                } else {
                    val2 = atoi(num[i]);
                }
            } else {
                uint32_t val = 0;
                uint32_t lastVal = 0;
                uint32_t currentVal = 0;
                int lastNumIndex = 13;
                size_t k = 0;
                while (1) {
                    ptr[0] = num[i][k ++];
                    ptr[1] = num[i][k ++];
                    ptr[2] = num[i][k ++];
                    ptr[3] = '\0';
                    if (ptr[0] == '\0' || ptr[1] == '\0' || ptr[2] == '\0') {
                        break;
                    }
                    int numIndex = findString_P(_num, ptr);
                    if (numIndex == -1) {
                        break;
                    }
                    if (numIndex > 0 && numIndex < 10) {
                        lastVal = numIndex;
                    } else if (numIndex != 0) {
                        if (numIndex > lastNumIndex) {
                            lastVal += currentVal;
                            currentVal = 0;
                        }
                        switch (numIndex) {
                            case 14:
                                lastVal *= 10000;
                            case 13:
                                lastVal *= 10;
                            case 12:
                                lastVal *= 10;
                            case 11:
                                lastVal *= 10;
                            case 10:
                                lastVal *= 10;
                                break;
                                
                        }
                        if (numIndex > lastNumIndex) {
                            val += lastVal;
                        } else {
                            currentVal += lastVal;
                        }
                        lastVal = 0;
                        lastNumIndex = numIndex;
                    }
                }
                val += currentVal + lastVal;
                if (!i) {
                    val1 = val;
                } else {
                    val2 = val;
                }
            }
            if (!i) {
                _dbgprint(val1);
                i ++;
            } else {
                int signIndex = findString_P(_sign, sign[j]);
                if (signIndex != -1) {
                    if (signIndex > 3) {
                        signIndex -= 4;
                    }
                    _dbgprint(_sign[signIndex]);
                    _dbgprint(val2);
                    switch (signIndex) {
                        case 0:
                            val1 += val2;
                            break;
                        
                        case 1:
                            val1 -= val2;
                            break;
                            
                        case 2:
                            val1 *= val2;
                            break;
                            
                        case 3:
                            if (val2 != 0) {
                                val1 /= val2;
                            }
                            break;
                            
                    }
                    i ++;
                }
                j ++;
            }
        }
        _dbgprint("=");
#if defined(OLED)
        display.println(val1);
        display.display();
#else
        Serial.println(val1);
#endif
        return val1;
    }
}

