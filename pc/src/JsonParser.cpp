#include "JsonParser.h"

JsonParser::JsonParser() {}

JsonParser::~JsonParser() {}

void JsonParser::parse(const char* jsonString) {
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(jsonString, root);
    if (!parsingSuccessful) {
        // report to the user the failure and their locations in the document.
        throw "Failed to parse configuration";
    }
}

int JsonParser::getTotalResults() {
    return root["pageInfo"]["totalResults"].asInt();
}

vector<string> JsonParser::getVideoIds() {
    Json::Value items = root["items"];
    vector<string> videoIds;
    for (int index = 0; index < items.size(); ++index) {
        videoIds.push_back(items[index]["id"]["videoId"].asString());
    }
    return videoIds;
}

vector<string> JsonParser::getVideoTitles() {
    Json::Value items = root["items"];
    vector<string> videoTitles;
    for (int index = 0; index < items.size(); ++index) {
        videoTitles.push_back(items[index]["snippet"]["title"].asString());
    }
    return videoTitles;
}
