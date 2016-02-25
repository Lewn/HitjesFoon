#include "GUIWt.h"


GUIWt::GUIWt(PRINT_LEVEL msglevel) : GUINull(msglevel) {
}

GUIWt::~GUIWt() {
}

Wt::WApplication *GUIWt::createApplication(const WEnvironment& env) {
    GUIWtApplication *application = new GUIWtApplication(env);
    applications.push_back(application);
    return application;
}

//void GUIWt::printlevel(PRINT_LEVEL level, const char *format, va_list args) {
//    char buf[1000];
//    vsnprintf(buf, sizeof(buf), format, args);
//
//    if (level == LERROR) {
//        cerr << "ERR : " << buf;
//    } else if (level == LWARNING) {
//        cerr << "WARN: " << buf;
//    } else {
//        cout << buf;
//    }
//}

