/*
 *  HttpRequest.cpp
 *  Copyright (c) 2015-2016 ARESPNO.
 */

#include "HttpRequest.h"

HttpRequest::HttpRequest()
:_method(NULL)
,_URI(NULL)
,_isConnected(false)
,_ssl(false)
,_host(NULL)
,_property("") {
}

HttpRequest::HttpRequest(const char *host)
:_method(NULL)
,_URI(NULL)
,_isConnected(false)
,_ssl(false) {
	_host = host;
	if (host != NULL) {
		_property += "Host: ";
		_property += host;
		_property += "\r\n";
	} else {
		_property = "";
	}
}

HttpRequest::~HttpRequest() {
	stop();
}

void HttpRequest::setMethod(const char *method) {
	if (method != NULL) {
		_method = method;
	}
}

void HttpRequest::setProperty(const char *property, const __FlashStringHelper *value) {
	setProperty(property, reinterpret_cast<const char *>(value));
}

void HttpRequest::setProperty(const char *property, const char *value) {
	if (property != NULL && _property.indexOf(property) < 0 && value != NULL) {
		_property += property;
		_property += ": ";
		_property += value;
		_property += "\r\n";
	}
}

void HttpRequest::setProperty(const char *property, int value) {
	if (property != NULL && _property.indexOf(property) < 0) {
		_property += property;
		_property += ": ";
		_property += value;
		_property += "\r\n";
	}
}

void HttpRequest::setProperty(const char *property, String value) {
	setProperty(property, value.c_str());
}

void HttpRequest::setURI(const char *URI) {
	if (URI != NULL) {
		_URI = URI;
	}
}

uint8_t HttpRequest::_connect(uint16_t port, boolean SSL) {
	if (_host == NULL || _method == NULL || _URI == NULL) {
		return ERR_HTTPREQUEST_PARAMS;
	}

	_ssl = SSL;
	if (_ssl) {
#if defined(ESP8266)
		if (!_clientSSL.connect(_host, port)) {
			return ERR_HTTPREQUEST_CONNECT;
		}
#elif defined(__SAMD21G18A__)
		if (!_client.connectSSL(_host, port)) {
			return ERR_HTTPREQUEST_CONNECT;
		}
#endif
	} else {
		if (!_client.connect(_host, port)) {
			return ERR_HTTPREQUEST_CONNECT;
		}
	}
	
	_isConnected = true;

    // This will send the request to the server
#if defined(ESP8266)
	if (_ssl) {
		_clientSSL.print(String(_method) + " " + _URI + " HTTP/1.1\r\n" +
				_property + "\r\n");// + "Connection: Keep-Alive\r\n\r\n");
	} else {
#endif
    _client.print(String(_method) + " " + _URI + " HTTP/1.1\r\n" +
			_property + "\r\n");// + "Connection: Keep-Alive\r\n\r\n");
#if defined(ESP8266)
	}
#endif
	return ERR_HTTPREQUEST_OK;
}

uint8_t HttpRequest::connect(uint16_t port) {
	return _connect(port, false);
}

uint8_t HttpRequest::connectSSL(uint16_t port) {
	return _connect(port, true);
}

void HttpRequest::sendData(const uint8_t *data, size_t length) {
	if (!_isConnected) {
		return;
	}

	const size_t unit_size = HTTP_SEND_SIZE;
	size_t size_to_send = length;
	const uint8_t* send_start = data;

	while (size_to_send) {
		size_t will_send = (size_to_send < unit_size) ? size_to_send : unit_size;
		size_t sent;
#if defined(ESP8266)
		if (_ssl) {
			sent = _clientSSL.write(send_start, will_send);
		} else {
#endif
		sent = _client.write(send_start, will_send);
#if defined(ESP8266)
		}
#endif
		if (sent == 0) {
			break;
		}
		size_to_send -= sent;
		send_start += sent;
	}
}

void HttpRequest::sendData(const char *data) {
	sendData((const uint8_t *) data, strlen(data));
}

void HttpRequest::sendData(String data) {
	sendData((const uint8_t *) data.c_str(), data.length());
}

