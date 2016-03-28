#ifndef HTTPTRANSFER_H
#define HTTPTRANSFER_H

#include <chrono>
#include <thread>
#include <memory>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>

#include "gui/GUI.h"
#include "Tools.h"

using namespace std;

class HTTPTransfer {
public:
    HTTPTransfer(GUI &gui);
    ~HTTPTransfer();

    string get(const char *url);
    void get(const char *url, const char *filename);
    string escape(const char *toEscape);
protected:
    GUI &gui;
private:
};

#endif // HTTPTRANSFER_H
