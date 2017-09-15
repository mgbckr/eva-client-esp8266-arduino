#include "data.h"



/**
 * Data array builder.
 */

EvaDataArrayBuilder::EvaDataArrayBuilder(size_t bufferSize) {
    _jsonBuffer = new DynamicJsonBuffer(bufferSize);
    _dataArray = &_jsonBuffer->createArray();
}

EvaDataPointBuilder & EvaDataArrayBuilder::addDataPoint(const long long timestamp) {
    
    JsonObject & dataPoint = _jsonBuffer->createObject();
    dataPoint["timestamp"] = timestamp;
    JsonObject & channels = dataPoint.createNestedObject("channels");
    
    _dataArray->add(dataPoint);
    
    return *new EvaDataPointBuilder(this, &channels);
}

JsonArray & EvaDataArrayBuilder::finalizeDataArray() {
    return *_dataArray;
}



/**
 * Data point builder.
 */

EvaDataPointBuilder::EvaDataPointBuilder(EvaDataArrayBuilder * arrayBuilder, JsonObject * channels) {
    _arrayBuilder = arrayBuilder;
    _channels = channels;
}

EvaChannelBuilder & EvaDataPointBuilder::addChannel(const String name) {
    JsonObject & channel = _channels->createNestedObject(name);
    return *new EvaChannelBuilder(this, &channel);
}

EvaDataPointBuilder & EvaDataPointBuilder::addChannelValue(const String name, const double value) {
    JsonObject & channel = _channels->createNestedObject(name);
    channel["value"] = value;
    return *this;
}

EvaDataPointBuilder & EvaDataPointBuilder::addChannelValue(const String name, const String value) {
    JsonObject & channel = _channels->createNestedObject(name);
    channel["value"] = value;
    return *this;
}

EvaDataArrayBuilder & EvaDataPointBuilder::finalizeDataPoint() {
    return *_arrayBuilder;
}



/**
 * Channel builder.
 */

EvaChannelBuilder::EvaChannelBuilder(EvaDataPointBuilder * pointBuilder, JsonObject * channel) {
    _pointBuilder = pointBuilder;
    _channel = channel;
}

EvaChannelBuilder & EvaChannelBuilder::addComponent(const String name, const double value) {
    JsonObject & component = _channel->createNestedObject(name);
    component["value"] = value;
    return *this;
}

EvaChannelBuilder & EvaChannelBuilder::addComponent(const String name, const String value) {
    JsonObject & component = _channel->createNestedObject(name);
    component["value"] = value;
    return *this;
}

EvaDataPointBuilder & EvaChannelBuilder::finalizeChannel() {
    return *_pointBuilder;
}