int HttpRequest::getResponse(char *&data, boolean only200, boolean jsons) {
	if (!_isConnected) {
		return ERR_HTTPREQUEST_CONNECT;
	}

	// Wait the host response, time out is 10000ms
	uint8_t count = 0;
	while (count < 100) {
#if defined(ESP8266)
		if (_ssl) {
			if (_clientSSL.available() > 0) {
				break;
			}
		} else {
#endif
		if (_client.available() > 0) {
			break;
		}
#if defined(ESP8266)
		}
#endif
		delay(100);
		count ++;
	}
	if (count >= 100) {
		return ERR_HTTPREQUEST_OUTTIME;
	}

	int code = ERR_HTTPREQUEST_RESPONSE;
	count = 0;
    // Read all the lines of the reply from server
	char line[256];
	int contentLength = 0;
	while (1) {
#if defined(ESP8266)
		if (_ssl) {
			if (_clientSSL.available() <= 0) {
				break;
			}
		} else {
#endif
		if (_client.available() <= 0) {
			break;
		}
#if defined(ESP8266)
		}
#endif
		int len;
#if defined(ESP8266)
		if (_ssl) {
			len = _clientSSL.readBytesUntil('\r', line, 255);
		} else {
#endif
		len = _client.readBytesUntil('\r', line, 255);
#if defined(ESP8266)
		}
#endif
		line[len] = '\x0';

//		Serial.print(line);

		char *ptr;
        if (count == 0 && (ptr = strstr(line, "HTTP/1.1")) != NULL) {
			// response code
			ptr += 9;
			code = strtol(ptr, NULL, 10);
			continue;
        } else if (!strcmp(line, "\n")) {
			// skip blank line
			count ++;
		} else if ((ptr = strstr(line, "Content-Length:")) != NULL) {
			ptr += 15;
			contentLength = strtol(ptr, NULL, 10);
			continue;
		} else if (!only200 && code == 202 && len >= 60 && (ptr = strstr(line, "Operation-Location:")) != NULL) {
			if (data != NULL) {
				free(data);
				data = NULL;
			}
			data = (char *) malloc(37);
			if (data != NULL) {
				strncpy(data, &line[len - 36], 36);
				data[36] = '\x0';
			} else {
				code = ERR_HTTPREQUEST_MEMORY;
			}
			count = 2;
			continue;
		}
		if (count == 1 && (code == HTTP_CODE_OK || (!only200 && 200 < code && code < 600))) {
			if (contentLength == 0) {
#if defined(ESP8266)
				if (_ssl) {
					len = _clientSSL.readBytesUntil('\r', line, 255);
				} else {
#endif
				len = _client.readBytesUntil('\r', line, 255);
#if defined(ESP8266)
				}
#endif
				line[len] = '\x0';
				contentLength = strtol(line, NULL, 16);
			}
			// get reply string
#if defined(ESP8266)
			if (_ssl) {
				_clientSSL.read();    // Skip '\n'
			} else {
#endif
			_client.read();    // Skip '\n'
#if defined(ESP8266)
			}
#endif
			if (data != NULL) {
				free(data);
				data = NULL;
			}
			
			if (!jsons && code == HTTP_CODE_OK) {
				return HTTP_BINARY_PAYLOAD;
			}
			
			data = (char *) malloc(contentLength + 1);  // contentLength + 1 for storage null character
			if (data != NULL) {
#if defined(ESP8266)
				if (_ssl) {
					contentLength = _clientSSL.readBytes(data, contentLength);
				} else {
#endif
				contentLength = _client.readBytes(data, contentLength);
#if defined(ESP8266)
				}
#endif
				data[contentLength] = '\x0';
			} else {
				code = ERR_HTTPREQUEST_MEMORY;
			}
			count ++;
        }
    }
	return code;
}

size_t HttpRequest::getBinaryPayload(uint8_t *data, size_t len) {
	if (data == NULL || len == 0) {
		return 0;
	}
#if defined(ESP8266)
	if (_ssl) {
		if (_clientSSL.available() <= 0) {
			return 0;
		}
	} else {
#endif
		if (_client.available() <= 0) {
			return 0;
		}
#if defined(ESP8266)
	}
#endif
	size_t length;
#if defined(ESP8266)
	if (_ssl) {
		length = _clientSSL.readBytes(data, len);
	} else {
#endif
		length = _client.readBytes(data, len);
#if defined(ESP8266)
	}
#endif
	return length;
}

void HttpRequest::stop() {
	if (_isConnected) {
#if defined(ESP8266)
		if (_ssl) {
			_clientSSL.flush();
			_clientSSL.stop();
		} else {
#endif
		_client.flush();
		_client.stop();
#if defined(ESP8266)
		}
#endif
		_isConnected = false;
	}
}
