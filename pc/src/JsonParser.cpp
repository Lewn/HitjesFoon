#include "JsonParser.h"

JsonParser::JsonParser(GUI &gui) : gui(gui) {
}

JsonParser::~JsonParser() {
}

void JsonParser::parse(const char *jsonString) {
    root.Parse(jsonString);
    if (!root.IsObject()) {
        // report to the user the failure and their locations in the document.
        gui.printlevel(LERROR, "JSON:\n%s\n", jsonString);
        throw "Failed to parse configuration";
    }
}

int JsonParser::getTotalResults() {
    return root["pageInfo"]["totalResults"].GetInt();
}

vector<string> JsonParser::getVideoIds() {
    Value &items = root["items"];
    vector<string> videoIds;
    for (Value::ConstValueIterator itr = items.Begin(); itr != items.End(); ++itr) {
        const Value &videoId = (*itr)["id"]["videoId"];
        if (!videoId.IsNull() && videoId.IsString()) {
            videoIds.push_back(videoId.GetString());
        } else {
            throw "Invalid string videoId from json";
        }
    }
    return videoIds;
}

vector<string> JsonParser::getVideoTitles() {
    Value &items = root["items"];
    vector<string> videoTitles;
    for (Value::ConstValueIterator itr = items.Begin(); itr != items.End(); ++itr) {
        const Value &videoTitle = (*itr)["snippet"]["title"];
        if (!videoTitle.IsNull() && videoTitle.IsString()) {
            videoTitles.push_back(videoTitle.GetString());
        } else {
            throw "Invalid string title  from json";
        }
    }
    return videoTitles;
}
