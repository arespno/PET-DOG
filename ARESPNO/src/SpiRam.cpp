/*
  SpiRam.cpp -- Spi ram 23LC1024
  Copyright (c) 2015-2016 ARESPNO.
*/

#include <SPI.h>

#include "SpiRam.h"

#define srSPI  SPI2

SpiRam::SpiRam()
:_pos(0) 
,_size(0)
,_cspin(-1) {
}

void SpiRam::begin(int cspin) {
	_cspin = cspin;
	pinMode(_cspin, OUTPUT);
	digitalWrite(_cspin, HIGH);
	srSPI.begin();
}

void SpiRam::open(uint8_t mode) {
	_pos = 0;
	if (mode == SPIRAM_OVERRIDE) {
		_size = 0;
	}
}

boolean SpiRam::seek(uint32_t pos) {
	if (pos > _size) {
		return false;
	}
	_pos = pos;
	return true;
}

uint32_t SpiRam::position() {
	return _pos;
}

uint32_t SpiRam::size() {
	return _size;
}

uint32_t SpiRam::available() {
	if (_size < _pos) {
		return 0;
	}
	return _size - _pos;
}

uint32_t SpiRam::read(uint8_t *buf, uint32_t len) {
	if (_cspin == -1 || !buf || !len) {
		return 0;
	}
    uint32_t _len = _pos + len > _size ? _size - _pos : len;
    if (_len == 0) {
        return 0;
    }
    digitalWrite(_cspin, LOW);
    srSPI.transfer(SPIRAM_READ);
    srSPI.transfer((_pos >> 0x10) & 0xff);
    srSPI.transfer((_pos >> 0x08) & 0xff);
    srSPI.transfer(_pos & 0xff);
    for (uint32_t i = 0; i < _len; i ++) {
        buf[i] = srSPI.transfer(0x00);
    }
    digitalWrite(_cspin, HIGH);
    _pos += _len;
    return _len;
}

uint32_t SpiRam::write(uint8_t *buf, uint32_t len) {
	if (_cspin == -1 || !buf || !len) {
		return 0;
	}
    uint32_t _len = _pos + len > SPIRAM_SIZE ? SPIRAM_SIZE - _pos : len;
    if (_len == 0) {
        return 0;
    }
    digitalWrite(_cspin, LOW);
    srSPI.transfer(SPIRAM_WRITE);
    srSPI.transfer((_pos >> 0x10) & 0xff);
    srSPI.transfer((_pos >> 0x08) & 0xff);
    srSPI.transfer(_pos & 0xff);
    for (uint32_t i = 0; i < _len; i ++) {
		srSPI.transfer(buf[i]);
    }
    digitalWrite(_cspin, HIGH);
    _pos += _len;
	if (_size < _pos) {
		_size = _pos;
	}
    return _len;
}

uint32_t SpiRam::write(const char *buf) {
	return write((uint8_t *) buf, strlen(buf));
}

uint32_t SpiRam::write(uint8_t value) {
	return write(&value, 1);
}

uint32_t SpiRam::write(String string) {
	return write(string.c_str());
}

SpiRam SPIRAM;
