#define ARDUINOJSON_USE_LONG_LONG 1 // we need to store long long

#define EVA_DEBUG 1
#define EVA_HOST "cs.everyaware.eu"
#define EVA_PORT 443

#include "Arduino.h"
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

class EvaClientCore {
    
    public:

        EvaClientCore(const char * host = EVA_HOST, int port = EVA_PORT);
        EvaClientCore(const char * host, const int port, const unsigned char caCert[], const unsigned int caCertLen);
        ~EvaClientCore(void);

        JsonObject* getJson(String relativeUrl, size_t objectSize);

    protected:
    
        const char * _host;
        int _port;

        const unsigned char * _caCert;
        unsigned int _caCertLen;

        WiFiClientSecure _wifiClient;
        
        void _syncTime();
             
        void _init(
            const char * host, 
            const int port, 
            const unsigned char caCert[], 
            const unsigned int caCertLen);

};
