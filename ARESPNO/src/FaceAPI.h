/*

  FaceAPI.h - Library for Microsoft Project Oxford Face API.
  Copyright (c) 2015-2016 ARESPNO.
  
*/

#ifndef _FACEAPI_H
#define _FACEAPI_H

#include <functional>
#include "Arduino.h"

#define RETURN_FACEID  0x01
#define RETURN_FACELANDMARKS  0x02
#define RETURN_AGE  0x04
#define RETURN_GENDER  0x08
#define RETURN_SMILE  0x10
#define RETURN_FACIALHAIR  0x20
#define RETURN_HEADPOSE  0x40
#define RETURN_FACEATTRIBUTES  0x7C
#define RETURN_FACE_ALL  0x7F

#define FACEAPI_CALLBACK(callback) std::function<void (uint8_t *&, size_t &)> callback
#define PARSE_CALLBACK2(callback) std::function<void (const char*, int)> callback

class FaceAPI {
	
public:
	FaceAPI();
	virtual ~FaceAPI();
	
	void begin(const char *subscription_key);

	boolean detectFace(size_t length, const char *url = NULL, uint8_t mode = RETURN_FACEID | RETURN_AGE | RETURN_GENDER | RETURN_SMILE);
	                                           // Detect face.
	void onData(FACEAPI_CALLBACK(fn));         // Get more image data to send.
	void onParse(PARSE_CALLBACK2(fn));         // Parse the returned string of translation.


	uint8_t getError() { return _err; }        // What error occurred while face recognizing.

private:
	const char *_host_face_api;            // The host of Project Oxford AI.
	const char *_subscription_key;         // The Subscription key for Face API.
	uint8_t _err;                          // What's happend.
	char *_json_str;                       // The returned string of face recognition.

	FACEAPI_CALLBACK(_data_callback);      // The callback for image data.
	PARSE_CALLBACK2(_parse_callback);      // The callback for parse
	
	boolean _finHttpRequest(int code);     // Get the httprequest response and return status.

};

extern FaceAPI FAMSPO;

#endif  // _FACEAPI_H
