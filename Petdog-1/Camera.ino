ArduCAM myCAM(OV2640, CAMERA_CS);

// image data block counts
size_t imageDataCounts;

void initCamera() {
    Wire.begin();

    pinMode (CAMERA_CS, OUTPUT);
    SPI.begin();
    // set SPI clock to FPU / 6 = 8000000
    SPI.setClockDivider(6);

    // Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    if (myCAM.read_reg(ARDUCHIP_TEST1) != 0x55) {
        _dbgprintln("SPI interface Error!");
    }

#if defined(LOW_POWER)
    myCAM.set_bit(ARDUCHIP_GPIO, GPIO_PWDN_MASK);
#endif

    uint8_t vid, pid;
    // Check if the camera module type is OV2640
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26) || (pid != 0x42)) {
        _dbgprintln("Can't find OV2640 module!");
    } else {
        _dbgprintln("OV2640 detected.");
    }

    // Change to JPEG capture mode and initialize the OV2640 module
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.OV2640_set_JPEG_size(OV2640_640x480);
    myCAM.clear_fifo_flag();
    myCAM.write_reg(ARDUCHIP_FRAMES, 0x00);
    
#if defined(LOW_POWER)
    myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
#endif
}

void start_capture() {
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    myCAM.start_capture();
}

void doCapture() {
    start_capture();
    uint8_t delays = 0;
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK) && delays < 20) {
        delay(10);
        delays ++;
    }
    if (delays >= 20) {
        _dbgprintln("Capture error...");
        return;
    }
    faceDetect(myCAM);
    emotionRecognize(myCAM);
    myCAM.clear_fifo_flag();
}

uint32_t _fifoBurstMode(ArduCAM myCAM) {
    uint32_t len = 0;

    len = myCAM.read_fifo_length();
    // the FIFO memory is 384kb
    if (len >= 393216 ) {
        _dbgprintln("Over size.");
        return 0;
    }
    if (len == 0 ) {
        _dbgprintln("Size is 0.");
        return 0;
    }
    myCAM.write_reg(ARDUCHIP_FIFO, FIFO_RDPTR_RST_MASK);
    myCAM.CS_LOW();
    // set fifo burst mode
    myCAM.set_fifo_burst();
    // skip first byte
    SPI.transfer(0x00);
    imageDataCounts = len / HTTP_SEND_SIZE;
    if (len % HTTP_SEND_SIZE) {
        imageDataCounts ++;
    }
    return len;
}

void faceDetect(ArduCAM myCAM) {
    uint32_t len = _fifoBurstMode(myCAM);
    if (len) {
        startFaceDetect(len);
        myCAM.CS_HIGH();
    }
}

void emotionRecognize(ArduCAM myCAM) {
    uint32_t len = _fifoBurstMode(myCAM);
    if (len) {
        startEmotionRecognize(len);
        myCAM.CS_HIGH();
    }
}

