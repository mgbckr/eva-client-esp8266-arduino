#include "time.h"
#include "EvaRootCert.h"
#include "EvaClientCore.h"

EvaClientCore::EvaClientCore(const char *  host, const int port) {
    _init(host, port, evaRootCert, evaRootCertLen);
}

EvaClientCore::EvaClientCore(const char * host, const int port, const unsigned char caCert[], const unsigned int caCertLen) {
    _init(host, port, caCert, caCertLen);
}

EvaClientCore::~EvaClientCore() {
    // nothing to do
}

/**
 * To calculate the object size see:
 * https://bblanchon.github.io/ArduinoJson/assistant/ 
 */
JsonObject* EvaClientCore::getJson(String relativeUrl, size_t objectSize) {
    
    #if (EVA_DEBUG == 1)
        Serial.print("Requesting JSON from: "); Serial.print(_host); Serial.println(relativeUrl);
    #endif

    // connect to remote server
    Serial.print("Connecting to: "); Serial.println(_host);
    if (!_wifiClient.connect(_host, _port)) {
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


/** 
 * Synchronize time useing SNTP. This is necessary to verify that
 * the TLS certificates offered by the server are currently valid.
 * 
 * TODO: check this
 */
void EvaClientCore::_syncTime() {

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

void EvaClientCore::_init(const char * host, const int port, const unsigned char caCert[], const unsigned int caCertLen) {
    _host = host;
    _port = port;
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

