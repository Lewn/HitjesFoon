#ifndef PERSISTENCEDATA_H
#define PERSISTENCEDATA_H

#include "Structs.h"

#include <mutex>
#include <unordered_map>

#include <boost/signals2.hpp>

using namespace std;
using namespace boost;

template <typename Type> class PersistenceData {
public:
    typedef boost::signals2::signal<void (const string &key)> changeSignal;
    typedef boost::function<void (Type &type)> Callback;

    PersistenceData();
    virtual ~PersistenceData();

    boost::signals2::connection connect(const changeSignal::slot_type &subscriber);

    void initVal(const char *key, const Type &val);
    void initVal(const string &key, const Type &val);
    const Type &getVal(const char *key);
    const Type &getVal(const string &key);
    void setVal(const char *key, const Type &val);
    void setVal(const string &key, const Type &val);
    void addVal(const char *key, Type add);
    void addVal(const string &key, Type add);
    void manipulateVal(const char *key, const Callback &callback);
    void manipulateVal(const string &key, const Callback &callback);

protected:
    mutex typeMutex;
    changeSignal sig;

    unordered_map<string, Type> vals;
private:
};

template class PersistenceData<int>;
template class PersistenceData<string>;
template class PersistenceData<vector<int>>;
template class PersistenceData<vector<string>>;
template class PersistenceData<Hitje>;

#endif // PERSISTENCEDATA_H
