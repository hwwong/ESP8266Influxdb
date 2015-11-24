#include "Arduino.h"
#include "ESP8266Influxdb.h"
#include <ESP8266WiFi.h>

// comment below line to disable debug print
#define DEBUG_PRINT(a) (Serial.println(a))
#ifndef DEBUG_PRINT(a)
#define DEBUG_PRINT(a)
#endif

Influxdb::Influxdb(const char *host, uint16_t port) {
        _port = port;
        _host = host;
}

DB_RESPONSE Influxdb::opendb(String db, String user, String password) {
        _db = "db=" + db + "&u=" + user + "&p=" + password;
}

DB_RESPONSE Influxdb::opendb(String db) {
        _db = "db=" + db;
}

DB_RESPONSE Influxdb::write(FIELD data) {
        return this->write(data.postString());
}

DB_RESPONSE Influxdb::write(String data) {
        if (!_client.connect(_host, _port)) {
                DEBUG_PRINT("connection failed");
                _response = DB_CONNECT_FAILED;
                return _response;
        }
        String postHead = "POST /write?" + _db + " HTTP/1.1\r\n";
        // postHead += "Host: " + _client.localIP().toString() + ":" +
        // _client.localPort() + "\r\n";
        // postHead += "Content-Type: application/x-www-form-urlencoded\r\n";
        postHead += "Content-Length: " + String(data.length()) + "\r\n\r\n";

        DEBUG_PRINT("Writing data to " + String(_host) + ":" + String(_port));
        _client.print(postHead + data);
        DEBUG_PRINT(postHead + data);

        uint8_t t = 0;
        // Check the reply whether writing is success or not
        while (!_client.available() && t < 200) {
                delay(10);
                t++;
        }
        if (_client.available()) {
                _response = (_client.findUntil("204", "\r")) ? DB_SUCCESS : DB_ERROR;
                return _response;
        } else
                _response = DB_ERROR;
        return DB_ERROR;
}

DB_RESPONSE Influxdb::query(String sql) {

        if (!_client.connect(_host, _port)) {
                DEBUG_PRINT("connection failed");
                _response = DB_CONNECT_FAILED;
                return _response;
        }

        String url = "/query?pretty=true&" + _db;
        url += "&q=" + URLEncode(sql);
        DEBUG_PRINT("Requesting URL: ");
        DEBUG_PRINT(url);

        // This will send the request to the server
        _client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + _host +
                      ":" + _port + "\r\n" + "Connection: close\r\n\r\n");

        // Read all the lines of the reply from server and print them to Serial
        uint8_t t = 0;
        while (!_client.available() && t < 200) {
                delay(10);
                t++;
        }

        while (_client.available())

        {
                String line = _client.readStringUntil('\r');
                Serial.println(line);
        }
}

DB_RESPONSE Influxdb::response() {
        return _response;
}

/* -----------------------------------------------*/
//        Field object
/* -----------------------------------------------*/
FIELD::FIELD(String m) {
        measurement = m;
}

void FIELD::empty() {
        _data = "";
        _tag = "";
}

void FIELD::addTag(String key, String value) {
        _tag += "," + key + "=" + value;
}

void FIELD::addField(String key, float value) {
        _data = (_data == "") ? (" ") : (_data += ",");
        _data += key + "=" + String(value);
}

String FIELD::postString() {
        //  uint32_t utc = 1448114561 + millis() /1000;
        return measurement + _tag + _data;
}

// URL Encode with Arduino String object
String URLEncode(String msg) {
        const char *hex = "0123456789abcdef";
        String encodedMsg = "";

        uint16_t i;
        for (i = 0; i < msg.length(); i++) {
                if (('a' <= msg.charAt(i) && msg.charAt(i) <= 'z') ||
                    ('A' <= msg.charAt(i) && msg.charAt(i) <= 'Z') ||
                    ('0' <= msg.charAt(i) && msg.charAt(i) <= '9')) {
                        encodedMsg += msg.charAt(i);
                } else {
                        encodedMsg += '%';
                        encodedMsg += hex[msg.charAt(i) >> 4];
                        encodedMsg += hex[msg.charAt(i) & 15];
                }
        }
        return encodedMsg;
}
