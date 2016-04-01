/*
 *  HttpRequest.h
 *  Copyright (c) 2015-2016 ARESPNO.
 */

#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H

#include "Arduino.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>

#define HTTP_SEND_SIZE 2728

#elif defined(__SAMD21G18A__)
#include <SPI.h>
#include <WiFi101.h>

#define HTTP_SEND_SIZE 1400

#else
#error "Must be used for ESP8266 or Arduino/Genuino MKR1000!"
#endif

#define ERR_HTTPREQUEST_OK  0
#define ERR_HTTPREQUEST_NOKEY  1
#define ERR_HTTPREQUEST_CONNECT  2
#define ERR_HTTPREQUEST_OUTTIME  3
#define ERR_HTTPREQUEST_RESPONSE  4
#define ERR_HTTPREQUEST_MEMORY  5
#define ERR_HTTPREQUEST_PARAMS  6
#define ERR_HTTPREQUEST_END  7

#define HTTP_CODE_OK  200
#define HTTP_BINARY_PAYLOAD  0x7FFFFFFF

class HttpRequest {

public:
	HttpRequest();
	HttpRequest(const char *host);
	~HttpRequest();

	void setMethod(const char *method);                // Set http request method.
	void setProperty(const char *property, const __FlashStringHelper *value);
	void setProperty(const char *property, const char *value);
	void setProperty(const char *property, int value);
	void setProperty(const char *property, String value);
	                                                   // Set http request property with value.
	void setURI(const char *URI);                      // Set http request URI.
	uint8_t connect(uint16_t port);                    // Connect to http server with port.
	uint8_t connectSSL(uint16_t port);                 // Connect SSL to http server with port.
	void sendData(const uint8_t *data, size_t length);
	void sendData(const char *data);
	void sendData(String data);                        // Send data to http server.
	int getResponse(char *&data, boolean only200 = true, boolean jsons = true);
                                                  	   // Get http response string.
	size_t getBinaryPayload(uint8_t *data, size_t len);
	                                                   // Get http binary payload.
	
	void stop();                                       // Disconnect from http server.

protected:
	uint8_t _connect(uint16_t port, boolean SSL);

private:
	WiFiClient _client;            // The WiFiClient for http request.
#if defined(ESP8266)
	WiFiClientSecure _clientSSL;
#endif
	const char *_host;             // Http request host.
	const char *_method;           // Http request method.
	String _property;              // Http request property.
	const char *_URI;              // Http request URI.
	
	boolean _isConnected;          // Is connected to http server?
	boolean _ssl;

};

#endif  // _HTTP_REQUEST_H
