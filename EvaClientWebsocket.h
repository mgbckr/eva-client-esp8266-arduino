#include "EvaClientHttp.h"
#include <WebSocketsClient.h>

class EvaClientWebsocket : public EvaClientHttp {

    public:

        void connectWebsocket();
        void loop();
        WebSocketsClient _webSocket;
    
    protected:
    
        void _handleWebsocketEvent(WStype_t type, uint8_t * payload, size_t length);
        void _connectWebsocket();
};

