#define ARDUINOJSON_USE_LONG_LONG 1 // we need to store long long

#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SimpleDHT.h>
#include "eva.h"
#include "data.h"

/**
 * Imports:
 * EVA_CONFIG_WLAN_SSID
 * EVA_CONFIG_WLAN_PASSWORD
 */
#include "config.h"

// https://github.com/Links2004/arduinoWebSockets
#include <WebSocketsClient.h>


// SETTINGS

const char* wlan_ssid             = EVA_CONFIG_WLAN_SSID;
const char* wlan_password         = EVA_CONFIG_WLAN_PASSWORD;

const String eva_client           = EVA_CONFIG_CLIENT_ID;
const String eva_client_secret    = EVA_CONFIG_CLIENT_SECRET;

const String eva_user             = EVA_CONFIG_USER;
const String eva_password         = EVA_CONFIG_PASSWORD;

const String eva_sourceid         = "esp8266-test01";
const String eva_feed             = "global";

#define USE_SERIAL Serial

// CONSTANTS

const int led = D0;
const char* host = "cs.everyaware.eu";
const int httpsPort = 443;
const char* fingerprint = "29 41 9A 40 AF AF C6 70 65 E3 33 5D D2 B5 22 76 A0 07 13 6C";

// GLOBALS

WebSocketsClient webSocket;

String accessToken;
String refreshToken;

SimpleDHT11 dht;
int pinDHT = D1;

// FUNCTIONS


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

    switch (type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[WSc] Disconnected!\n");
            break;

        case WStype_CONNECTED:
            USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
            break;

        case WStype_TEXT: {

                String text = (char*) payload;

                USE_SERIAL.printf("[WSc] get text: %s\n", payload);

                if (payload[0] == 'h') {
                    
                    Serial.println("Heartbeat!");
                    
                } else if (payload[0] == 'o') {
                    
                    String msg = "[\"CONNECT\\naccept-version:1.1,1.0\\nauthorization:Bearer ";
                    msg += accessToken;
                    msg += "\\nheart-beat:10000,10000\\n\\n\\u0000\"]";
                    webSocket.sendTXT(msg);

                } else if (text.startsWith("a[\"CONNECTED")) {

                    // subscribe to message queue (data comes in through this channel)
                    String msg = "[\"SUBSCRIBE\\nid:sub-0\\ndestination:/user/queue/messages\\n\\n\\u0000\"]";
                    webSocket.sendTXT(msg);
                    delay(1000);

                    // subscribe to ack queue (we get an ACK after we request data updates from a source)
                    msg = "[\"SUBSCRIBE\\nid:sub-1\\ndestination:/user/queue/ack\\n\\n\\u0000\"]";
                    webSocket.sendTXT(msg);
                    delay(1000);

                    // send request to send data updates from a source
                    msg = "[\"SEND\\ndestination:/app/channelpoint/register\\n\\n{\\\"feed\\\":\\\"";
                    msg += eva_feed;
                    msg += "\\\",\\\"sourceId\\\":\\\"";
                    msg += eva_sourceid;
                    msg += "\\\",\\\"channels\\\":[\\\"ledOn\\\"],\\\"requestId\\\":\\\"randomConstant:x12gazs81\\\"}\\u0000\"]";
                    webSocket.sendTXT(msg);
                    delay(1000);
                    
                } else if (text.startsWith("a[\"MESSAGE") && text.indexOf("destination:/user/queue/messages") >= 0) {

                    Serial.print("Reveived data update: ");
                    int index = text.indexOf("ledOn");
                    if (index >= 0) {

                        int offset = 21;
                        Serial.print("ledOn=");
                        Serial.println(text.substring(index + offset, index + offset + 4));
                        bool ledOn = (text[index + offset] == 't'); // extracts first letter of true/false
                        if (ledOn) {
                            digitalWrite(led, LOW); // low turns it on ... don't know why this is reversed
                        } else {
                            digitalWrite(led, HIGH);
                        }
                        
                    } else {
                        Serial.println("no 'ledOn' channel available.");
                    }
                    
                }

                // send message to server
                // webSocket.sendTXT("message here");
                break;
            }
        case WStype_BIN:
            USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
        default:
            USE_SERIAL.printf("[WSc] something: %s\n", payload);
            break;
    }
}

void getTokens() {

    EveryAwareAccess eva;
    eva.init(eva_user, eva_password, eva_client, eva_client_secret);
    String refreshToken = eva.getRefreshToken();
    
    EveryAwareAccess eva2;
    eva2.init(refreshToken, eva_client, eva_client_secret);

}


// CORE

