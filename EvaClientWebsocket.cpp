#include "EvaClientWebsocket.h"

void EvaClientWebsocket::_handleWebsocketEvent(WStype_t type, uint8_t * payload, size_t length) {

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

void EvaClientWebsocket::_setupWebsocket() {

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
