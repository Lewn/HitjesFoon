#ifndef JSONPARSER_H
#define JSONPARSER_H

#include "rapidjson/document.h"
#include "Tools.h"
#include <stdio.h>
#include <vector>

using namespace std;
using namespace rapidjson;

class JsonParser {
public:
    JsonParser();
    virtual ~JsonParser();

    void parse(const char *jsonString);
    int getTotalResults();
    vector<string> getVideoIds();
    vector<string> getVideoTitles();
protected:
    Document *root;
private:
};

#endif // JSONPARSER_H
