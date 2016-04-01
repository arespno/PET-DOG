/*
  SpiRam.h -- Library for spi ram 23LC1024
  Copyright (c) 2015-2016 ARESPNO.
*/

#ifndef _SPIRAM_H
#define _SPIRAM_H

#include "Arduino.h"

#define SPIRAM_READ  3
#define SPIRAM_WRITE  2
#define SPIRAM_SIZE  0x20000

#define SPIRAM_READONLY  0
#define SPIRAM_OVERRIDE  1

class SpiRam {
	
public:
	SpiRam();
	
	void begin(int cspin);                       // Set spi ram CS pin.
	void open(uint8_t mode = SPIRAM_READONLY);   // Open spi ram for read or write, and what is similar SD library.
	boolean seek(uint32_t pos);
	uint32_t position();
	uint32_t size();
	uint32_t available();
	uint32_t read(uint8_t *buf, uint32_t len);
	uint32_t write(uint8_t *buf, uint32_t len);
	uint32_t write(const char *buf);
	uint32_t write(uint8_t value);
	uint32_t write(String string);
	
private:
	int _cspin;                        // cs pin
	uint32_t _pos;                     // current position
	uint32_t _size;                    // current size
	
};

extern SpiRam SPIRAM;

#endif  // _SPIRAM_H
