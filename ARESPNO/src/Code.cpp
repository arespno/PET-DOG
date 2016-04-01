/*
  Code.cpp - Code translate
  Copyright (c) 2015-2016 ARESPNO.
*/

#include "Code.h"

Code::Code() {
}

/*-----------------------------------------------------------------------------
 * Create a GUID
 * @function: cguid()
 * @params: guid -- char array [37]
 ------------------------------------------------------------------------------*/
uint8_t Code::cguid(char *guid, boolean dashes) {
	if (guid == NULL) {
		return 0;
	}
	randomSeed(micros());
	for (uint8_t i = 0; i < 36; i ++) {
		if (dashes && (i == 8 || i == 13 || i == 18 || i == 23)) {
			guid[i] = '-';
		} else {
			guid[i] = hexToChar2[random(16)];
		}
	}
	if (dashes) {
		guid[36] = '\0';
	} else {
		guid[32] = '\0';
	}
	return 1;
}


/*-----------------------------------------------------------------------------
 * URL encode
 * @function: URLEncode()
 * @params: encode -- Storage encode string
 *          src -- The string to encode
 *          count -- The max count to encode
 * @return -- The encode length, if encode == NULL, only return length
 *
 ------------------------------------------------------------------------------*/

 size_t Code::URLEncode(char *encode, const char *src, size_t count) {
	if (src == NULL) {
		return 0;
	}

	size_t index = 0;
	size_t size = 0;
	size_t len = strlen(src);
	
	if (encode == NULL) {
		while (index < len) {
			char c = src[index ++];
			if ((c < '0' && c != '-' && c != '.' && c != ' ') || (c < 'A' && c > '9') ||
				(c > 'Z' && c < 'a' && c != '_') || (c > 'z')) {
				size += 3;
			} else {
				size ++;
			}
		}
	} else {
		while (index < len && size < count) {
			char c = src[index ++];
			if (c == ' ') {
				*encode++ = '+';
				size ++;
			} else if ((c < '0' && c != '-' && c != '.') || (c < 'A' && c > '9') ||
				(c > 'Z' && c < 'a' && c != '_') || (c > 'z')) {
				*encode++ = '%';
				*encode++ = hexToChar[c >> 4];
				*encode++ = hexToChar[c & 0x0F];
				size += 3;
			} else {
				*encode++ = c;
				size ++;
			}
		}
	}
	return size;
}

/*-----------------------------------------------------------------------------
 * URL encode
 * @function: URLEncode2()
 * @params: src -- The string to encode
 * @return -- The encoded string
 *
 ------------------------------------------------------------------------------*/

String Code::URLEncode2(const char *src) {
	if (src == NULL) {
		return "";
	}

	size_t index = 0;
	size_t len = strlen(src);
	String encode = "";
	while (index < len) {
		char c = src[index ++];
		if (c == ' ') {
			encode += '+';
		} else if ((c < '0' && c != '-' && c != '.') || (c < 'A' && c > '9') ||
			(c > 'Z' && c < 'a' && c != '_') || (c > 'z')) {
			encode += '%';
			encode += hexToChar[c >> 4];
			encode += hexToChar[c & 0x0F];
		} else {
			encode += c;
		}
	}
	return encode;
}

/*-----------------------------------------------------------------------------
 * Base64 to Bytes translate
 * @function: base64tobytes()
 * @params: bytestr -- Storage translated string
 *          base64str -- The string to translate
 *          count -- The base64str length
 * @return -- The bytestr length, if bytestr == NULL, only return length
 *
 ------------------------------------------------------------------------------*/

size_t Code::base64tobytes(uint8_t *bytestr, const char *base64str, size_t count) {
	if (base64str == NULL) {
		return 0;
	}
	
	size_t size = count / 4 * 3;
	if (bytestr != NULL) {
		for (size_t i = 0; i < count / 4; i ++) {
			*bytestr++ = (decodeTable[*base64str++] << 0x02) | (decodeTable[*base64str] >> 0x04);
			*bytestr++ = (decodeTable[*base64str++] << 0x04) | (decodeTable[*base64str] >> 0x02);
			*bytestr++ = (decodeTable[*base64str++] << 0x06) | (decodeTable[*base64str++]);
		}
		if (size > 0) {
			if (*(base64str - 2) == '=') {
				size -= 2;
			} else if (*--base64str == '=') {
				size --;
			}
		}
	}
	return size;
}

/*-----------------------------------------------------------------------------
 * Bytes to Base64 translate
 * @function: bytestobase64()
 * @params: base64str -- Storage translated string
 *          bytestr -- The string to translate
 *          count -- The bytestr length
 * @return -- The base64str length, if base64str == NULL, only return length
 *
 ------------------------------------------------------------------------------*/

