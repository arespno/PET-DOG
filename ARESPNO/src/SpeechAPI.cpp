/*
  SpeechAPI.cpp - Microsoft Project Oxford Speech API.
  Copyright (c) 2015-2016 ARESPNO.
*/

#include <ArduinoJson.h>
#include "Code.h"
#include "HttpRequest.h"
#include "SpeechAPI.h"

const char *outputFormat[] PROGMEM = {
	"raw-8khz-8bit-mono-mulaw",
	"raw-16khz-16bit-mono-pcm",
	"riff-8khz-8bit-mono-mulaw",
	"riff-16khz-16bit-mono-pcm",
};

const char *language_country[] PROGMEM = {
	"ar-EG",
	"ca-ES",
	"da-DK",
	"de-DE",
	"en-AU",
	"en-CA",
	"en-GB",
	"en-IN",
	"en-NZ",
	"en-US",
	"es-ES",
	"es-MX",
	"fi-FI",
	"fr-CA",
	"fr-FR",
	"it-IT",
	"ja-JP",
	"ko-KR",
	"nb-NO",
	"nl-NL",
	"pl-PL",
	"pt-BR",
	"pt-PT",
	"ru-RU",
	"sv-SE",
	"zh-CN",
	"zh-HK",
	"zh-TW",
};

const char *service_name_mapping[] PROGMEM = {
	"", 
	"Hedda", 
	"Stefan, Apollo", 
	"Catherine", 
	"Linda", 
	"Susan, Apollo", 
	"George, Apollo", 
	"Ravi, Apollo", 
	"ZiraRUS", 
	"BenjaminRUS", 
	"Laura, Apollo", 
	"Pablo, Apollo", 
	"Raul, Apollo", 
	"Caroline", 
	"Julie, Apollo", 
	"Paul, Apollo", 
	"Cosimo, Apollo", 
	"Ayumi, Apollo", 
	"Ichiro, Apollo", 
	"Daniel, Apollo", 
	"Daniel, Apollo", 
	"Pavel, Apollo", 
	"Yaoyao, Apollo", 
	"Kangkang, Apollo", 
	"Tracy, Apollo", 
	"Danny, Apollo", 
	"Yating, Apollo", 
	"Zhiwei, Apollo", 
};

const uint8_t mapping_index[][2] = {
	{  0,  0 }, 
	{  0,  0 }, 
	{  0,  0 }, 
	{  1,  2 }, 
	{  3,  0 }, 
	{  4,  0 }, 
	{  5,  6 }, 
	{  0,  7 }, 
	{  0,  0 }, 
	{  8,  9 }, 
	{ 10, 11 }, 
	{  0, 12 }, 
	{  0,  0 }, 
	{ 13,  0 }, 
	{ 14, 15 }, 
	{  0, 16 }, 
	{ 17, 18 }, 
	{  0,  0 }, 
	{  0,  0 }, 
	{  0,  0 }, 
	{  0,  0 }, 
	{  0, 19 }, 
	{  0,  0 }, 
	{ 20, 21 }, 
	{  0,  0 }, 
	{ 22, 23 }, 
	{ 24, 25 }, 
	{ 26, 27 }, 
};

SpeechAPI::SpeechAPI()
:_access_token(NULL)
,_app_id(NULL)
,_client_id(NULL)
,_subscription_key(NULL)
,_host_access_token("oxford-speech.cloudapp.net")
,_host_voice_recognition("speech.platform.bing.com")
,_json_str(NULL)
,_data_recognize_callback(NULL)
,_data_synthesize_callback(NULL)
,_parse_recognize_callback(NULL)
,_parse_synthesize_callback(NULL)
,_parse_LUIS_callback(NULL)
,_expires_in(0) {
}

SpeechAPI::~SpeechAPI() {
	if (_json_str != NULL) {
		free(_json_str);
		_json_str = NULL;
	}
	if (_access_token != NULL) {
		free(_access_token);
		_access_token = NULL;
	}
}

void SpeechAPI::begin(const char *app_id, const char *client_id, const char *subscription_key) {
	_app_id = app_id;
	_client_id = client_id;
	_subscription_key = subscription_key;
}