void setup() {

    // setup pins
    pinMode(led, OUTPUT);
    pinMode(led, HIGH);

    Serial.begin(115200);

    Serial.println(WiFi.macAddress());

    Serial.print("Logging into WLAN: "); Serial.print(wlan_ssid); Serial.print(" ...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(wlan_ssid, wlan_password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" success.");
    Serial.print("IP: "); Serial.println(WiFi.localIP());


    // get access and refresh token
    getTokens();


    // initialize websocket connection
    int randomNumber = random(0, 999);
    String socketUrl = "/socketentry/";
    socketUrl += random(0, 999);
    socketUrl += "/";
    socketUrl += random(0, 999999);
    socketUrl += "/websocket";
    webSocket.begin("cs.everyaware.eu", 80, socketUrl);
    webSocket.setExtraHeaders();
    //  webSocket.begin("echo.websocket.org", 80, "/");
    //  webSocket.setAuthorization(const_cast<char*>(("Bearer " + accessToken).c_str()));
    //  webSocket.setAuthorization(const_cast<char*>(eva_user.c_str()), const_cast<char*>(eva_password.c_str()));
    webSocket.onEvent(webSocketEvent);

    //  webSocket.setAuthorization(const_cast<char*>(("Bearer " + accessToken).c_str()));
    ////  webSocket.setAuthorization(const_cast<char*>(eva_user.c_str()), const_cast<char*>(eva_password.c_str()));
    //  int randomNumber = random(0,999);
    //  String socketUrl = "/ubicon-webapp/socketentry/";
    //  socketUrl += random(0,999);
    //  socketUrl += "/";
    //  socketUrl += random(0,999999);
    //  socketUrl += "/websocket";
    //  webSocket.begin("192.168.43.128", 8080, socketUrl);
    //  webSocket.onEvent(webSocketEvent);
}

void loop() {

    // test

    JsonArray & data = EvaDataArrayBuilder(1200).
        addDataPoint(123123).
            addChannelValue("temp", 8.9).
            addChannel("gps").
                addComponent("lon", 45.0).
                addComponent("lat", 12.1).
                finalizeChannel().
            finalizeDataPoint().
        finalizeDataArray();

    // rest
    webSocket.loop();

    //  if(WiFi.status() == WL_CONNECTED){ //Check WiFi connection status
    //
    //    Serial.println(""); Serial.print("Connected: "); Serial.println(time(nullptr));
    //
    //    // reading and sending data
    //
    //    byte temperature = 0;
    //    byte humidity = 0;
    //    int err = SimpleDHTErrSuccess;
    //    if ((err = dht.read(pinDHT, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    //      Serial.print("Read DHT11 failed, err="); Serial.print(err);
    //    } else {
    //
    //      Serial.print("Sample OK: ");
    //      Serial.print((int)temperature); Serial.print(" *C, ");
    //      Serial.print((int)humidity); Serial.println(" %");
    //
    //      // put data into JSON
    //
    //      const size_t bufferSize = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3) + 60;
    //      DynamicJsonBuffer jsonBuffer(bufferSize);
    //
    //      JsonObject& point = jsonBuffer.createObject();
    //      long long timestamp = time(NULL);
    //      point["timestamp"] = timestamp * 1000LL;
    //
    //      JsonObject& channels = point.createNestedObject("channels");
    //      JsonObject& channelTemp = channels.createNestedObject("temperature");
    //      channelTemp["value"] = temperature;
    //      JsonObject& channelHum = channels.createNestedObject("humidity");
    //      channelHum["value"] = humidity;
    //
    //      JsonArray& data = jsonBuffer.createArray();
    //      data.add(point);
    //
    //      data.printTo(Serial);
    //      Serial.println("");
    //
    //      // send data
    //
    //      Serial.println("Sending data #################################");
    //      const String url = "https://cs.everyaware.eu/api/v1/packets";
    //
    ////      HTTPClient http;
    ////      http.begin(url, fingerprint);
    ////
    ////      http.addHeader("Authorization",             "Bearer " + accessToken);
    ////      http.addHeader("Content-Type",              "application/json");
    ////
    ////      http.addHeader("meta.sourceId",             eva_sourceid);
    ////      http.addHeader("meta.feeds",                eva_feed);
    ////      http.addHeader("data.contentDetails.type",  "generic");
    ////
    ////      String postMessage;
    ////      data.printTo(postMessage);
    ////
    ////      int httpCode = http.POST(postMessage);
    ////
    ////      Serial.print("http result:");
    ////      Serial.println(httpCode);
    ////
    ////      String payload = http.getString();
    ////      Serial.println(payload);
    ////
    ////      http.end();
    //
    ////      const size_t bufferSize2 = JSON_OBJECT_SIZE(2) + 110;
    ////      DynamicJsonBuffer jsonBuffer2(bufferSize2);
    ////      JsonObject& message = jsonBuffer2.parseObject(payload);
    ////      if (message.containsKey("error")) {
    ////        Serial.println("Error occurred. Requesting new tokens.");
    ////        getTokens();
    ////      }
    //
    //    }

//    // receiving data

//    Serial.println("Requesting data #################################");
//    
//    const String requestUrl = "https://cs.everyaware.eu/api/v1/finalfeeds/global/sources/" + eva_sourceid;
//      
//    HTTPClient http;
//    http.begin(requestUrl, fingerprint);
//      
//    http.addHeader("Authorization",             "Bearer " + accessToken);
//    http.addHeader("Content-Type",              "application/json");
//            
//    int httpCode = http.GET();
//    
//    Serial.print("http result:");
//    Serial.println(httpCode);
//    
//    String payload = http.getString();
////    Serial.println(payload);
//    
//    http.end();
//
//    const size_t bufferSize = 2*JSON_OBJECT_SIZE(1) + 5*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 292;
//    DynamicJsonBuffer jsonBuffer(bufferSize);
//    JsonObject& latestData = jsonBuffer.parseObject(payload); 
//    if (latestData["recentData"]["ledOn"] != NULL) {
//      const bool ledOn = latestData["recentData"]["ledOn"][""]["val"];
//        if (ledOn) {
//          digitalWrite(led, HIGH);
//        } else {
//          digitalWrite(led, LOW);
//        }
//    } else {
//      Serial.println("No attribute \"ledOn\".");
//    }

    //
    //  } else {
    //
    //    Serial.print("Error in Wifi connection.");
    //
    //  }
    //
    //  delay(1000); // Send a request every 1 seconds

}
