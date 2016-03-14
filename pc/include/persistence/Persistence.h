#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <mutex>
#include <unordered_map>
#include <vector>

#include <Wt/WServer>

#include "structs.h"
#include "Tools.h"
#include "persistence/PersistenceData.h"

using namespace std;
using namespace boost;
using namespace Wt;

class Persistence {
public:
    typedef boost::function<void (const string &type)> PersistenceCallback;

    Persistence(WServer &server);
    virtual ~Persistence();
    Persistence(const Persistence&) = delete;
    Persistence &operator=(const Persistence&) = delete;

    void onChangeCallback(const PersistenceCallback &callback);

    PersistenceData<int> &getIntData();
    PersistenceData<string> &getStringData();
    PersistenceData<vector<int>> &getIntVectorData();
    PersistenceData<vector<string>> &getStringVectorData();
    PersistenceData<std::shared_ptr<DownloadState>> &getDownloadStateData();

protected:
    WServer &server;
    mutex callbackMutex;

    unordered_map<string, vector<PersistenceCallback>> callbacks;
    PersistenceData<int> intData;
    PersistenceData<string> stringData;
    PersistenceData<vector<int>> intVectorData;
    PersistenceData<vector<string>> stringVectorData;
    PersistenceData<std::shared_ptr<DownloadState>> downloadStateData;

    void postOnChange(const string &key);
private:
};

#endif // PERSISTENCE_H