boolean SpeechAPI::acquireAccessToken() {
	if (_client_id == NULL || _subscription_key == NULL) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}

	HttpRequest http(_host_access_token);
	http.setMethod("POST");
    // We now create a URI for the request
	http.setURI("/token/issueToken");
	http.setProperty("Content-Type", "application/x-www-form-urlencoded");
    String request = "?grant_type=client_credentials";
	request += "&client_id=";
    request += CODE.URLEncode2(_client_id);
    request += "&client_secret=";
    request += CODE.URLEncode2(_subscription_key);
	request += "&scope=";
	request += CODE.URLEncode2("https://speech.platform.bing.com");
	http.setProperty("Content-Length", request.length());
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}

	http.sendData(request);

    delay(10);

	int code;
	code = http.getResponse(_json_str);
	if (code != HTTP_CODE_OK) {
		if (code < ERR_HTTPREQUEST_END) {
			_err = code;
		} else {
			_err = ERR_HTTPREQUEST_RESPONSE;
		}
		return false;
	}

	_err = ERR_HTTPREQUEST_OK;
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(_json_str);
	if (root.success()) {
		_expires_in = millis() + (uint32_t) root["expires_in"] * 1000;
		const char *access_token = root["access_token"];    // Get Access Token
		int size = strlen(access_token);
		if (_access_token != NULL) {
			free(_access_token);
			_access_token = NULL;
		}
		_access_token = (char *) malloc(size + 1);
		if (_access_token != NULL) {
			strcpy(_access_token, access_token);
		} else {
			_err = ERR_HTTPREQUEST_MEMORY;
		}
	}
	free(_json_str);
	_json_str = NULL;
	if (_err == ERR_HTTPREQUEST_OK) {
		return true;
	} else {
		return false;
	}
}

void SpeechAPI::setAccessToken(const char *access_token) {
	if (access_token == NULL) {
		return;
	}

	int size = strlen(access_token);
	if (_access_token != NULL) {
		free(_access_token);
	}
	_access_token = (char *) malloc(size + 1);
	if (_access_token != NULL) {
		strcpy(_access_token, access_token);
	}
}

/**
 * params: lang-- see SpeechAPI.h
 *         rate -- 8000 or 16000
 */
boolean SpeechAPI::recognize(size_t length, size_t rate, uint8_t lang) {
	if (_access_token == NULL || _expires_in < millis()) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	if (length == 0 || lang > SPEECHAPI_zh_TW || (rate != 8000 && rate != 16000)) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}

	HttpRequest http(_host_voice_recognition);
	http.setMethod("POST");
	char guid[37];
    // We now create a URI for the request
	String uri = "/recognize/query";
	uri += "?version=3.0";
	uri += "&requestid=";
	CODE.cguid(guid);
	uri += guid;
	uri += "&appid=D4D52672-91D7-4C74-8AD8-42B1D98141A5";
	uri += "&format=json";
	uri += "&device.os=mkr1000";
	uri += "&scenarios=smd";
	uri += "&instanceid=";
	CODE.cguid(guid);
	uri += guid;
	uri += "&locale=";
	uri += language_country[lang];
	http.setURI(uri.c_str());
	http.setProperty("Content-Type", String("audio/wav; codec=\"audio/pcm\"; samplerate=") + rate);
	http.setProperty("Content-Length", length);
	http.setProperty("Authorization", String("Bearer ") + _access_token);
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}

	while (1) {
		uint8_t *data = NULL;
		if (_data_recognize_callback) {
			_data_recognize_callback(data, length);
		}
		if (data == NULL) {
			break;
		}
		http.sendData(data, length);
	}

	delay(10);

	int code;
	code = http.getResponse(_json_str, false);
	if (code == HTTP_CODE_OK || (400 <= code && code < 600)) {
		_err = ERR_HTTPREQUEST_OK;
		if (_parse_recognize_callback) {
			_parse(_parse_recognize_callback, code);
		}
		free(_json_str);
		_json_str = NULL;
		return true;
	} else {
		if (code < ERR_HTTPREQUEST_END) {
			_err = code;
		} else {
			_err = ERR_HTTPREQUEST_RESPONSE;
		}
		return false;
	}
}

/**
 * params: lang-- see SpeechAPI.h
 *         rate -- 8000 or 16000
 */
