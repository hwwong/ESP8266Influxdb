
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266Influxdb.h>

const char *INFLUXDB_HOST = "192.168.1.111";
const uint16_t INFLUXDB_PORT = 8086;

const char *DATABASE = "db_name";
const char *DB_USER = "user";
const char *DB_PASSWORD = "password";

ESP8266WiFiMulti WiFiMulti;
Influxdb influxdb(INFLUXDB_HOST, INFLUXDB_PORT);

void setup() {
        Serial.begin(115200);
        WiFiMulti.addAP("ssid", "pw");
        while (WiFiMulti.run() != WL_CONNECTED) {
                delay(100);
        }
        Serial.println("Ready");
        influxdb.opendb(DATABASE, DB_USER, DB_PASSWORD);
}

void loop() {
        // Writing data with influxdb HTTP API
        // https://influxdb.com/docs/v0.9/guides/writing_data.html
        Serial.println("Writing data to host " + String(INFLUXDB_HOST) + ":" +
                       INFLUXDB_PORT + "'s database=" + DATABASE);
        String data = "analog_read,method=HTTP_API,pin=A0 value=" + String(analogRead(A0));
        influxdb.write(data);
        Serial.println(influxdb.response() == DB_SUCCESS ? "Writing Success"
                       : "Writing failed");

        // Writing data using FIELD object
        // Create field object with measurment name=analog_read
        FIELD dataObj("analog_read");
        dataObj.addTag("method", "Field_object"); // Add method tag
        dataObj.addTag("pin", "A0");         // Add pin tag
        dataObj.addField("value", analogRead(A0)); // Add value field
        Serial.println(influxdb.write(dataObj) == DB_SUCCESS ? "Writing sucess"
                       : "Writing failed");

        // Empty field object.
        dataObj.empty();

        // Querying Data
        // https://influxdb.com/docs/v0.9/query_language/query_syntax.html
        Serial.println("Querying data ........");
        influxdb.query("select * from analog_read");
        delay(40000);
}
