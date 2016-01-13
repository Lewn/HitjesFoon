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

/**
  *compare two strings. They match depending on pattern characters.
  *special compare characters are:
  *- *, match any character
 */
bool strmatch(const char *str, const char *pattern) {
    for (; *pattern; pattern++, str++) {
        if (*pattern != '*' && *pattern != *str) {
            return false;
        }
    }
    return *pattern == *str;
}





#ifdef _WIN32
void getCursorXY(short &x, short&y) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        x = csbi.dwCursorPosition.X;
        y = csbi.dwCursorPosition.Y;
    }
}

void setCursorXY(short x, short y) {
    COORD p = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
}
#else
void resetTerminalMode() {
    tcsetattr(0, TCSANOW, &origTermios);
}

void setConioTerminalMode() {
    struct termios newTermios;

    / *take two copies - one for now, one for later * /
        tcgetattr(0, &origTermios);
    memcpy(&newTermios, &origTermios, sizeof(newTermios));

    / *register cleanup handler, and set the new terminal mode * /
    cfmakeraw(&newTermios);
    tcsetattr(0, TCSANOW, &newTermios);
}

int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch() {
    int r;
    unsigned char c;

    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}
#endif

#include "InputProcessor.h"

void printlevel(PRINT_LEVEL level, const char *format, ...) {
#ifdef _WIN32
    HANDLE hConsole;
#endif // _WIN32
    va_list args;
    if (level > msglevel) {
        return;
    }
#ifdef _WIN32
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (level == LERROR) {
        // print in red text
        SetConsoleTextAttribute(hConsole, 12);
        printf("ERR : ");
    } else if (level == LWARNING) {
        // print in yellow text
        SetConsoleTextAttribute(hConsole, 14);
        printf("WARN: ");
    }
#else
    if (level == LERROR) {
        // print in red text
        printf("\033[31mERR : ");
    } else if (level == LWARNING) {
        // print in yellow text
        printf("\033[33mWARN: ");
    }
#endif // _WIN32
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    // set print back to normal
#ifdef _WIN32
    if (level == LERROR || level == LWARNING) {
        SetConsoleTextAttribute(hConsole, 7);
    }
    if (msglevel == LDEBUGSLOW) {
        Sleep(20);
    }
#else
    if (level == LERROR || level == LWARNING) {
        printf("\033[0m");
    }
    if (msglevel == LDEBUGSLOW) {
        sleep(20);
    }
#endif
}

int getKey() {
    unsigned char c = getch();
    if (c == 0 || c == 224) {
        // extended code, add 256 to the next result
        return 256 + getch();
    }
    return c;
}

int readKeyboard() {
    int hit = kbhit();

    if (hit) {
        char c = getch();

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


int selection(vector<string> options) {
    int curSelection = 0;
    short x, y;
    getCursorXY(x, y);
    do {
        setCursorXY(x, y);
        int i = 0;
        for (vector<string>::iterator it = options.begin(); it != options.end(); it++) {
            if (i++ == curSelection) {
                setColors(BLACK, WHITE);
            } else {
                setColors(WHITE, BLACK);
            }
            printlevel(LINFO, "%s\n", (*it).data());
        }
        resetColors();

        int c = getKey();
        if (c == 13) {
            break;
        }
        if (c == 328) {
            // up arrow
            curSelection--;
            curSelection = max(curSelection, 0);
        } else if (c == 336) {
            // down arrow
            curSelection++;
            curSelection = min(curSelection, (int)options.size() - 1);
        }
    } while (true);
    return curSelection;
}

void filesystemSafe(char *str) {
    // replace + with spaces in title
    const char *filechars = "`~!@#$%^&()-_+=[]{};'.,";
    for (; *str; str++) {
        if (!((*str >= 'A' && *str <= 'Z') || (*str >= 'a' && *str <= 'z') || (*str >= '0' && *str <= '9') || strchr(filechars, *str))) {
            *str = ' ';
        }
    }
}
