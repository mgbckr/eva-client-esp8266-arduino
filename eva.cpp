#include "Arduino.h"
#include "eva.h"
#include <time.h>

// https://github.com/Links2004/arduinoWebSockets
#include <WebSocketsClient.h>

const int httpsPort = 443;

static unsigned char evaRootCert[] = {
  0x30, 0x82, 0x03, 0x4a, 0x30, 0x82, 0x02, 0x32, 0xa0, 0x03, 0x02, 0x01,
  0x02, 0x02, 0x10, 0x44, 0xaf, 0xb0, 0x80, 0xd6, 0xa3, 0x27, 0xba, 0x89,
  0x30, 0x39, 0x86, 0x2e, 0xf8, 0x40, 0x6b, 0x30, 0x0d, 0x06, 0x09, 0x2a,
  0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00, 0x30, 0x3f,
  0x31, 0x24, 0x30, 0x22, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x1b, 0x44,
  0x69, 0x67, 0x69, 0x74, 0x61, 0x6c, 0x20, 0x53, 0x69, 0x67, 0x6e, 0x61,
  0x74, 0x75, 0x72, 0x65, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x43,
  0x6f, 0x2e, 0x31, 0x17, 0x30, 0x15, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13,
  0x0e, 0x44, 0x53, 0x54, 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x43, 0x41,
  0x20, 0x58, 0x33, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x30, 0x30, 0x39, 0x33,
  0x30, 0x32, 0x31, 0x31, 0x32, 0x31, 0x39, 0x5a, 0x17, 0x0d, 0x32, 0x31,
  0x30, 0x39, 0x33, 0x30, 0x31, 0x34, 0x30, 0x31, 0x31, 0x35, 0x5a, 0x30,
  0x3f, 0x31, 0x24, 0x30, 0x22, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x1b,
  0x44, 0x69, 0x67, 0x69, 0x74, 0x61, 0x6c, 0x20, 0x53, 0x69, 0x67, 0x6e,
  0x61, 0x74, 0x75, 0x72, 0x65, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20,
  0x43, 0x6f, 0x2e, 0x31, 0x17, 0x30, 0x15, 0x06, 0x03, 0x55, 0x04, 0x03,
  0x13, 0x0e, 0x44, 0x53, 0x54, 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x43,
  0x41, 0x20, 0x58, 0x33, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09,
  0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03,
  0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01,
  0x00, 0xdf, 0xaf, 0xe9, 0x97, 0x50, 0x08, 0x83, 0x57, 0xb4, 0xcc, 0x62,
  0x65, 0xf6, 0x90, 0x82, 0xec, 0xc7, 0xd3, 0x2c, 0x6b, 0x30, 0xca, 0x5b,
  0xec, 0xd9, 0xc3, 0x7d, 0xc7, 0x40, 0xc1, 0x18, 0x14, 0x8b, 0xe0, 0xe8,
  0x33, 0x76, 0x49, 0x2a, 0xe3, 0x3f, 0x21, 0x49, 0x93, 0xac, 0x4e, 0x0e,
  0xaf, 0x3e, 0x48, 0xcb, 0x65, 0xee, 0xfc, 0xd3, 0x21, 0x0f, 0x65, 0xd2,
  0x2a, 0xd9, 0x32, 0x8f, 0x8c, 0xe5, 0xf7, 0x77, 0xb0, 0x12, 0x7b, 0xb5,
  0x95, 0xc0, 0x89, 0xa3, 0xa9, 0xba, 0xed, 0x73, 0x2e, 0x7a, 0x0c, 0x06,
  0x32, 0x83, 0xa2, 0x7e, 0x8a, 0x14, 0x30, 0xcd, 0x11, 0xa0, 0xe1, 0x2a,
  0x38, 0xb9, 0x79, 0x0a, 0x31, 0xfd, 0x50, 0xbd, 0x80, 0x65, 0xdf, 0xb7,
  0x51, 0x63, 0x83, 0xc8, 0xe2, 0x88, 0x61, 0xea, 0x4b, 0x61, 0x81, 0xec,
  0x52, 0x6b, 0xb9, 0xa2, 0xe2, 0x4b, 0x1a, 0x28, 0x9f, 0x48, 0xa3, 0x9e,
  0x0c, 0xda, 0x09, 0x8e, 0x3e, 0x17, 0x2e, 0x1e, 0xdd, 0x20, 0xdf, 0x5b,
  0xc6, 0x2a, 0x8a, 0xab, 0x2e, 0xbd, 0x70, 0xad, 0xc5, 0x0b, 0x1a, 0x25,
  0x90, 0x74, 0x72, 0xc5, 0x7b, 0x6a, 0xab, 0x34, 0xd6, 0x30, 0x89, 0xff,
  0xe5, 0x68, 0x13, 0x7b, 0x54, 0x0b, 0xc8, 0xd6, 0xae, 0xec, 0x5a, 0x9c,
  0x92, 0x1e, 0x3d, 0x64, 0xb3, 0x8c, 0xc6, 0xdf, 0xbf, 0xc9, 0x41, 0x70,
  0xec, 0x16, 0x72, 0xd5, 0x26, 0xec, 0x38, 0x55, 0x39, 0x43, 0xd0, 0xfc,
  0xfd, 0x18, 0x5c, 0x40, 0xf1, 0x97, 0xeb, 0xd5, 0x9a, 0x9b, 0x8d, 0x1d,
  0xba, 0xda, 0x25, 0xb9, 0xc6, 0xd8, 0xdf, 0xc1, 0x15, 0x02, 0x3a, 0xab,
  0xda, 0x6e, 0xf1, 0x3e, 0x2e, 0xf5, 0x5c, 0x08, 0x9c, 0x3c, 0xd6, 0x83,
  0x69, 0xe4, 0x10, 0x9b, 0x19, 0x2a, 0xb6, 0x29, 0x57, 0xe3, 0xe5, 0x3d,
  0x9b, 0x9f, 0xf0, 0x02, 0x5d, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x42,
  0x30, 0x40, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff,
  0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30, 0x0e, 0x06, 0x03, 0x55,
  0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x01, 0x06, 0x30,
  0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xc4, 0xa7,
  0xb1, 0xa4, 0x7b, 0x2c, 0x71, 0xfa, 0xdb, 0xe1, 0x4b, 0x90, 0x75, 0xff,
  0xc4, 0x15, 0x60, 0x85, 0x89, 0x10, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
  0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00, 0x03, 0x82, 0x01,
  0x01, 0x00, 0xa3, 0x1a, 0x2c, 0x9b, 0x17, 0x00, 0x5c, 0xa9, 0x1e, 0xee,
  0x28, 0x66, 0x37, 0x3a, 0xbf, 0x83, 0xc7, 0x3f, 0x4b, 0xc3, 0x09, 0xa0,
  0x95, 0x20, 0x5d, 0xe3, 0xd9, 0x59, 0x44, 0xd2, 0x3e, 0x0d, 0x3e, 0xbd,
  0x8a, 0x4b, 0xa0, 0x74, 0x1f, 0xce, 0x10, 0x82, 0x9c, 0x74, 0x1a, 0x1d,
  0x7e, 0x98, 0x1a, 0xdd, 0xcb, 0x13, 0x4b, 0xb3, 0x20, 0x44, 0xe4, 0x91,
  0xe9, 0xcc, 0xfc, 0x7d, 0xa5, 0xdb, 0x6a, 0xe5, 0xfe, 0xe6, 0xfd, 0xe0,
  0x4e, 0xdd, 0xb7, 0x00, 0x3a, 0xb5, 0x70, 0x49, 0xaf, 0xf2, 0xe5, 0xeb,
  0x02, 0xf1, 0xd1, 0x02, 0x8b, 0x19, 0xcb, 0x94, 0x3a, 0x5e, 0x48, 0xc4,
  0x18, 0x1e, 0x58, 0x19, 0x5f, 0x1e, 0x02, 0x5a, 0xf0, 0x0c, 0xf1, 0xb1,
  0xad, 0xa9, 0xdc, 0x59, 0x86, 0x8b, 0x6e, 0xe9, 0x91, 0xf5, 0x86, 0xca,
  0xfa, 0xb9, 0x66, 0x33, 0xaa, 0x59, 0x5b, 0xce, 0xe2, 0xa7, 0x16, 0x73,
  0x47, 0xcb, 0x2b, 0xcc, 0x99, 0xb0, 0x37, 0x48, 0xcf, 0xe3, 0x56, 0x4b,
  0xf5, 0xcf, 0x0f, 0x0c, 0x72, 0x32, 0x87, 0xc6, 0xf0, 0x44, 0xbb, 0x53,
  0x72, 0x6d, 0x43, 0xf5, 0x26, 0x48, 0x9a, 0x52, 0x67, 0xb7, 0x58, 0xab,
  0xfe, 0x67, 0x76, 0x71, 0x78, 0xdb, 0x0d, 0xa2, 0x56, 0x14, 0x13, 0x39,
  0x24, 0x31, 0x85, 0xa2, 0xa8, 0x02, 0x5a, 0x30, 0x47, 0xe1, 0xdd, 0x50,
  0x07, 0xbc, 0x02, 0x09, 0x90, 0x00, 0xeb, 0x64, 0x63, 0x60, 0x9b, 0x16,
  0xbc, 0x88, 0xc9, 0x12, 0xe6, 0xd2, 0x7d, 0x91, 0x8b, 0xf9, 0x3d, 0x32,
  0x8d, 0x65, 0xb4, 0xe9, 0x7c, 0xb1, 0x57, 0x76, 0xea, 0xc5, 0xb6, 0x28,
  0x39, 0xbf, 0x15, 0x65, 0x1c, 0xc8, 0xf6, 0x77, 0x96, 0x6a, 0x0a, 0x8d,
  0x77, 0x0b, 0xd8, 0x91, 0x0b, 0x04, 0x8e, 0x07, 0xdb, 0x29, 0xb6, 0x0a,
  0xee, 0x9d, 0x82, 0x35, 0x35, 0x10
};
static unsigned int evaRootCertLen = 846;


