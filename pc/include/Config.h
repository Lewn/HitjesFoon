#ifndef CONFIG_H
#define CONFIG_H

#include "Tools.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace rapidjson;

class Config {
public:
    Config(const char *configPath);
    virtual ~Config();

    bool nextHitjesConfig();
    string getHitjesList();
    string getHitjesPath();

    bool nextAudioDevices();
    string getVLCPhoneDevice();
    string getVLCSpeakerDevice();

    string getConfigMenuPath();
protected:
    Document *config = NULL;

    const Value *hitjesConfig = NULL;
    Value::ConstValueIterator hitjesConfigIterator = NULL;
    const Value *soundInterface = NULL;
    Value::ConstValueIterator soundConfigIterator = NULL;

    Value::ConstValueIterator getHitjesConfigIterator();
    Value::ConstValueIterator getSoundConfigIterator();
private:
};

#endif // CONFIG_H
