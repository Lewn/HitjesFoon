#ifndef JSONPARSER_H
#define JSONPARSER_H

#include "json/json.h"
#include "Tools.h"
#include <stdio.h>
#include <vector>

using namespace std;

class JsonParser {
public:
    JsonParser();
    virtual ~JsonParser();

    void parse(const char* jsonString);
    int getTotalResults();
    vector<string> getVideoIds();
protected:
    Json::Value root;
private:
};

#endif // JSONPARSER_H
