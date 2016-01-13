#include "JsonParser.h"

JsonParser::JsonParser() {
    root = NULL;
}

JsonParser::~JsonParser() {
    SAFE_DELETE(root);
}

void JsonParser::parse(const char* jsonString) {
    root = new Document;
    root->Parse(jsonString);
    bool parsingSuccessful = true;
    if (!parsingSuccessful) {
        // report to the user the failure and their locations in the document.
        throw "Failed to parse configuration";
    }
}

int JsonParser::getTotalResults() {
    return (*root)["pageInfo"]["totalResults"].GetInt();
}

vector<string> JsonParser::getVideoIds() {
    Value &items = (*root)["items"];
    vector<string> videoIds;
    for (Value::ConstValueIterator itr = items.Begin(); itr != items.End(); ++itr) {
        videoIds.push_back((*itr)["id"]["videoId"].GetString());
    }
    return videoIds;
}

vector<string> JsonParser::getVideoTitles() {
    Value &items = (*root)["items"];
    vector<string> videoTitles;
    for (Value::ConstValueIterator itr = items.Begin(); itr != items.End(); ++itr) {
        videoTitles.push_back((*itr)["snippet"]["title"].GetString());
    }
    return videoTitles;
}
