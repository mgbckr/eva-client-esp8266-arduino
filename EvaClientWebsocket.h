#include "EvaClientHttp.h"
#include <WebSocketsClient.h>

class EvaClientWebsocket : public EvaClientHttp {
    
    protected:
    
        WebSocketsClient _webSocket;
        void _handleWebsocketEvent(WStype_t type, uint8_t * payload, size_t length);
        void _setupWebsocket();
};