size_t Code::bytestobase64(char *base64str, const uint8_t *bytestr, size_t count) {
	if (bytestr == NULL) {
		return 0;
	}

	size_t size = count / 3;
	if (count % 3) {
		size += 1;
	}
	size *= 4;
	if (base64str != NULL) {
		for (size_t i = 0; i < count / 3; i ++) {
			*base64str++ = encodeTable[(*bytestr & 0xFC) >> 0x02];
			*base64str++ = encodeTable[((*bytestr++ & 0x03) << 0x04) | ((*bytestr & 0xF0) >> 0x04)];
			*base64str++ = encodeTable[((*bytestr++ & 0x0F) << 0x02) | ((*bytestr & 0xC0) >> 0x06)];
			*base64str++ = encodeTable[*bytestr++ & 0x3F];
		}
		if (count % 3) {
			*base64str++ = encodeTable[(*bytestr & 0xFC) >> 0x02];
			if ((count % 3) == 1) {
				*base64str++ = encodeTable[(*bytestr & 0x03) << 0x04];
				*base64str++ = '=';
			} else {
				*base64str++ = encodeTable[((*bytestr++ & 0x03) << 0x04) | ((*bytestr & 0xF0) >> 0x04)];
				*base64str++ = encodeTable[(*bytestr & 0x0F) << 0x02];
			}
			*base64str = '=';
		}
	}
	return size;
}

/*-----------------------------------------------------------------------------
 * Utf8 to Unicode translate
 * @function: utf8stounis()
 * @params: unistr -- Storage translated string
 *          utf8str -- The string to translate
 *          count -- The max count to translate (wchar_t)
 *
 * @return -- The unistr length, if unistr == NULL, only return length
 * Please reference to mbstowcs()
 ------------------------------------------------------------------------------*/

size_t Code::utf8stounis(wchar_t *unistr, const char *utf8str, size_t count) {
	if (utf8str == NULL) {
		return 0;
	}

	size_t index = 0;
	size_t size = 0;
	size_t len = strlen(utf8str);
	
	if (unistr == NULL) {
		while (utf8str[index] != '\x0' && index < len) {
			uint8_t c = utf8str[index] & 0xF0;
			if (c < 0x80) {
				index ++;
			} else if (c == 0xE0) {
				index += 3;
			} else if (c == 0xC0 || c == 0xD0) {
				index += 2;
			} else {    // 4--6 bytes utf8 no support
				return 0;
				
			}
			size ++;
		}
	} else {
		while (utf8str[index] != '\x0' && index < len && size < count) {
			uint8_t c = utf8str[index] & 0xF0;
			if (c < 0x80) {
				unistr[size] = utf8str[index];
				index ++;
			} else if (c == 0xE0) {
				unistr[size] = (utf8str[index + 2] & 0x3F) | ((utf8str[index + 1] & 0x3F) << 6) |
									((utf8str[index] & 0x0F) << 12);
				index += 3;
			} else if (c == 0xC0 || c == 0xD0) {
				unistr[size] = (utf8str[index + 1] & 0x3F) | ((utf8str[index] & 0x1F) << 6);
				index += 2;
			} else {    // 4--6 bytes utf8 no support
				return 0;
				
			}
			size ++;
		}
	}
	return size;
}

/*-----------------------------------------------------------------------------
 * Unicode to Utf8 translate
 * @function: unistoutf8s()
 * @params: utf8str -- Storage translated string
 *          unistr -- The string to translate
 *          count -- The max count to translate (char)
 *
 * @return -- The utf8str length, if utf8str == NULL, only return length
 * Please reference to wcstombs()
 ------------------------------------------------------------------------------*/

size_t Code::unistoutf8s(char *utf8str, const wchar_t *unistr, size_t count) {
	if (unistr == NULL) {
		return 0;
	}

	size_t index = 0;
	size_t size = 0;
	
	if (utf8str == NULL) {
		while (unistr[index] != 0) {
			if (unistr[index] < 0x0080) {
				size ++;
			} else if (unistr[index] < 0x0800) {
				size += 2;
			} else {
				size += 3;
			}
			index ++;
		}
	} else {
		while (unistr[index] != 0 && size < count) {
			if (unistr[index] < 0x0080) {
				utf8str[size ++] = unistr[index];
			} else if (unistr[index] < 0x0800) {
				utf8str[size ++] = 0xC0 | ((unistr[index] & 0x07C0) >> 6);
				utf8str[size ++] = 0x80 | (unistr[index] & 0x003F);
			} else {
				utf8str[size ++] = 0xE0 | ((unistr[index] & 0xF000) >> 12);
				utf8str[size ++] = 0x80 | ((unistr[index] & 0x0FC0) >> 6);
				utf8str[size ++] = 0x80 | (unistr[index] & 0x003F);
			}
			index ++;
		}
	}
	return size;
}

Code CODE;