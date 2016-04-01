/*
  VS1053.cpp -- vs1053 sound chip
  Copyright (c) 2015-2016 ARESPNO.
*/

#include <SPI.h>

#include "VS1053.h"

#define vsSPI  SPI2

VS1053::VS1053()
:_xcs(-1)
,_xdcs(-1)
,_xreset(-1)
,_dreq(-1)
,_vol(0x3030) {
}

uint8_t VS1053::_waitDreq() {
	unsigned long t = millis() + 1000;
    while (!digitalRead(_dreq) && millis() < t);
	if (digitalRead(_dreq)) {
		return 1;
	} else {
		return 0;
	}
}

void VS1053::begin(int xcs, int xdcs, int xreset, int dreq) {
	_xcs = xcs;
	_xdcs = xdcs;
	_xreset = xreset;
	_dreq = dreq;
	pinMode(_xcs, OUTPUT);
	pinMode(_xdcs, OUTPUT);
	pinMode(_xreset, OUTPUT);
	pinMode(_dreq, INPUT);

	vsSPI.begin();
	digitalWrite(_xreset, LOW);
	digitalWrite(_xcs, HIGH);
	digitalWrite(_xdcs, HIGH);
	delay(100);
    vsSPI.setClockDivider(SPI_CLOCK_DIV64);
    digitalWrite(_xreset, HIGH);

    sciWrite(SCI_MODE, SM_SDINEW | SM_RESET);
	delay(1);
    sciWrite(SCI_CLOCKF, 0xb800);
    sciWrite(SCI_BASS, 0x0000);
    sciWrite(SCI_DECODE_TIME, 0x0000);
    sciWrite(SCI_VOL, _vol);
    _waitDreq();

    vsSPI.setClockDivider(SPI_CLOCK_DIV4);
    digitalWrite(_xreset, HIGH);
    digitalWrite(_xcs, HIGH);
    digitalWrite(_xdcs, HIGH);
}

uint16_t VS1053::sciRead(uint8_t reg) {
    if (_waitDreq()) {
        digitalWrite(_xcs, LOW);
        vsSPI.transfer(VS_READ_COMMAND);
        vsSPI.transfer(reg);
        uint16_t val = vsSPI.transfer16(0x0000);
        digitalWrite(_xcs, HIGH);
        return val;
    } else {
		return 0;
	}
}

void VS1053::sciWrite(uint8_t reg, uint16_t val) {
    if (_waitDreq()) {
        digitalWrite(_xcs, LOW);
        vsSPI.transfer(VS_WRITE_COMMAND);
        vsSPI.transfer(reg);
        vsSPI.transfer16(val);
        digitalWrite(_xcs, HIGH);
    }
}

uint16_t VS1053::sdiWrite(uint8_t *buf, uint16_t len) {
    if (_waitDreq()) {
        digitalWrite(_xdcs, LOW);
        vsSPI.transfer(buf, len);
        digitalWrite(_xdcs, HIGH);
		return len;
	} else {
		return 0;
	}
}

void VS1053::softReset() {
	uint16_t mode = sciRead(SCI_MODE);
	mode |= SM_RESET;
    sciWrite(SCI_MODE, mode);
}

void VS1053::loadPatch(const uint16_t *buf, uint16_t len) {	
    for (uint16_t i = 0; i < len; ) {
        uint16_t addr = buf[i ++];
        uint16_t c = buf[i ++];
        if (c & 0x8000) {
            c &= 0x7FFF;
            uint16_t val = buf[i ++];
            while (c --) {
				sciWrite(addr, val);
            }
        } else {
            while (c --) {
                sciWrite(addr, buf[i ++]);
            }
        }
    }
}

void VS1053::initRecord(uint16_t samplerate) {
	// Eliminate the recording of whistlers.
    sciWrite(SCI_VOL, 0xfefe);
    sciWrite(SCI_WRAMADDR, 0xc045);
    sciWrite(SCI_WRAM, 0xfefe);
 
    sciWrite(SCI_AICTRL0, samplerate);  // samplerate -- default is 8000
    sciWrite(SCI_AICTRL1, 0x2000);      // GC -- 8x
    sciWrite(SCI_AICTRL2, 0x0000);      // 
    sciWrite(SCI_AICTRL3, 0x0006);      // left channel, PCM
    sciWrite(SCI_MODE, SM_ADPCM | SM_SDINEW | SM_RESET);
	delay(1);

	static const uint16_t wav_plugin[] = {
		0x0007, 0x0001, 0x8010, 0x0006, 0x001c, 0x3e12, 0xb817, 0x3e14,
		0xf812, 0x3e01, 0xb811, 0x0007, 0x9717, 0x0020, 0xffd2, 0x0030,
		0x11d1, 0x3111, 0x8024, 0x3704, 0xc024, 0x3b81, 0x8024, 0x3101,
		0x8024, 0x3b81, 0x8024, 0x3f04, 0xc024, 0x2808, 0x4800, 0x36f1,
		0x9811, 0x0007, 0x0001, 0x8028, 0x0006, 0x0002, 0x2a00, 0x040e,
	};
	loadPatch(wav_plugin, 40);
}

void VS1053::finishRecord() {
    sciWrite(SCI_MODE, SM_SDINEW | SM_RESET);
	delay(1);
    sciWrite(SCI_WRAMADDR, 0xc045);
    sciWrite(SCI_WRAM, _vol);
    sciWrite(SCI_VOL, _vol);
}

VS1053 VSSound;
