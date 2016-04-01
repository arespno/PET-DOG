/*
  SpeechAPI.h - Library for Microsoft Project Oxford Speech API.
  Copyright (c) 2015-2016 ARESPNO.
*/

#ifndef _SPEECH_API
#define _SPEECH_API

#include <functional>
#include "Arduino.h"

#define SPEECHAPI_ar_EG  0
#define SPEECHAPI_ca_ES  1
#define SPEECHAPI_da_DK  2
#define SPEECHAPI_de_DE  3
#define SPEECHAPI_en_AU  4
#define SPEECHAPI_en_CA  5
#define SPEECHAPI_en_GB  6
#define SPEECHAPI_en_IN  7
#define SPEECHAPI_en_NZ  8
#define SPEECHAPI_en_US  9
#define SPEECHAPI_es_ES  10
#define SPEECHAPI_es_MX  11
#define SPEECHAPI_fi_FI  12
#define SPEECHAPI_fr_CA  13
#define SPEECHAPI_fr_FR  14
#define SPEECHAPI_it_IT  15
#define SPEECHAPI_ja_JP  16
#define SPEECHAPI_ko_KR  17
#define SPEECHAPI_nb_NO  18
#define SPEECHAPI_nl_NL  19
#define SPEECHAPI_pl_PL  20
#define SPEECHAPI_pt_BR  21
#define SPEECHAPI_pt_PT  22
#define SPEECHAPI_ru_RU  23
#define SPEECHAPI_sv_SE  24
#define SPEECHAPI_zh_CN  25
#define SPEECHAPI_zh_HK  26
#define SPEECHAPI_zh_TW  27

#define RECOGNIZE_CALLBACK(callback) std::function<void (uint8_t *&, size_t&)> callback
#define SYNTHESIZE_CALLBACK(callback) std::function<void (uint8_t *, size_t)> callback
#define PARSE_CALLBACK2(callback) std::function<void (const char*, int)> callback

class SpeechAPI {

public:
	SpeechAPI();
	virtual ~SpeechAPI();
	
	void begin(const char *app_id, const char *client_id, const char *subscription_key);
	
	boolean acquireAccessToken();              // Acquire Access Token with Client ID and Subscription Key.
	const char *getAccessToken() { return _access_token; }
	                                           // Return the Access Token.
	void setAccessToken(const char *access_token);
	                                           // Store the Access Token.
	boolean recognize(size_t length, size_t rate, uint8_t lang = SPEECHAPI_en_US);
	                                           // Speech to server, return text.
	boolean synthesize(const char *text, size_t rate, boolean raw = false, uint8_t lang = SPEECHAPI_en_US, boolean female = true);
	                                           // Send text to server, return voice.
	boolean LUIS(const char *luis_appid, const char *luis_subscriptionkey, const char *text);
	                                           // Send text to LUIS server.
	void onDataRecognize(RECOGNIZE_CALLBACK(fn));
                                               // Get more voice data to send.
	void onDataSynthesize(SYNTHESIZE_CALLBACK(fn));
                                               // Play more voice data.
	void onParseRecognize(PARSE_CALLBACK2(fn));
	void onParseSynthesize(PARSE_CALLBACK2(fn));
	void onParseLUIS(PARSE_CALLBACK2(fn));
	                                           // Parse the returned json string.

	uint8_t getError() { return _err; }        // What error occurred.

private:
	char *_access_token;                   // The Access Token for Speech API.
	const char *_host_access_token;        // The host of get Access Token.
	const char *_host_voice_recognition;   // The host of Microsoft Bing Voice.
	const char *_app_id;                   // The App ID.
	const char *_client_id;                // The Client ID.
	const char *_subscription_key;         // The Subscription Key.
	uint8_t _err;                          // What's happend.
	char *_json_str;                       // The returned string of Speech API.
	unsigned long _expires_in;             // The expires in time.

	RECOGNIZE_CALLBACK(_data_recognize_callback);
	                                       // The callback for get voice data.
	SYNTHESIZE_CALLBACK(_data_synthesize_callback);
	                                       // The callback for play voice data.
	PARSE_CALLBACK2(_parse_recognize_callback);
	PARSE_CALLBACK2(_parse_synthesize_callback);
	PARSE_CALLBACK2(_parse_LUIS_callback);
	                                       // The callback for parse
										   
	void _parse(PARSE_CALLBACK2(fn), int code);
};

extern SpeechAPI SAMSPO;

#endif  // _SPEECH_API