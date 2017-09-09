#include "Arduino.h"
#include "ESP8266HTTPClient.h"
#include "eva.h"

const int httpsPort = 443;

// should this be dynamic?

EveryAware::EveryAware(String baseUrl) {
    _baseUrl = baseUrl;
}

EveryAware::~EveryAware() {
    // nothing to do
}

/**
 * To calculate the object size see:
 * https://bblanchon.github.io/ArduinoJson/assistant/ 
 */
JsonObject* EveryAware::getJson(String relativeUrl, size_t objectSize) {

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

bool EveryAware::init(
            const String username, 
            const String password,
            const String clientId,
            const String clientSecret) {

    // building URL to request refresh and access token
    // TODO: move this to a constant?
    String url = "https://cs.everyaware.eu/oauth/token";
    url += "?grant_type=password";
    url += "&client_id=" + clientId;
    url += "&client_secret=" + clientSecret;
    url += "&username=" + username;
    url += "&password=" + password;

    #if (EVA_DEBUG == 1)
        Serial.print("Requesting access token via: "); Serial.println(url);
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
        const size_t bufferSize = JSON_OBJECT_SIZE(4) + 160;
        DynamicJsonBuffer jsonBuffer(bufferSize);
        JsonObject& root = jsonBuffer.parseObject(content);

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

bool EveryAware::init(
    const String refreshToken,
    const String clientId,
    const String clientSecret) {

    // set refresh token
    _refreshToken = refreshToken;

    // directly get access token to check refresh token validity
    
    // building URL to request refresh and access token
    // TODO: move this to a constant?
    /**
    String url = "https://cs.everyaware.eu/oauth/token";
    url += "?grant_type=password";
    url += "&client_id=" + clientId;
    url += "&client_secret=" + clientSecret;
    url += "&username=" + username;
    url += "&password=" + password;
    */

    
}

