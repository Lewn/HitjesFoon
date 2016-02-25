#ifndef GUIWT_H
#define GUIWT_H

#include "Tools.h"
#include "GUI.h"
#include "GUINull.h"
#include "GUIWtApplication.h"
#include <vector>
#include <Wt/WApplication>

using namespace Wt;

class GUIWt : public GUINull {
public:
    GUIWt(PRINT_LEVEL msglevel);
    virtual ~GUIWt();

    WApplication *createApplication(const WEnvironment& env);
protected:

    vector<GUIWtApplication *> applications;

    //void printlevel(PRINT_LEVEL level, const char *format, va_list args);
private:
};

#endif // GUIWT_H
