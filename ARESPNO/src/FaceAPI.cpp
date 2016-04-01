/*

  FaceAPI.cpp - Microsoft Project Oxford Face API.
  Copyright (c) 2015-2016 ARESPNO.
  
*/

#include "HttpRequest.h"
#include "FaceAPI.h"

FaceAPI::FaceAPI()
:_subscription_key(NULL)
,_host_face_api("api.projectoxford.ai")
,_json_str(NULL)
,_data_callback(NULL)
,_parse_callback(NULL) {
}

FaceAPI::~FaceAPI() {
	if (_json_str != NULL) {
		free(_json_str);
		_json_str = NULL;
	}
}

void FaceAPI::begin(const char *subscription_key) {
	_subscription_key = subscription_key;
}

boolean FaceAPI::_finHttpRequest(int code) {
	if (code == HTTP_CODE_OK || (400 <= code && code < 600)) {
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
 *  Detect face
 *  @params: length -- Image size
 *           mode -- return value, e.g. face id, face landmarks, age, ......
 *           url -- Image url, if has url would ignore _data_callback
 *  @return: if success, return true.
 */
boolean FaceAPI::detectFace(size_t length, const char *url, uint8_t mode) {
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
    String uri = "/face/v1.0/detect";
	if (mode & RETURN_FACE_ALL) {
		uri += '?';
		if (mode & RETURN_FACEID) {
			uri += "returnFaceId=true";
		} else {
			uri += "returnFaceId=false";
		}
		if (mode & RETURN_FACELANDMARKS) {
			uri += "&returnFaceLandmarks=true";
		} else {
			uri += "&returnFaceLandmarks=false";
		}
		if (mode & RETURN_FACEATTRIBUTES) {
			uri += "&returnFaceAttributes=";
			if (mode & RETURN_AGE) {
				uri += "age,";
			}
			if (mode & RETURN_GENDER) {
				uri += "gender,";
			}
			if (mode & RETURN_SMILE) {
				uri += "smile,";
			}
			if (mode & RETURN_FACIALHAIR) {
				uri += "facialHair,";
			}
			if (mode & RETURN_HEADPOSE) {
				uri += "headPose,";
			}
			uri.remove(uri.length() - 1);
		}
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

void FaceAPI::onData(FACEAPI_CALLBACK(fn)) {
	_data_callback = fn;
}

void FaceAPI::onParse(PARSE_CALLBACK2(fn)) {
	_parse_callback = fn;
}

FaceAPI FAMSPO;