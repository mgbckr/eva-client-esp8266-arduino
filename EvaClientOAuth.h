#include "EvaClientCore.h"

class EvaClientOAuth: public EvaClientCore {

    public:
    
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
        
        String _clientId;
        String _clientSecret;
        
        String _refreshToken;
        unsigned long _refreshTokenExpiresIn;
        unsigned long _refreshTokenReferenceTime;
        
        String _accessToken;

        bool _refreshAccessToken();
        bool _handleOauthResponse(JsonObject* jsonPtr);
};
