#include "Arduino.h"
#include "ESP8266HTTPClient.h"
#include "eva.h"

const int httpsPort = 443;

// should this be dynamic?

EveryAwareAccess::EveryAwareAccess(String baseUrl) {
    _baseUrl = baseUrl;
    _syncTime();
}

EveryAwareAccess::~EveryAwareAccess() {
    // nothing to do
}

/**
 * To calculate the object size see:
 * https://bblanchon.github.io/ArduinoJson/assistant/ 
 */
JsonObject* EveryAwareAccess::getJson(String relativeUrl, size_t objectSize) {

    const String url = _baseUrl + relativeUrl;
    
    #if (EVA_DEBUG == 1)
        Serial.print("Requesting JSON via: "); Serial.println(url);
    #endif

    HTTPClient http;
    http.begin(url, EVA_SSH_FINGERPRINT);

    http.addHeader("Content-Type", "application/json");
    int httpCode = http.GET();

    #if (EVA_DEBUG == 1)
        Serial.print("HTTP response code:"); Serial.println(httpCode);
    #endif

    // extract content
    String content = http.getString();

    // end connection
    http.end();

    #if (EVA_DEBUG == 1)
        Serial.println("HTTP content:");
        Serial.println(content);
    #endif

    if (httpCode == 200) {
        
        #if (EVA_DEBUG == 1)
                Serial.println("Processing response.");
        #endif

        // see: https://bblanchon.github.io/ArduinoJson/assistant/
        const size_t bufferSize = objectSize;
        DynamicJsonBuffer jsonBuffer(bufferSize);
        JsonObject& root = jsonBuffer.parseObject(content);

        return &root;
    } else {
        return nullptr;
    }
}

bool EveryAwareAccess::init(
            const String username, 
            const String password,
            const String clientId,
            const String clientSecret) {

    _clientId = clientId;
    _clientSecret = clientSecret;

    // building URL to request refresh and access token
    // TODO: move this to a constant?
    String url = "/oauth/token";
    url += "?grant_type=password";
    url += "&client_id=" + clientId;
    url += "&client_secret=" + clientSecret;
    url += "&username=" + username;
    url += "&password=" + password;

    JsonObject* jsonPtr = getJson(url, JSON_OBJECT_SIZE(4) + 160);
    return _handleOauthResponse(jsonPtr);
    
}

bool EveryAwareAccess::init(
    const String refreshToken,
    const String clientId,
    const String clientSecret) {

    _refreshToken = refreshToken;
    _clientId = clientId;
    _clientSecret = clientSecret;

    return _refreshAccessToken();
}

bool EveryAwareAccess::_refreshAccessToken() {
    
    // building URL to refresh access token
    // TODO: move this to a constant?
    String url = "https://cs.everyaware.eu/oauth/token";
    url += "?grant_type=refresh_token";
    url += "&client_id=" + _clientId;
    url += "&client_secret=" + _clientSecret;
    url += "&refresh_token=" + _refreshToken;

    JsonObject* jsonPtr = getJson(url, JSON_OBJECT_SIZE(4) + 160);
    return _handleOauthResponse(jsonPtr);
}

bool EveryAwareAccess::_handleOauthResponse(JsonObject* jsonPtr) {
    
    if (jsonPtr) {

        JsonObject& root = *jsonPtr;

        const char* newAccessToken = root["access_token"];
        const char* newRefreshToken = root["refresh_token"];
        _accessToken = String(newAccessToken);
        _refreshToken = String(newRefreshToken);
        _refreshTokenExpiresIn = root["expires_in"];
        _refreshTokenReferenceTime = millis();

        #if (EVA_DEBUG == 1)
                Serial.println("Refresh token: "); Serial.println(_refreshToken);
                Serial.println("Access token:  "); Serial.println(_accessToken);
                Serial.println("Tokens successfully retrieved.");
        #endif

        return true;
    } else {
        return false;
    }
}

String EveryAwareAccess::getRefreshToken() {
    return _refreshToken;
}


unsigned long EveryAwareAccess::getRefreshTokenExpiresIn(bool update) {
    
    if (update) {
        _refreshAccessToken();
    }
    
    return _refreshTokenExpiresIn - (millis() - _refreshTokenReferenceTime);
}

String EveryAwareAccess::getAccessToken() {
    return _accessToken;
}

/** 
 * Synchronize time useing SNTP. This is necessary to verify that
 * the TLS certificates offered by the server are currently valid.
 * 
 * TODO: check this
 */
void EveryAwareAccess::_syncTime() {

    #if (EVA_DEBUG == 1)
        Serial.print("Setting time using SNTP ...");
    #endif
    
    configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    time_t now = time(nullptr);
    while (now < 1000) {
        delay(500);
        #if (EVA_DEBUG == 1)
            Serial.print(".");
        #endif
        now = time(nullptr);
    }
    
    #if (EVA_DEBUG == 1)
        Serial.println(" success.");
        Serial.print("Current time (UNIX): "); Serial.println(now);
    #endif
}



void EveryAwareClient::postData(
            const String feed, 
            const String source, 
            const String datapoints) {
}

void EveryAwareClient::postData(
            const String feed, 
            const String source, 
            JsonArray& datapoints) {



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
}

