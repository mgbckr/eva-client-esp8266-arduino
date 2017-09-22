#ifndef EVA_H
#define EVA_H

#define ARDUINOJSON_USE_LONG_LONG 1 // we need to store long long

#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

#define EVA_DEBUG 1
#define EVA_WILDCARD "*"
#define EVA_HOST "cs.everyaware.eu"

 /**
 * TODO: maybe add function to retrieve arbitrary ranges of data, not just the latest
 */
class EveryAwareAccess {
    
    public:

        EveryAwareAccess(const char * host = EVA_HOST);
        EveryAwareAccess(const char * host, const unsigned char caCert[], const unsigned int caCertLen);
        ~EveryAwareAccess(void);


        JsonObject* getJson(String relativeUrl, size_t objectSize);

        /**
         * Directly use a refresh token to initialize the EveryAware client.
         * You can get it with this class by initializing via username and password
         * and calling the getRefreshToken method.
         * Or, e.g., by using CURL (replace CLIENT):
         *  curl "https://cs.everyaware.eu/oauth/token?grant_type=password&client_id=CLIENT_ID&client_secret=CLIENT_SECRET&username=USERNAME&password=PASSWORD"
         */
        bool init(
            const String refreshToken,
            const String clientId,
            const String clientSecret);

        /**
         * If no request token is available we can initialize the client
         * using a username, password, client id and client secret.
         */
        bool init(
            const String username, 
            const String password,
            const String clientId,
            const String clientSecret);

        String getRefreshToken();
        unsigned long getRefreshTokenExpiresIn(bool update = false);
        
        String getAccessToken();

    protected:
        const char * _host;

        const unsigned char * _caCert;
        unsigned int _caCertLen;

        WiFiClientSecure _wifiClient;
        
        String _clientId;
        String _clientSecret;
        
        String _refreshToken;
        unsigned long _refreshTokenExpiresIn;
        unsigned long _refreshTokenReferenceTime;
        
        String _accessToken;

        void _init(
            const char * host, 
            const unsigned char caCert[], 
            const unsigned int caCertLen);

        bool _refreshAccessToken();
        bool _handleOauthResponse(JsonObject* jsonPtr);

        void _syncTime();

        
};

class EveryAwareClient: public EveryAwareAccess {

    public:
    
        void postData(
            const String feed, 
            const String source, 
            const String datapoints);
            
        void postData(
            const String feed, 
            const String source, 
            JsonArray& datapoints);

    private:
    
        WebSocketsClient _webSocket;
        void _handleWebsocketEvent(WStype_t type, uint8_t * payload, size_t length);
        void _setupWebsocket();

/**

        JsonObject getLatestData(
            const char * feed, 
            const char * source, 
            const char * channel = EVA_WILDCARD, 
            const char * components = EVA_WILDCARD);

        JsonObject getData(
            const char * feed, 
            const char * source, 
            const char * channel = EVA_WILDCARD, 
            const char * components = EVA_WILDCARD,
            const long from,
            const long until);
        
        void addDataListener(
            void (*onData)(data), 
            const char * feed, 
            const char * source, 
            const char * channel = EVA_WILDCARD);



        void listenerLoop(void);
*/
};

#endif
