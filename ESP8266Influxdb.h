/* Influxdb library

   MIT license
   Written by HW Wong
 */

#ifndef INFLUXDB_H
#define INFLUXDB_H
#include "Arduino.h"
#include <ESP8266WiFi.h>

enum DB_RESPONSE {DB_SUCCESS, DB_ERROR, DB_CONNECT_FAILED};

// Url encode function
String URLEncode(String msg);

class FIELD
{
public:
        FIELD(String m);

        String measurement;

        void addField(String key, float value);
        void addTag(String key, String value);
        void empty();
        String postString();

private:
        String _data;
        String _tag;

};

class Influxdb : private WiFiClient
{
public:
        Influxdb(const char* host, uint16_t port);

        DB_RESPONSE opendb(String db);
        DB_RESPONSE opendb(String db, String user, String password);
        DB_RESPONSE write(FIELD data);
        DB_RESPONSE write(String data);
        DB_RESPONSE query(String sql);
        //uint8_t createDatabase(char *dbname);
        DB_RESPONSE response();

        using WiFiClient::available;
        using WiFiClient::read;
        using WiFiClient::flush;
        using WiFiClient::find;
        using WiFiClient::findUntil;
        using WiFiClient::peek;
        using WiFiClient::readBytes;
        using WiFiClient::readBytesUntil;
        using WiFiClient::readString;
        using WiFiClient::readStringUntil;
        using WiFiClient::parseInt;
        using WiFiClient::setTimeout;

private:
        uint16_t _port;
        const char* _host;
        String _db;
        DB_RESPONSE _response;

};




#endif
