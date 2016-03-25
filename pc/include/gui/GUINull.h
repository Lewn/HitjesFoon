#ifndef GUINULL_H
#define GUINULL_H

#include <iostream>
#include <cstdarg>

#include "gui/GUI.h"

using namespace std;

class GUINull : public GUI {
public:
    GUINull(PRINT_LEVEL msglevel);
    virtual ~GUINull();

    virtual void printlevel(PRINT_LEVEL level, const char *format, ...);
    virtual void confirm(PRINT_LEVEL level, const char *format, ...);
    virtual int getInput();
    virtual int selection(vector<string> options);
protected:
    virtual void printlevel(PRINT_LEVEL level, const char *format, va_list args);
private:
};

#endif // GUINULL_H
