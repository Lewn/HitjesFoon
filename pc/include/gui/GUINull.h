#ifndef GUINULL_H
#define GUINULL_H

#include <iostream>
#include <cstdarg>
#include <vector>

#include "gui/GUI.h"

using namespace std;

class GUINull : public GUI {
public:
    GUINull(PRINT_LEVEL msglevel);
    virtual ~GUINull();

    virtual void printlevel(PRINT_LEVEL level, const char *format, ...) override;
    virtual void confirm(PRINT_LEVEL level, const char *format, ...) override;
    virtual void addInput(int input) override;
    virtual int getInput() override;
    virtual int getKeystroke() override;
    virtual int selection(vector<string> options) override;
protected:
    vector<int> inputs;

    virtual void printlevel(PRINT_LEVEL level, const char *format, va_list args);
private:
};

#endif // GUINULL_H
