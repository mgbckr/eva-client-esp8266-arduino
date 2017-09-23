#define ARDUINOJSON_USE_LONG_LONG 1 // we need to store long long

#include <ESP8266WiFi.h>

#include "EvaClient.h"
#include "EvaData.h"

/**
 * Imports:
 * EVA_CONFIG_WLAN_SSID
 * EVA_CONFIG_WLAN_PASSWORD
 */
#include "sketch_eva_lib_config.h"

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

// GLOBALS

EvaClient eva;

// FUNCTIONS

// CORE

void setup() {

    // setup pins
    pinMode(led, OUTPUT);
    pinMode(led, HIGH);

    // init serial
    Serial.begin(115200);


    // logging into WLAN
    
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

    // initialize EvA client
    eva.init(eva_user, eva_password, eva_client, eva_client_secret);
    eva.connectWebsocket();
}

void loop() {


    eva.loop();

    // test
    
//    JsonArray & data = EvaDataArrayBuilder(1200).
//        addDataPoint(123123).
//            addChannelValue("temp", 8.9).
//            addChannel("gps").
//                addComponent("lon", 45.0).
//                addComponent("lat", 12.1).
//                finalizeChannel().
//            finalizeDataPoint().
//        finalizeDataArray();

    // rest

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
