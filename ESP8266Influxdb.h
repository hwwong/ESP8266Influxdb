/* Influxdb library

MIT license
Written by HW Wong
*/

#ifndef INFLUXDB_H
#define INFLUXDB_H
#include "Arduino.h"
#include <ESP8266WiFi.h>

enum DB_RESPOND {DB_SUCCESS, DB_ERROR};

// Url encode function
String URLEncode(String msg);

class FIELD
{
  public:
    FIELD(String m);

    String measurement;

    void addField(String key, float value);
    void addTag(String key, String value);
    String postString();
    void empty();
  private:
    String _data;
    String _tag;

};

class Influxdb
{
  public:
    Influxdb(const char* host, uint16_t port);

    DB_RESPOND opendb(String db);
    DB_RESPOND opendb(String db, String user, String password);

    DB_RESPOND write(FIELD data);
    DB_RESPOND write(String data);

    DB_RESPOND query(String sql);
    //uint8_t createDatabase(char *dbname);

  private:
    WiFiClient _client;
    uint16_t _port;
    const char* _host;
    String _db;

};




#endif
