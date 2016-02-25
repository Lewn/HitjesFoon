#include "GUICurses.h"

GUICurses::GUICurses(PRINT_LEVEL msglevel) : GUI(msglevel) {
    initScr();
}

GUICurses::~GUICurses() {
    deinitScr();
}



void GUICurses::initScr() {
    initscr();
    // directly get typed key (no character buffering until newline)
    cbreak();
    // getch() is non-blocking
    nodelay(stdscr, TRUE);
    // allow all keyboard keys (long keys)
    keypad(stdscr, TRUE);
    // enable auto scroll when last line is written
    scrollok(stdscr, TRUE);
    // use color terminal
    start_color();
    // define our colors
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
}

void GUICurses::deinitScr() {
    endwin();
}


int GUICurses::getchsilent() {
    int c;
    noecho();
    c = getch();
    echo();
    return c;
}

int GUICurses::getchar() {
    int c;
    noecho();
    do {
        c = getch();
    } while (c == ERR);
    echo();
    return c;
}

void GUICurses::printlevel(PRINT_LEVEL level, const char *format, va_list args) {
    char buf[1000];
    vsnprintf(buf, sizeof(buf), format, args);

    if (level == LERROR) {
        attrset(COLOR_PAIR(1) | A_BOLD);
        addstr("ERR : ");
    } else if (level == LWARNING) {
        attrset(COLOR_PAIR(2) | A_BOLD);
        addstr("WARN: ");
    } else {
        attrset(COLOR_PAIR(0) | A_BOLD);
    }
    addstr(buf);
    // set print back to normal
    attrset(A_NORMAL);
    // actually draw the screen
    refresh();
    if (level == LDEBUGSLOW) {
        // Wait 20 seconds for debugslow type text
        napms(20000);
    }
}

void GUICurses::printlevel(PRINT_LEVEL level, const char *format, ...) {
    if (level > msglevel) {
        return;
    }
    va_list args;
    va_start(args, format);
    printlevel(level, format, args);
    va_end(args);
}

void GUICurses::confirm(PRINT_LEVEL level, const char *format, ...) {
    if (level > msglevel) {
        return;
    }
    va_list args;
    va_start(args, format);
    printlevel(level, format, args);
    va_end(args);
    printlevel(level, "\nPress any key to continue...\n");
    getchar();
}

int GUICurses::getInput() {
    int c = getchsilent();

    if (c != ERR) {
        if (c >= '0' && c <= '9') {
            // got a number, return it
            return c - '0';
        } else if (c == 8 || c == KEY_BACKSPACE) {
            // backspace, simulate earth button (reset number)
            return INPUT_EARTH_DOWN;
        } else if (c == 's') {
            // s, simulate horn (swap output)
            return INPUT_HORN_SWAP;
        } else if (c == 'q') {
            // q, return quit
            return INPUT_END;
        } else if (c == 'u') {
            return INPUT_UPDATE;
        } else if (c == 't') {
            // t, testing purposes
            return INPUT_TEST;
        }
    }

    return INPUT_NONE;
}

int GUICurses::selection(vector<string> options) {
    int curSelection = 0;
    int x = 0, y = 0, minY, maxY, startY = 0;
    minY = 5;
    maxY = getmaxy(stdscr) - minY - 1;
    do {
        int i = 0;
        for (vector<string>::iterator it = options.begin(); it != options.end(); it++, i++) {
            if (i < startY) {
                continue;
            } else if (i >= maxY + startY) {
                break;
            }
            if (i == curSelection) {
                attrset(COLOR_PAIR(0) | A_REVERSE);
            } else {
                attrset(COLOR_PAIR(0) | A_BOLD);
            }
            printw("%s\n", (*it).data());
        }
        attrset(A_NORMAL | A_BOLD);

        int c = getchar();
        if (c == 0x0A) {
            // Enter key
            break;
        }
        if (c == KEY_UP) {
            curSelection--;
            curSelection = max(curSelection, 0);
            if (curSelection - startY < 0) {
                startY--;
            }
        } else if (c == KEY_DOWN) {
            curSelection++;
            curSelection = min(curSelection, (int)options.size() - 1);
            if (curSelection - startY >= maxY) {
                startY++;
            }
        }
        getyx(stdscr, y, x);
        move(y - min((int)options.size(), maxY), x);
    } while (true);
    move(y, x);
    return curSelection;
}
