#include "persistence/PersistenceData.h"

template <typename Type> PersistenceData<Type>::PersistenceData() {
}

template <typename Type> PersistenceData<Type>::~PersistenceData() {
}

template <typename Type> signals2::connection PersistenceData<Type>::connect(const changeSignal::slot_type &subscriber) {
    return sig.connect(subscriber);
}


template <typename Type> mutex &PersistenceData<Type>::findMutex(const string &key, bool create) {
    // Only allow one mutex to be accessed or created at a time
    lock_guard<mutex> guard(superMutex);
    typename unordered_map<string, mutex>::iterator keymutex = mutexes.find(key);
    if (keymutex == mutexes.end()) {
        // Couldn't find
        if (!create) {
            throw "PersistenceData key not found";
        }
        // Create a new mutex and return it
        return mutexes[key];
    }
    return keymutex->second;
}


template <typename Type> void PersistenceData<Type>::initVal(const char *key, const Type &val) {
    initVal(string(key), val);
}

template <typename Type> void PersistenceData<Type>::initVal(const string &key, const Type &val) {
    mutex &valMutex = findMutex(key, true);
    valMutex.lock();
    if (vals.insert({key, val}).second) {
        // We actually inserted, signal connected listeners
        valMutex.unlock();
        sig(key);
    } else {
        // No new value inserted
        valMutex.unlock();
    }
}

template <typename Type> const Type &PersistenceData<Type>::getVal(const char *key) {
    return getVal(string(key));
}

template <typename Type> const Type &PersistenceData<Type>::getVal(const string &key) {
    mutex &valMutex = findMutex(key);
    lock_guard<mutex> guard(valMutex);
    // If we could get the mutex, the key must exists
    // Simply return the value at this location
    return vals.at(key);
}

template <typename Type> void PersistenceData<Type>::setVal(const char *key, const Type &val) {
    setVal(string(key), val);
}

template <typename Type> void PersistenceData<Type>::setVal(const string &key, const Type &val) {
    bool change = false;
    // Access the element, or create a new mutex if not exists yet
    mutex &valMutex = findMutex(key, true);
    valMutex.lock();
    // Make sure no element is inside, as it stops insert
    auto insval = vals.insert({key, val});
    if (!insval.second) {
        // Key already existed, overwrite if different
        if (insval.first->second != val) {
            insval.first->second = val;
            change = true;
        }
    } else {
        // Key didn't exist but is inserted now
        change = true;
    }
    valMutex.unlock();
    if (change) {
        // We had a change in value, signal it
        sig(key);
    }
}

template <typename Type> void PersistenceData<Type>::addVal(const char *key, Type add) {
    addVal(string(key), add);
}

template <> void PersistenceData<Hitje>::addVal(const char *key, Hitje add) = delete;

template <typename Type> void PersistenceData<Type>::addVal(const string &key, Type add) {
    mutex &valMutex = findMutex(key);
    valMutex.lock();
    typename unordered_map<string, Type>::iterator keyval = vals.find(key);
    if (keyval == vals.end()) {
        // Mutex existed, thus this shouldn't happen?
        valMutex.unlock();
        throw "Inconsistency between value and mutex";
    }
    keyval->second += add;
    valMutex.unlock();
    sig(key);
}

template <> void PersistenceData<vector<int>>::addVal(const string &key, vector<int> add) {
    mutex &valMutex = findMutex(key);
    valMutex.lock();
    unordered_map<string, vector<int>>::iterator keyval = vals.find(key);
    if (keyval == vals.end()) {
        // Mutex existed, thus this shouldn't happen?
        valMutex.unlock();
        throw "Inconsistency between value and mutex";
    }
    keyval->second.insert(keyval->second.end(), add.begin(), add.end());
    valMutex.unlock();
    sig(key);
}

template <> void PersistenceData<vector<string>>::addVal(const string &key, vector<string> add) {
    mutex &valMutex = findMutex(key);
    valMutex.lock();
    unordered_map<string, vector<string>>::iterator keyval = vals.find(key);
    if (keyval == vals.end()) {
        // Mutex existed, thus this shouldn't happen?
        valMutex.unlock();
        throw "Inconsistency between value and mutex";
    }
    keyval->second.insert(keyval->second.end(), add.begin(), add.end());
    valMutex.unlock();
    sig(key);
}

template <> void PersistenceData<Hitje>::addVal(const string &key, Hitje add) = delete;

template <typename Type> void PersistenceData<Type>::manipulateVal(const char *key, const Callback &callback) {
    manipulateVal(string(key), callback);
}

template <typename Type> void PersistenceData<Type>::manipulateVal(const string &key, const Callback &callback) {
    mutex &valMutex = findMutex(key);
    valMutex.lock();
    typename unordered_map<string, Type>::iterator keyval = vals.find(key);
    if (keyval != vals.end()) {
        // Call user function
        callback(keyval->second);
        valMutex.unlock();
        sig(key);
    } else {
        // Mutex existed, thus this shouldn't happen?
        valMutex.unlock();
        throw "Inconsistency between value and mutex";
    }
}
