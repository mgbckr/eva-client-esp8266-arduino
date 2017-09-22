#include "EvaClientOAuth.h"

bool EvaClientOAuth::init(
            const String username, 
            const String password,
            const String clientId,
            const String clientSecret) {

    _clientId = clientId;
    _clientSecret = clientSecret;

    _syncTime();

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

bool EvaClientOAuth::init(
        const String refreshToken,
        const String clientId,
        const String clientSecret) {

    _refreshToken = refreshToken;
    _clientId = clientId;
    _clientSecret = clientSecret;
    
    _syncTime();

    return _refreshAccessToken();
}

bool EvaClientOAuth::_refreshAccessToken() {
    
    // building URL to refresh access token
    // TODO: move this to a constant?
    String url = "/oauth/token";
    url += "?grant_type=refresh_token";
    url += "&client_id=" + _clientId;
    url += "&client_secret=" + _clientSecret;
    url += "&refresh_token=" + _refreshToken;

    JsonObject* jsonPtr = getJson(url, JSON_OBJECT_SIZE(4) + 160);
    return _handleOauthResponse(jsonPtr);
}

bool EvaClientOAuth::_handleOauthResponse(JsonObject* jsonPtr) {
    
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

String EvaClientOAuth::getRefreshToken() {
    return _refreshToken;
}


unsigned long EvaClientOAuth::getRefreshTokenExpiresIn(bool update) {
    
    if (update) {
        _refreshAccessToken();
    }
    
    return _refreshTokenExpiresIn - (millis() - _refreshTokenReferenceTime);
}

String EvaClientOAuth::getAccessToken() {
    return _accessToken;
}

