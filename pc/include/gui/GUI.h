#ifndef GUI_H
#define GUI_H

#include <vector>
#include <string>

using namespace std;

enum PRINT_LEVEL {
    LERROR,
    LWARNING,
    LINFO,
    LBGINFO,
    LDEBUG,
    LDEBUGSLOW
};

class GUI {
public:

    GUI(PRINT_LEVEL msglevel);
    void setMsglevel(PRINT_LEVEL msglevel);
    PRINT_LEVEL getMsglevel();

    virtual void printlevel(PRINT_LEVEL level, const char *format, ...) = 0;
    virtual void confirm(PRINT_LEVEL level, const char *format, ...) = 0;
    virtual int getInput() = 0;
    virtual int selection(vector<string> options) = 0;

    virtual void setPhoneNum(int num) = 0;
    virtual void setPlaylist(const vector<int> &playlist) = 0;

    virtual void setSpeakerVolume(int volume) = 0;
    virtual void setPhoneVolume(int volume) = 0;
protected:
    PRINT_LEVEL msglevel;
private:
};

#endif // GUI_H