boolean SpeechAPI::synthesize(const char *text, size_t rate, boolean raw, uint8_t lang, boolean female) {
	if (_access_token == NULL || _expires_in < millis()) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	if (text == NULL || lang > SPEECHAPI_zh_TW || (rate != 8000 && rate != 16000)) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}

	HttpRequest http(_host_voice_recognition);
	http.setMethod("POST");
	http.setURI("/synthesize");
	http.setProperty("Content-Type", "application/ssml+xml");
	http.setProperty("X-Microsoft-OutputFormat", outputFormat[raw ? 0 : 2 + rate / 16000]);	
	http.setProperty("Authorization", String("Bearer ") + _access_token);
	http.setProperty("X-Search-AppId", _app_id);
	http.setProperty("X-Search-ClientID", _client_id);
	http.setProperty("User-Agent", "PETDOG");
	String ssml = "<speak version='1.0' xml:lang='";
	ssml += language_country[lang];
	ssml += "'><voice xml:gender='";
	ssml += female ? "female" : "male";
	ssml += "'";
	if (strcmp_P(service_name_mapping[mapping_index[lang][female ? 0 : 1]], "")) {
		ssml += " name='Microsoft Server Speech Text to Speech Voice (";
		ssml += language_country[lang];
		ssml += ", ";
		ssml += service_name_mapping[mapping_index[lang][female ? 0 : 1]];
		ssml += ")'>";
	} else {
		ssml += ">";
	}
	ssml += text;
    ssml += "</voice></speak>";
	http.setProperty("Content-Length", ssml.length());
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}

	http.sendData(ssml);

	delay(10);

	int code;
	code = http.getResponse(_json_str, false, false);
	if (code == HTTP_CODE_OK || (400 <= code && code < 600)) {
		_err = ERR_HTTPREQUEST_OK;
		if (_parse_synthesize_callback) {
			_parse(_parse_synthesize_callback, code);
		}
		free(_json_str);
		_json_str = NULL;
		return true;
	} else if (code == HTTP_BINARY_PAYLOAD) {
		_err = ERR_HTTPREQUEST_OK;
		uint8_t data[32];
		size_t len;
		for (uint8_t i = 0; i < 10; i ++) {
			while ((len = http.getBinaryPayload(data, 32)) > 0) {
				i = 0;
				if (_data_synthesize_callback) {
					_data_synthesize_callback(data, len);
				}
			}
			delay(100);
		}
		return true;
	} else {
		if (code < ERR_HTTPREQUEST_END) {
			_err = code;
		} else {
			_err = ERR_HTTPREQUEST_RESPONSE;
		}
		return false;
	}
}

boolean SpeechAPI::LUIS(const char *luis_appid, const char *luis_subscriptionkey, const char *text) {
	if (luis_appid == NULL || luis_subscriptionkey == NULL || text == NULL) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}

	HttpRequest http("api.projectoxford.ai");
	http.setMethod("GET");
    // We now create a URI for the request
	String uri = "/luis/v1/application";
	uri += "?id=";
	uri += luis_appid;
	uri += "&subscription-key=";
	uri += luis_subscriptionkey;
	uri += "&q=";
	uri += CODE.URLEncode2(text);
	http.setURI(uri.c_str());
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}

	delay(10);

	int code;
	code = http.getResponse(_json_str, false);
	if (code == HTTP_CODE_OK || (400 <= code && code < 600)) {
		_err = ERR_HTTPREQUEST_OK;
		if (_parse_LUIS_callback) {
			_parse(_parse_LUIS_callback, code);
		}
		free(_json_str);
		_json_str = NULL;
		return true;
	} else {
		if (code < ERR_HTTPREQUEST_END) {
			_err = code;
		} else {
			_err = ERR_HTTPREQUEST_RESPONSE;
		}
		return false;
	}
}

void SpeechAPI::onDataRecognize(RECOGNIZE_CALLBACK(fn)) {
	_data_recognize_callback = fn;
}

void SpeechAPI::onDataSynthesize(SYNTHESIZE_CALLBACK(fn)) {
	_data_synthesize_callback = fn;
}

void SpeechAPI::onParseRecognize(PARSE_CALLBACK2(fn)) {
	_parse_recognize_callback = fn;
}

void SpeechAPI::onParseSynthesize(PARSE_CALLBACK2(fn)) {
	_parse_synthesize_callback = fn;
}

void SpeechAPI::onParseLUIS(PARSE_CALLBACK2(fn)) {
	_parse_LUIS_callback = fn;
}

void SpeechAPI::_parse(PARSE_CALLBACK2(fn), int code) {
	if (fn) {
		fn(_json_str, code);
	}
}

SpeechAPI SAMSPO;
