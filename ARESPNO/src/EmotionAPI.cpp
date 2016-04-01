/*

  EmotionAPI.cpp - Microsoft Project Oxford Emotion API.
  Copyright (c) 2015-2016 ARESPNO.
  
*/

#include "HttpRequest.h"
#include "EmotionAPI.h"

EmotionAPI::EmotionAPI()
:_subscription_key(NULL)
,_host_face_api("api.projectoxford.ai")
,_json_str(NULL)
,_data_callback(NULL)
,_parse_callback(NULL) {
}

EmotionAPI::~EmotionAPI() {
	if (_json_str != NULL) {
		free(_json_str);
		_json_str = NULL;
	}
}

void EmotionAPI::begin(const char *subscription_key) {
	_subscription_key = subscription_key;
}

boolean EmotionAPI::_finHttpRequest(int code) {
	if (code == HTTP_CODE_OK || (400 <= code && code < 500)) {
		_err = ERR_HTTPREQUEST_OK;
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

/*
 *  Recognize emotion
 *  @params: length -- Image size
 *           mode -- Specify faceRectangles(e.g. "30,10,100,100" ...)
 *           url -- Image url, if has url would ignore _data_callback
 *  @return: if success, return true.
 */
boolean EmotionAPI::recognizeEmotion(size_t length, const char *url, const char *mode) {
	if (length == 0 || (_data_callback == NULL && url == NULL)) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}
	if (_subscription_key == NULL) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	
	HttpRequest http(_host_face_api);
	http.setMethod("POST");
    // We now create a URI for the request
    String uri = "/emotion/v1.0/recognize";
	if (mode != NULL) {
		uri += "?faceRectangles=";
		uri += mode;
	}
	http.setURI(uri.c_str());
	if (url != NULL) {
		http.setProperty("Content-Type", "application/json");
		length = strlen(url) + 10;
	} else {
		http.setProperty("Content-Type", "application/octet-stream");
	}
	http.setProperty("Ocp-Apim-Subscription-Key", _subscription_key);
	http.setProperty("Content-Length", length);
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}

	if (url != NULL) {
		http.sendData(String("{") + "\"url\":\"" + url + "\"}");
	} else {
		while (1) {
			uint8_t *data = NULL;
			if (_data_callback) {
				_data_callback(data, length);
			}
			if (data == NULL) {
				break;
			}
			http.sendData(data, length);
		}
	}

	delay(10);

	int code = http.getResponse(_json_str, false);
	boolean result = _finHttpRequest(code);
	if (result) {
		if (_parse_callback) {
			_parse_callback(_json_str, code);
		}
		free(_json_str);
		_json_str = NULL;
	}
	return result;
}

void EmotionAPI::onData(EMOTIONAPI_CALLBACK(fn)) {
	_data_callback = fn;
}

void EmotionAPI::onParse(PARSE_CALLBACK2(fn)) {
	_parse_callback = fn;
}

EmotionAPI EAMSPO;