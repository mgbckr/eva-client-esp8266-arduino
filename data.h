#define ARDUINOJSON_USE_LONG_LONG 1 // we need to store long long

#include <ArduinoJson.h>


class EvaDataArrayBuilder;
class EvaDataPointBuilder;
class EvaChannelBuilder;

/**
EvaDataArrayBuilder test(1234);
test.
    addDatapoint(123123123).
        addChannelValue("tag", "BLUBB").
        addChannel("gps").
            addComponent("lon", 123.12).
            addComponent("lat", 11.11).
            finalizeChannel().
        finalizeDataPoint().
    finalizeDataArray();
*/

class EvaDataArrayBuilder {
    
    public:
        EvaDataArrayBuilder(size_t bufferSize);
    
        /** Unix timestamp is in MILLI-seconds! */
        EvaDataPointBuilder & addDataPoint(const long long timestamp);
        JsonArray & finalizeDataArray();

    private:
        DynamicJsonBuffer * _jsonBuffer;
        JsonArray * _dataArray;
};


class EvaDataPointBuilder {
    
    public:    
        EvaDataPointBuilder(EvaDataArrayBuilder * _arrayBuilder, JsonObject * channels);
        
        EvaChannelBuilder & addChannel(const String name);
        EvaDataPointBuilder & addChannelValue(const String name, const double value);
        EvaDataPointBuilder & addChannelValue(const String name, const String value);
    
        EvaDataArrayBuilder & finalizeDataPoint();

    private:
        EvaDataArrayBuilder * _arrayBuilder;
        JsonObject * _channels;
};

class EvaChannelBuilder {
    
    public:
        EvaChannelBuilder(EvaDataPointBuilder * _pointBuilder, JsonObject * channel);
        
        EvaChannelBuilder& addComponent(const String name, const double value);
        EvaChannelBuilder& addComponent(const String name, const String value);
        
        EvaDataPointBuilder & finalizeChannel();

    private:
        EvaDataPointBuilder * _pointBuilder;
        JsonObject * _channel;
};

