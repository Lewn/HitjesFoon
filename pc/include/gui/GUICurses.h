#ifndef GUICURSES_H
#define GUICURSES_H

#include "gui/GUI.h"
#include "InputProcessor.h"

#ifdef _WIN32
#include <curses.h>
#else
#include <ncurses.h>
#endif

class GUICurses : public GUI {
public:
    GUICurses(PRINT_LEVEL msglevel);
    virtual ~GUICurses();

    void printlevel(PRINT_LEVEL level, const char *format, ...);
    void confirm(PRINT_LEVEL level, const char *format, ...);
    int getInput();
    int selection(vector<string> options);

    int getchsilent();
    int getchar();
    int getKey();
protected:
    void initScr();
    void deinitScr();
    void printlevel(PRINT_LEVEL level, const char *format, va_list args);
private:
};

#endif // GUICURSES_H
