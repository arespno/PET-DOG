/*
 * FIRMATA SETUP
 * because we use smartlink, don't write ssid and passphrase in wifiConfig.h
 * must modify firmata\utility\WiFi101Stream.h
 * on 
  inline bool maintain()
  {
  }
 * modify
      else
      {
        result = WiFi.begin( _ssid );
      } 
 * to
      else if ( _ssid )
      {
        result = WiFi.begin( _ssid );
      } 
      else
      {
        result = WiFi.begin();
      }
 * and add
  // Only set _port
  inline int begin(uint16_t port)
  {
    if( !is_ready() ) return 0;

    _port = port;

    int result = WiFi.begin( );
    if( result == 0 ) return 0;
    
    _server = WiFiServer( port );
    _server.begin();
    return result;
  }
 */

/*
 * Firmata -- receive string callback function
 */
void stringCallback(char *string) {
    if (string) {
        _dbgprintln(string);
        petdog(string);
    }
}

void initFirmata() {
    Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);
    // we receive string from windows 10 remote for arduino
    Firmata.attach(STRING_DATA, stringCallback);
    Firmata.begin(stream);
}

