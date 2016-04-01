/*

  SpeakerRecognitionAPI.cpp - Microsoft Project Oxford Speaker Recognition API.
  Copyright (c) 2015-2016 ARESPNO.
  
*/

#include <ArduinoJson.h>
#include "HttpRequest.h"
#include "SpeakerRecognitionAPI.h"

const char *locale[] = {
	"en-US",
	"zh-CN",
};

const char *dictOfSpeaker PROGMEM = "/spid/v1.0/";
const char *_identification PROGMEM = "identificationProfiles";
const char *_verification PROGMEM = "verificationProfiles";

SpeakerRecognitionAPI::SpeakerRecognitionAPI()
:_subscription_key(NULL)
,_host_speaker_api("api.projectoxford.ai")
,_json_str(NULL)
,_data_callback(NULL)
,_parse_callback(NULL) {
}

SpeakerRecognitionAPI::~SpeakerRecognitionAPI() {
	if (_json_str != NULL) {
		free(_json_str);
		_json_str = NULL;
	}
}

void SpeakerRecognitionAPI::begin(const char *subscription_key) {
	_subscription_key = subscription_key;
}

boolean SpeakerRecognitionAPI::_finHttpRequest(int code) {
	if (code == HTTP_CODE_OK || (200 < code && code < 600)) {
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

boolean SpeakerRecognitionAPI::createProfile(char *guid, boolean identify, uint8_t lang) {
	if (guid == NULL || lang >= LOCALE_END) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}
	if (_subscription_key == NULL) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	
	HttpRequest http(_host_speaker_api);
	http.setMethod("POST");
    // We now create a URI for the request
    String uri = dictOfSpeaker;
	if (identify) {
		uri += _identification;
	} else {
		uri += _verification;
	}
	http.setURI(uri.c_str());
	http.setProperty("Content-Type", "application/json");
	http.setProperty("Ocp-Apim-Subscription-Key", _subscription_key);
	http.setProperty("Content-Length", strlen(locale[lang]) + 14);
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}
	
	http.sendData(String("{") + "\"locale\":\"" + locale[lang] + "\",}");
	
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
		if (identify) {
			strcpy(guid, (const char *) root["identificationProfileId"]);
		} else {
			strcpy(guid, (const char *) root["verificationProfileId"]);
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

boolean SpeakerRecognitionAPI::deleteProfile(const char *guid, boolean identify) {
	if (guid == NULL) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}
	if (_subscription_key == NULL) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	
	HttpRequest http(_host_speaker_api);
	http.setMethod("DELETE");
    // We now create a URI for the request
    String uri = dictOfSpeaker;
	if (identify) {
		uri += _identification;
	} else {
		uri += _verification;
	}
	uri += "/";
	uri += guid;
	http.setURI(uri.c_str());
	http.setProperty("Ocp-Apim-Subscription-Key", _subscription_key);
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}
	
	delay(10);
	
	int code = http.getResponse(_json_str, false);
	boolean result = _finHttpRequest(code);
	if (result) {
		if (_parse_callback) {
			_parse(_parse_callback, code);
		}
		free(_json_str);
		_json_str = NULL;
	}
	return result;
}

boolean SpeakerRecognitionAPI::resetEnrollments(const char *guid, boolean identify) {
	if (guid == NULL) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}
	if (_subscription_key == NULL) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	
	HttpRequest http(_host_speaker_api);
	http.setMethod("POST");
    // We now create a URI for the request
    String uri = dictOfSpeaker;
	if (identify) {
		uri += _identification;
	} else {
		uri += _verification;
	}
	uri += "/";
	uri += guid;
	uri += "/reset";
	http.setURI(uri.c_str());
	http.setProperty("Ocp-Apim-Subscription-Key", _subscription_key);
	http.setProperty("Content-Length", 0);
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}
	
	delay(10);
	
	int code = http.getResponse(_json_str, false);
	boolean result = _finHttpRequest(code);
	if (result) {
		if (_parse_callback) {
			_parse(_parse_callback, code);
		}
		free(_json_str);
		_json_str = NULL;
	}
	return result;
}

// If guid is NULL, get All profiles
boolean SpeakerRecognitionAPI::getProfile(const char *guid, boolean identify) {
	if (_subscription_key == NULL) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	
	HttpRequest http(_host_speaker_api);
	http.setMethod("GET");
    // We now create a URI for the request
    String uri = dictOfSpeaker;
	if (identify) {
		uri += _identification;
	} else {
		uri += _verification;
	}
	if (guid) {
		uri += "/";
		uri += guid;
	}
	http.setURI(uri.c_str());
	http.setProperty("Ocp-Apim-Subscription-Key", _subscription_key);
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}
	
	delay(10);
	
	int code = http.getResponse(_json_str, false);
	boolean result = _finHttpRequest(code);
	if (result) {
		if (_parse_callback) {
			_parse(_parse_callback, code);
		}
		free(_json_str);
		_json_str = NULL;
	}
	return result;
}

