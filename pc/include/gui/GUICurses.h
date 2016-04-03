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

    virtual void printlevel(PRINT_LEVEL level, const char *format, ...) override;
    virtual void confirm(PRINT_LEVEL level, const char *format, ...) override;
    virtual int getInput() override;
    virtual int selection(vector<string> options) override;

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
