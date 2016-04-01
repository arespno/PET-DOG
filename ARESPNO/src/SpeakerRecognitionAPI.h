/*

  SpeakerRecognitionAPI.h - Library for Microsoft Project Oxford Speaker Recognition API.
  Copyright (c) 2015-2016 ARESPNO.
  
*/

#ifndef _SPEAKERRECOGNITIONAPI_H
#define _SPEAKERRECOGNITIONAPI_H

#include <functional>
#include "Arduino.h"

#define LOCALE_EN_US  0
#define LOCALE_ZH_CN  1
#define LOCALE_END  2

#define SPEAKER_CALLBACK(callback) std::function<void (uint8_t *&, size_t &)> callback
#define PARSE_CALLBACK2(callback) std::function<void (const char*, int)> callback

class SpeakerRecognitionAPI {

public:
	SpeakerRecognitionAPI();
	virtual ~SpeakerRecognitionAPI();
	
	void begin(const char *subscription_key);
	
	boolean createProfile(char *guid, boolean identify, uint8_t lang = LOCALE_EN_US);
	                                           // Create profile for Identification or Verification.
	boolean getProfile(const char *guid, boolean identify);
	                                           // Get profile of Identification or Verification.
	boolean deleteProfile(const char *guid, boolean identify);
	                                           // Delete profile for Identification or Verification.
	boolean resetEnrollments(const char *guid, boolean identify);
	                                           // Reset enrollment of Identification or Verification.
	boolean createEnrollment(const char *guid, boolean identify, size_t length);
	                                           // Create enrollment for Identification or Verification.
	boolean getOperationStatus(const char *operationId);
	                                           // Get operation status or result for 
											   // Speaker Recognition - Identification or Identification Profile - Create Enrollment. 
	boolean identification(const char *guids, size_t length);
	                                           // To automatically identify who is speaking given a group of speakers,
	                                           // guids is comma-delimited, and at most 10 guids.
	boolean listAllPhrases(uint8_t lang = LOCALE_EN_US);
	                                           // Returns the list of supported verification phrases that can be used for 
											   // Verification Profile - Create Enrollment and Speaker Recognition - Verification.
	boolean verification(const char *guid, size_t length);
	                                           // To automatically verify and authenticate users using their voice or speech.

	void onData(SPEAKER_CALLBACK(fn));         // Get more sound data to send.
	void onParse(PARSE_CALLBACK2(fn));         // Parse the returned string of translation.


	uint8_t getError() { return _err; }        // What error occurred while speaker recognizing.

private:
	const char *_host_speaker_api;            // The host of Project Oxford AI.
	const char *_subscription_key;         // The Subscription key for Speaker Recognition API.
	uint8_t _err;                          // What's happend.
	char *_json_str;                       // The returned string of Speaker Recognition.

	SPEAKER_CALLBACK(_data_callback);      // The callback for sound data.
	PARSE_CALLBACK2(_parse_callback);      // The callback for parse
	
	boolean _finHttpRequest(int code);     // Get the httprequest response and return status.
	
	void _parse(PARSE_CALLBACK2(fn), int code);
	void _getData(SPEAKER_CALLBACK(fn), uint8_t *&data, size_t &len);

};

extern SpeakerRecognitionAPI SRAMSPO;

#endif  // _SPEAKERRECOGNITIONAPI_H
