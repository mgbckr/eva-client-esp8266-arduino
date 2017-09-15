#ifndef EVA_H
#define EVA_H

#define ARDUINOJSON_USE_LONG_LONG 1 // we need to store long long

#include <ArduinoJson.h>
#include <time.h>

#define EVA_DEBUG 1
#define EVA_WILDCARD "*"
#define EVA_SSH_FINGERPRINT "29 41 9A 40 AF AF C6 70 65 E3 33 5D D2 B5 22 76 A0 07 13 6C"
#define EVA_BASE_URL "https://cs.everyaware.eu"

 /**
 * TODO: maybe add function to retrieve arbitrary ranges of data, not just the latest
 */
class EveryAwareAccess {
    
    public:

        EveryAwareAccess(const String baseUrl = EVA_BASE_URL);
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

    private:
        String _baseUrl;
        
        String _clientId;
        String _clientSecret;
        
        String _refreshToken;
        unsigned long _refreshTokenExpiresIn;
        unsigned long _refreshTokenReferenceTime;
        
        String _accessToken;

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
