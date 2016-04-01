/*
  VS1053.h -- Library for vs1053 sound chip
  Copyright (c) 2015-2016 ARESPNO.
*/

#ifndef _VS1053_H
#define _VS1053_H

#include "Arduino.h"

#define VS_WRITE_COMMAND    0x02
#define VS_READ_COMMAND     0x03

//VS1053 register
#define SCI_MODE            0x00
#define SCI_STATUS          0x01
#define SCI_BASS            0x02
#define SCI_CLOCKF          0x03
#define SCI_DECODE_TIME     0x04
#define SCI_AUDATA          0x05
#define SCI_WRAM            0x06
#define SCI_WRAMADDR        0x07
#define SCI_HDAT0           0x08
#define SCI_HDAT1           0x09
#define SCI_AIADDR          0x0A
#define SCI_VOL             0x0B
#define SCI_AICTRL0         0x0C
#define SCI_AICTRL1         0x0D
#define SCI_AICTRL2         0x0E  
#define SCI_AICTRL3         0x0F

#define SM_DIFF             0x01
#define SM_LAYER12          0x02
#define SM_RESET            0x04
#define SM_CANCEL           0x08
#define SM_EARSPEAKER_LO    0x10
#define SM_TESTS            0x20
#define SM_STREAM           0x40
#define SM_EARSPEAKER_HI    0x80
#define SM_DACT             0x100
#define SM_SDIORD           0x200
#define SM_SDISHARE         0x400
#define SM_SDINEW           0x800
#define SM_ADPCM            0x1000
#define SM_NONE             0x2000
#define SM_LINE1            0x4000
#define SM_CLK_RANGE        0x8000

class VS1053 {

public:
	VS1053();
	
	void begin(int xcs, int xdcs, int xreset, int dreq);
	                                             // Set vs1053 xcs, xdcs, xreset, dreq pin.
	uint16_t sciRead(uint8_t reg);               // read from register
	void sciWrite(uint8_t reg, uint16_t val);    // write to register
	uint16_t sdiWrite(uint8_t *buf, uint16_t len);
	                                             // write data
	
	void setVolume(uint16_t vol) { _vol = vol; sciWrite(SCI_VOL, _vol); }
	void loadPatch(const uint16_t *buf, uint16_t len);
	void softReset();
	void initRecord(uint16_t samplerate = 8000); // Initialize register for recording.
	void finishRecord();                         // Restore register when finished recording.
	
private:
	uint8_t _waitDreq();               // wait for dreq raised

	int _xcs;                          // xcs pin
	int _xdcs;                         // xcs pin
	int _xreset;                       // xcs pin
	int _dreq;                         // dreq pin
	uint16_t _vol;                     // volume, maximum is 0, minimum is 0xfefe

};

extern VS1053 VSSound;

#endif  // _VS1053_H