EveryAwareAccess::EveryAwareAccess(const char *  host) {
    _init(host, evaRootCert, evaRootCertLen);
}

EveryAwareAccess::EveryAwareAccess(const char * host, const unsigned char caCert[], const unsigned int caCertLen) {
    _init(host, caCert, caCertLen);
}

EveryAwareAccess::~EveryAwareAccess() {
    // nothing to do
}

void EveryAwareAccess::_init(const char * host, const unsigned char caCert[], const unsigned int caCertLen) {
    _host = host;
    _caCert = caCert;
    _caCertLen = caCertLen;

    WiFiClientSecure wifiClient;
    _wifiClient = wifiClient;
    bool res = _wifiClient.setCACert(_caCert, _caCertLen);
    if (!res) {
        #if (EVA_DEBUG == 1)
            Serial.println("Failed to load root CA certificate!");
        #endif
        while (true) {
            yield();
        }
    }
}

/**
 * To calculate the object size see:
 * https://bblanchon.github.io/ArduinoJson/assistant/ 
 */
JsonObject* EveryAwareAccess::getJson(String relativeUrl, size_t objectSize) {
    
    #if (EVA_DEBUG == 1)
        Serial.print("Requesting JSON from: "); Serial.print(_host); Serial.println(relativeUrl);
    #endif

    // connect to remote server
    Serial.print("Connecting to: "); Serial.println(_host);
    if (!_wifiClient.connect(_host, httpsPort)) {
        #if (EVA_DEBUG == 1)
            Serial.println("Connection failed.");
        #endif
        return nullptr;
    } 
    
    // verify validity of server's certificate
    if (_wifiClient.verifyCertChain(_host)) {
        #if (EVA_DEBUG == 1)
            Serial.println("Server certificate verified.");
        #endif
    } else {
        #if (EVA_DEBUG == 1)
            Serial.println("ERROR: certificate verification failed!");
        #endif
        return nullptr;
    }

    #if (EVA_DEBUG == 1)
        Serial.println("Sending request");
    #endif
    _wifiClient.print(String("GET ") + relativeUrl + " HTTP/1.1\r\n" +
               "Host: " + _host + "\r\n" +
               "User-Agent: EvaClient\r\n" +
               "Connection: close\r\n\r\n");

    #if (EVA_DEBUG == 1)
        Serial.println("Request sent.");
        Serial.println("Reading headers.");
        Serial.println("========= headers");
    #endif
    bool response_ok = false;
    while (_wifiClient.connected()) {
        
        String line = _wifiClient.readStringUntil('\n');

        #if (EVA_DEBUG == 1)
            Serial.println(line);
        #endif
        if (line.startsWith("HTTP/1.1 200")) {
            response_ok = true;
        }
        
        if (line == "\r") {
            Serial.println("========= headers");
            Serial.println("Headers received");
            break;
        }
    }
    #if (EVA_DEBUG == 1)
        Serial.println("Reading content.");
    #endif
    // we expect a chunked transfer encoding (see: https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Transfer-Encoding)
    String length = _wifiClient.readStringUntil('\n'); 
    String content = _wifiClient.readStringUntil('\n');
    #if (EVA_DEBUG == 1)
        Serial.println("========= content");
        Serial.print("Length: "); Serial.println(length);
        Serial.println(content);
        Serial.println("========= content");
    #endif
    
    if (response_ok) {
        
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

bool EveryAwareAccess::init(
        const String refreshToken,
        const String clientId,
        const String clientSecret) {

    _refreshToken = refreshToken;
    _clientId = clientId;
    _clientSecret = clientSecret;
    
    _syncTime();

    return _refreshAccessToken();
}

bool EveryAwareAccess::_refreshAccessToken() {
    
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

void EveryAwareClient::_handleWebsocketEvent(WStype_t type, uint8_t * payload, size_t length) {

    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;

        case WStype_CONNECTED:
            Serial.printf("[WSc] Connected to url: %s\n",  payload);
            break;

        case WStype_TEXT: {

                String text = (char*) payload;

                Serial.printf("[WSc] get text: %s\n", payload);

                if (payload[0] == 'h') {
                    
                    Serial.println("Heartbeat!");
                    
                } else if (payload[0] == 'o') {
                    
                    String msg = "[\"CONNECT\\naccept-version:1.1,1.0\\nauthorization:Bearer ";
                    msg += _accessToken;
                    msg += "\\nheart-beat:10000,10000\\n\\n\\u0000\"]";
                    _webSocket.sendTXT(msg);

                } else if (text.startsWith("a[\"CONNECTED")) {

                    // subscribe to message queue (data comes in through this channel)
                    String msg = "[\"SUBSCRIBE\\nid:sub-0\\ndestination:/user/queue/messages\\n\\n\\u0000\"]";
                    _webSocket.sendTXT(msg);
                    delay(1000);

                    // subscribe to ack queue (we get an ACK after we request data updates from a source)
                    msg = "[\"SUBSCRIBE\\nid:sub-1\\ndestination:/user/queue/ack\\n\\n\\u0000\"]";
                    _webSocket.sendTXT(msg);
                    delay(1000);

                    // send request to send data updates from a source
//                    msg = "[\"SEND\\ndestination:/app/channelpoint/register\\n\\n{\\\"feed\\\":\\\"";
//                    msg += eva_feed;
//                    msg += "\\\",\\\"sourceId\\\":\\\"";
//                    msg += eva_sourceid;
//                    msg += "\\\",\\\"channels\\\":[\\\"ledOn\\\"],\\\"requestId\\\":\\\"randomConstant:x12gazs81\\\"}\\u0000\"]";
//                    webSocket.sendTXT(msg);
                    delay(1000);
                    
                } else if (text.startsWith("a[\"MESSAGE") && text.indexOf("destination:/user/queue/messages") >= 0) {

                    Serial.print("Reveived data update: ");
                    int index = text.indexOf("ledOn");
                    if (index >= 0) {

                        int offset = 21;
                        Serial.print("ledOn=");
                        Serial.println(text.substring(index + offset, index + offset + 4));
                        bool ledOn = (text[index + offset] == 't'); // extracts first letter of true/false
//                        if (ledOn) {
//                            digitalWrite(led, LOW); // low turns it on ... don't know why this is reversed
//                        } else {
//                            digitalWrite(led, HIGH);
//                        }
                        
                    } else {
                        Serial.println("no 'ledOn' channel available.");
                    }
                    
                }

                // send message to server
                // webSocket.sendTXT("message here");
                break;
            }
        case WStype_BIN:
            Serial.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
        default:
            Serial.printf("[WSc] something: %s\n", payload);
            break;
    }
}

void EveryAwareClient::_setupWebsocket() {

    WebSocketsClient webSocket;
    _webSocket = webSocket;
    
    // initialize websocket connection
    int randomNumber = random(0, 999);
    String socketUrl = "/socketentry/";
    socketUrl += random(0, 999);
    socketUrl += "/";
    socketUrl += random(0, 999999);
    socketUrl += "/websocket";
    webSocket.begin("cs.everyaware.eu", 80, socketUrl);
    webSocket.setExtraHeaders();
    //  webSocket.setAuthorization(const_cast<char*>(("Bearer " + _accessToken).c_str()));

    webSocket.onEvent( [this](WStype_t type, uint8_t * payload, size_t length) { 
        _handleWebsocketEvent(type, payload, length); 
    });
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

