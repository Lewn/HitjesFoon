#include "PersistenceData.h"

template <typename Type> PersistenceData<Type>::PersistenceData() {
}

template <typename Type> PersistenceData<Type>::~PersistenceData() {
}

template <typename Type> signals2::connection PersistenceData<Type>::connect(const changeSignal::slot_type &subscriber) {
    return sig.connect(subscriber);
}


template <typename Type> void PersistenceData<Type>::initVal(const char *key, const Type &val) {
    initVal(string(key), val);
}

template <typename Type> void PersistenceData<Type>::initVal(const string &key, const Type &val) {
    typeMutex.lock();
    typename unordered_map<string, Type>::const_iterator keyval = vals.find(key);
    if (keyval == vals.end()) {
        vals[key] = val;
        typeMutex.unlock();
        sig(key);
    } else {
        typeMutex.unlock();
    }
}

template <typename Type> const Type &PersistenceData<Type>::getVal(const char *key) {
    return getVal(string(key));
}

template <typename Type> const Type &PersistenceData<Type>::getVal(const string &key) {
    lock_guard<mutex> guard(typeMutex);
    typename unordered_map<string, Type>::const_iterator keyval = vals.find(key);
    if (keyval == vals.end()) {
        throw "PersistenceData key not found";
    }
    return keyval->second;
}

template <typename Type> void PersistenceData<Type>::setVal(const char *key, const Type &val) {
    setVal(string(key), val);
}

template <typename Type> void PersistenceData<Type>::setVal(const string &key, const Type &val) {
    typeMutex.lock();
    vals[key] = val;
    typeMutex.unlock();
    sig(key);
}

template <typename Type> void PersistenceData<Type>::addVal(const char *key, Type add) {
    addVal(string(key), add);
}

template <> void PersistenceData<std::shared_ptr<DownloadState>>::addVal(const char *key, std::shared_ptr<DownloadState> add) = delete;

template <typename Type> void PersistenceData<Type>::addVal(const string &key, Type add) {
    typeMutex.lock();
    typename unordered_map<string, Type>::iterator keyval = vals.find(key);
    if (keyval == vals.end()) {
        typeMutex.unlock();
        throw "PersistenceData key not found";
    }
    keyval->second += add;
    typeMutex.unlock();
    sig(key);
}

template <> void PersistenceData<vector<int>>::addVal(const string &key, vector<int> add) {
    typeMutex.lock();
    unordered_map<string, vector<int>>::iterator keyval = vals.find(key);
    if (keyval == vals.end()) {
        typeMutex.unlock();
        throw "PersistenceData key not found";
    }
    keyval->second.insert(keyval->second.end(), add.begin(), add.end());
    typeMutex.unlock();
    sig(key);
}

template <> void PersistenceData<vector<string>>::addVal(const string &key, vector<string> add) {
    typeMutex.lock();
    unordered_map<string, vector<string>>::iterator keyval = vals.find(key);
    if (keyval == vals.end()) {
        typeMutex.unlock();
        throw "PersistenceData key not found";
    }
    keyval->second.insert(keyval->second.end(), add.begin(), add.end());
    typeMutex.unlock();
    sig(key);
}

template <> void PersistenceData<std::shared_ptr<DownloadState>>::addVal(const string &key, std::shared_ptr<DownloadState> add) = delete;

template <typename Type> void PersistenceData<Type>::manipulateVal(const char *key, const Callback &callback) {
    manipulateVal(string(key), callback);
}

template <typename Type> void PersistenceData<Type>::manipulateVal(const string &key, const Callback &callback) {
    typeMutex.lock();
    typename unordered_map<string, Type>::iterator keyval = vals.find(key);
    if (keyval != vals.end()) {
        callback(keyval->second);
        typeMutex.unlock();
        sig(key);
    } else {
        typeMutex.unlock();
        throw "PersistenceData key not found";
    }
}