boolean SpeakerRecognitionAPI::createEnrollment(const char *guid, boolean identify, size_t length) {
	if (guid == NULL || length <= 0) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}
	if (_subscription_key == NULL) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	
	HttpRequest http(_host_speaker_api);
	http.setMethod("POST");
    // We now create a URI for the request
    String uri = dictOfSpeaker;
	if (identify) {
		uri += _identification;
	} else {
		uri += _verification;
	}
	uri += "/";
	uri += guid;
	uri += "/enroll";
	http.setURI(uri.c_str());
	http.setProperty("Content-Type", "application/octet-stream");
	http.setProperty("Content-Length", length);
	http.setProperty("Ocp-Apim-Subscription-Key", _subscription_key);
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}
	
	while (1) {
		uint8_t *data = NULL;
		if (_data_callback) {
			_getData(_data_callback, data, length);
//			_data_callback(data, length);
		}
		if (data == NULL) {
			break;
		}
		http.sendData(data, length);
	}

	delay(10);
	
	int code = http.getResponse(_json_str, false);
	boolean result = _finHttpRequest(code);
	if (result) {
		if (_parse_callback) {
			_parse(_parse_callback, code);
		}
		free(_json_str);
		_json_str = NULL;
	}
	return result;
}

boolean SpeakerRecognitionAPI::getOperationStatus(const char *operationId) {
	if (operationId == NULL) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}
	if (_subscription_key == NULL) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	
	HttpRequest http(_host_speaker_api);
	http.setMethod("GET");
    // We now create a URI for the request
    String uri = dictOfSpeaker;
	uri += "operations/";
	uri += operationId;
	http.setURI(uri.c_str());
	http.setProperty("Ocp-Apim-Subscription-Key", _subscription_key);
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}
	
	delay(10);
	
	int code = http.getResponse(_json_str, false);
	boolean result = _finHttpRequest(code);
	if (result) {
		if (_parse_callback) {
			_parse(_parse_callback, code);
		}
		free(_json_str);
		_json_str = NULL;
	}
	return result;
}

boolean SpeakerRecognitionAPI::identification(const char *guids, size_t length) {
	if (guids == NULL || length <= 0) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}
	if (_subscription_key == NULL) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	
	HttpRequest http(_host_speaker_api);
	http.setMethod("POST");
    // We now create a URI for the request
    String uri = dictOfSpeaker;
	uri += "identify?identificationProfileIds=";
	uri += guids;
	http.setURI(uri.c_str());
	http.setProperty("Content-Type", "application/octet-stream");
	http.setProperty("Content-Length", length);
	http.setProperty("Ocp-Apim-Subscription-Key", _subscription_key);
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}
	
	while (1) {
		uint8_t *data = NULL;
		if (_data_callback) {
			_getData(_data_callback, data, length);
		}
		if (data == NULL) {
			break;
		}
		http.sendData(data, length);
	}

	delay(10);
	
	int code = http.getResponse(_json_str, false);
	boolean result = _finHttpRequest(code);
	if (result) {
		if (_parse_callback) {
			_parse(_parse_callback, code);
		}
		free(_json_str);
		_json_str = NULL;
	}
	return result;
}

boolean SpeakerRecognitionAPI::listAllPhrases(uint8_t lang) {
	if (lang >= LOCALE_END) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}
	if (_subscription_key == NULL) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	
	HttpRequest http(_host_speaker_api);
	http.setMethod("GET");
    // We now create a URI for the request
    String uri = dictOfSpeaker;
	uri += "verificationPhrases?locale=";
	uri += locale[lang];
	http.setURI(uri.c_str());
	http.setProperty("Ocp-Apim-Subscription-Key", _subscription_key);
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}
	
	delay(10);
	
	int code = http.getResponse(_json_str, false);
	boolean result = _finHttpRequest(code);
	if (result) {
		if (_parse_callback) {
			_parse(_parse_callback, code);
		}
		free(_json_str);
		_json_str = NULL;
	}
	return result;
}

boolean SpeakerRecognitionAPI::verification(const char *guid, size_t length) {
	if (guid == NULL || length <= 0) {
		_err = ERR_HTTPREQUEST_PARAMS;
		return false;
	}
	if (_subscription_key == NULL) {
		_err = ERR_HTTPREQUEST_NOKEY;
		return false;
	}
	
	HttpRequest http(_host_speaker_api);
	http.setMethod("POST");
    // We now create a URI for the request
    String uri = dictOfSpeaker;
	uri += "verify?verificationProfileId=";
	uri += guid;
	http.setURI(uri.c_str());
	http.setProperty("Content-Type", "application/octet-stream");
	http.setProperty("Content-Length", length);
	http.setProperty("Ocp-Apim-Subscription-Key", _subscription_key);
	if ((_err = http.connectSSL(443)) != ERR_HTTPREQUEST_OK) {
		return false;
	}
	
	while (1) {
		uint8_t *data = NULL;
		if (_data_callback) {
			_getData(_data_callback, data, length);
		}
		if (data == NULL) {
			break;
		}
		http.sendData(data, length);
	}

	delay(10);
	
	int code = http.getResponse(_json_str, false);
	boolean result = _finHttpRequest(code);
	if (result) {
		if (_parse_callback) {
			_parse(_parse_callback, code);
		}
		free(_json_str);
		_json_str = NULL;
	}
	return result;
}

void SpeakerRecognitionAPI::onData(SPEAKER_CALLBACK(fn)) {
	_data_callback = fn;
}

void SpeakerRecognitionAPI::onParse(PARSE_CALLBACK2(fn)) {
	_parse_callback = fn;
}

void SpeakerRecognitionAPI::_parse(PARSE_CALLBACK2(fn), int code) {
	if (fn) {
		fn(_json_str, code);
	}
}

void SpeakerRecognitionAPI::_getData(SPEAKER_CALLBACK(fn), uint8_t *&data, size_t &len) {
	if (fn) {
		fn(data, len);
	}
}

SpeakerRecognitionAPI SRAMSPO;
