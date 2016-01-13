#include "Config.h"


extern PRINT_LEVEL msglevel;

Config::Config(const char *configPath) {
    char buffer[65536];

    FILE *configFile = fopen(configPath, "r");
    if (!configFile) {
        throw "Couldn't open config file";
    }

    config = new Document;
    FileReadStream is(configFile, buffer, sizeof(buffer));
    config->ParseStream<0, UTF8<>, FileReadStream>(is);

    fclose(configFile);

    if (!config->IsObject()) {
        throw "The config file is not a valid JSON object";
    }

    Value::MemberIterator msglevelMember = config->FindMember("msglevel");
    if (msglevelMember == config->MemberEnd()) {
        printlevel(LDEBUG, "No msglevel found, using default (%d)", msglevel);
    } else {
        msglevel = (PRINT_LEVEL)msglevelMember->value.GetInt();
    }
}

Config::~Config() {
    SAFE_DELETE(config);
}


Value::ConstValueIterator Config::getHitjesConfigIterator() {
    if (hitjesConfigIterator == NULL) {
        Value::MemberIterator hitjesMember = config->FindMember("hitjes");
        if (hitjesMember == config->MemberEnd()) {
            throw "Couldn't find member 'hitjes' in the config file";
        }
        if (!hitjesMember->value.IsArray()) {
            throw "Hitjes config should be an array";
        }
        hitjesConfig = &(hitjesMember->value);
        hitjesConfigIterator = hitjesConfig->Begin();
    }
    if (hitjesConfigIterator == hitjesConfig->End()) {
        throw "No hitjes config elements left";
    }
    return hitjesConfigIterator;
}

bool Config::nextHitjesConfig() {
    Value::ConstValueIterator itr = getHitjesConfigIterator();
    if (itr >= hitjesConfig->End()) {
        return false;
    }
    hitjesConfigIterator++;
    return hitjesConfigIterator != hitjesConfig->End();
}

string Config::getHitjesList() {
    Value::ConstValueIterator hitjesConfig = getHitjesConfigIterator();
    string list = string((*hitjesConfig)["list"].GetString());
    return trim(list);
}

string Config::getHitjesPath() {
    Value::ConstValueIterator hitjesConfig = getHitjesConfigIterator();
    string path = string((*hitjesConfig)["path"].GetString());
    return trim(path);
}



Value::ConstValueIterator Config::getSoundConfigIterator() {
    if (soundConfigIterator == NULL) {
        Value::MemberIterator soundMember = config->FindMember("soundinterface");
        if (soundMember == config->MemberEnd()) {
            throw "Couldn't find member 'soundinterface' in the config file";
        }
        if (!soundMember->value.IsArray()) {
            throw "Soundinterface config should be an array";
        }
        soundInterface = &(soundMember->value);
        soundConfigIterator = soundInterface->Begin();
    }
    if (soundConfigIterator == soundInterface->End()) {
        throw "No soundinterface elements left";
    }
    return soundConfigIterator;
}

bool Config::nextAudioDevices() {
    Value::ConstValueIterator itr = getSoundConfigIterator();
    if (itr >= soundInterface->End()) {
        return false;
    }
    soundConfigIterator++;
    return soundConfigIterator != soundInterface->End();
}

string Config::getVLCPhoneDevice() {
    Value::ConstValueIterator soundConfig = getSoundConfigIterator();
    string phone = (*soundConfig)["phone"].GetString();
    return trim(phone);
}

string Config::getVLCSpeakerDevice() {
    Value::ConstValueIterator soundConfig = getSoundConfigIterator();
    string speaker = (*soundConfig)["speaker"].GetString();
    return trim(speaker);
}


string Config::getConfigMenuPath() {
    Value::MemberIterator configmenuMember = config->FindMember("configmenu");
    if (configmenuMember == config->MemberEnd()) {
        throw "Couldn't find member 'configmenu' in the config file";
    }
    string configMenu = string(configmenuMember->value.GetString());
    return trim(configMenu);
}
