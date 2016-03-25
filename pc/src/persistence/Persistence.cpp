#include "persistence/Persistence.h"

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
    lock_guard<mutex> guard(callbackMutex);
    callbacks[getSessionId()].push_back(callback);
}


string Persistence::getSessionId() {
    if (isServer()) {
        return "server";
    }
    return WApplication::instance()->sessionId();
}

bool Persistence::isServer() {
    return WApplication::instance() == NULL;
}

void Persistence::postOnChange(const string &key) {
    // TODO prevent update more than x changes per time
    // or other solution, however prevent web interface crashes
    WApplication *app = WApplication::instance();
    for (auto &appCallbacks : callbacks) {
        string sessionId = appCallbacks.first;
        for (auto &callback : appCallbacks.second) {
            // TODO run server callbacks in separate thread instead of user thread?
            if (sessionId == "server" || (app && app->sessionId() == sessionId)) {
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
