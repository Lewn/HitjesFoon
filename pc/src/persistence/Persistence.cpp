#include "Persistence.h"

Persistence::Persistence(WServer &server) : server(server) {
    intData.connect(boost::bind(&Persistence::postOnChange, this, _1));
    stringData.connect(boost::bind(&Persistence::postOnChange, this, _1));
    intVectorData.connect(boost::bind(&Persistence::postOnChange, this, _1));
    stringVectorData.connect(boost::bind(&Persistence::postOnChange, this, _1));
    hitjeData.connect(boost::bind(&Persistence::postOnChange, this, _1));
}

Persistence::~Persistence() {

}

void Persistence::onChangeCallback(const PersistenceCallback &callback) {
    WApplication *app = WApplication::instance();
    lock_guard<mutex> guard(callbackMutex);
    callbacks[app->sessionId()].push_back(callback);
}

void Persistence::postOnChange(const string &key) {
    // TODO prevent update more than x changes per time
    // or other solution, however prevent web interface crashes
    WApplication *app = WApplication::instance();
    for (auto &appCallbacks : callbacks) {
        string sessionId = appCallbacks.first;
        for (auto &callback : appCallbacks.second) {
            if (app && app->sessionId() == sessionId) {
                callback(key);
            } else {
                // TODO use fallback function to remove dead sessions from list
                server.post(sessionId, boost::bind(callback, key));
            }
        }
    }
}


PersistenceData<int> &Persistence::getIntData() {
    return intData;
}

PersistenceData<string> &Persistence::getStringData() {
    return stringData;
}

PersistenceData<vector<int>> &Persistence::getIntVectorData() {
    return intVectorData;
}

PersistenceData<vector<string>> &Persistence::getStringVectorData() {
    return stringVectorData;
}

PersistenceData<Hitje> &Persistence::getHitjeData() {
    return hitjeData;
}
