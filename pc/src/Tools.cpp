#include "Tools.h"


char *trimLeft(char *toTrim) {
    while (*toTrim && !isgraph(*toTrim)) {
        toTrim++;
    }
    return toTrim;
}

char *trimRight(char *toTrim) {
    char *tmp = toTrim;
    toTrim += strlen(toTrim) + 1;
    while (--toTrim >= tmp && !isgraph(*(toTrim)));
    *(toTrim + 1) = 0;
    return tmp;
}

char *trim(char *toTrim) {
    return trimLeft(trimRight(toTrim));
}

// trim from start
std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

char *getAbsolutePath(const char *listFilePath, int pathLen, const char *filename) {
    // we need the path and the filename in one string
    int totalLen = pathLen + strlen(filename) + 1;
    char *absolute = new char[totalLen];
    // copy path in the new filename
    strncpy(absolute, listFilePath, pathLen);
    // and copy the name itself
    strcpy(absolute + pathLen, filename);
    return absolute;
}


void initScr() {
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

void deinitScr() {
    endwin();
}

int getchsilent() {
    int c;
    noecho();
    c = getch();
    echo();
    return c;
}

int getchar() {
    int c;
    noecho();
    do {
        c = getch();
    } while (c == ERR);
    echo();
    return c;
}


void printlevel(PRINT_LEVEL level, const char *format, ...) {
    char buf[1000];

    va_list args;
    if (level > msglevel) {
        return;
    }
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

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

int selection(vector<string> options) {
    int curSelection = 0;
    int x, y, minY, maxY, startY = 0;
    minY = 5;
    maxY = getmaxy(stdscr) - minY - 1;
    do {
        int i = 0;
        for (vector<string>::iterator it = options.begin(); it != options.end(); it++, i++) {
            if (i < startY) {
                continue;
            }else if (i >= maxY + startY) {
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
        refresh();

        int c = getch();
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

#include "InputProcessor.h"
int readKeyboard() {
    int c = getchsilent();

    if (c != ERR) {
        if (c >= '0' && c <= '9') {
            // got a number, return it
            return c - '0';
        } else if (c == 8) {
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

void filesystemSafe(char *str) {
    // replace + with spaces in title
    const char *filechars = "`~!@#$^&()-_+=[]{};'., ";
    for (; *str; str++) {
        if (!((*str >= 'A' && *str <= 'Z') || (*str >= 'a' && *str <= 'z') || (*str >= '0' && *str <= '9') || strchr(filechars, *str))) {
            *str = ' ';
        }
    }
}

void filesystemSafe(string &str) {
    // replace + with spaces in title
    const char *filechars = "`~!@#$^&()-_+=[]{};'., ";
    for (std::string::iterator it = str.begin(); it != str.end(); it++) {
        if (!((*it >= 'A' && *it <= 'Z') || (*it >= 'a' && *it <= 'z') || (*it >= '0' && *it <= '9') || strchr(filechars, *it))) {
            str.erase(it);
            it--;
        }
    }
}
