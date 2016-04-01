/*

  EmotionAPI.h - Library for Microsoft Project Oxford Emotion API.
  Copyright (c) 2015-2016 ARESPNO.
  
*/

#ifndef _EMOTIONAPI_H
#define _EMOTIONAPI_H

#include <functional>
#include "Arduino.h"

#define EMOTIONAPI_CALLBACK(callback) std::function<void (uint8_t *&, size_t &)> callback
#define PARSE_CALLBACK2(callback) std::function<void (const char*, int)> callback

class EmotionAPI {
	
public:
	EmotionAPI();
	virtual ~EmotionAPI();
	
	void begin(const char *subscription_key);

	boolean recognizeEmotion(size_t length, const char *url = NULL, const char *mode = NULL);
	                                           // Recognize emotion.
	void onData(EMOTIONAPI_CALLBACK(fn));      // Get more image data to send.
	void onParse(PARSE_CALLBACK2(fn));         // Parse the returned string of translation.


	uint8_t getError() { return _err; }        // What error occurred while emotion recognizing.

private:
	const char *_host_face_api;            // The host of Project Oxford AI.
	const char *_subscription_key;         // The Subscription key for Emotion API.
	uint8_t _err;                          // What's happend.
	char *_json_str;                       // The returned string of emotion recognition.

	EMOTIONAPI_CALLBACK(_data_callback);   // The callback for image data.
	PARSE_CALLBACK2(_parse_callback);      // The callback for parse
	
	boolean _finHttpRequest(int code);     // Get the httprequest response and return status.

};

extern EmotionAPI EAMSPO;

#endif  // _EMOTIONAPI_H
