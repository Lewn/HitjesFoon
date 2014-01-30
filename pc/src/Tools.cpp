#include "Tools.h"


char* trimLeft(char* toTrim) {
    while (*toTrim && !isgraph(*toTrim)) {
        toTrim++;
    }
    return toTrim;
}

char* trimRight(char* toTrim) {
    char* tmp = toTrim;
    toTrim += strlen(toTrim) + 1;
    while (--toTrim >= tmp && !isgraph(*(toTrim)));
    *(toTrim + 1) = 0;
    return tmp;
}

char* trim(char* toTrim) {
    return trimLeft(trimRight(toTrim));
}

char* getAbsolutePath(const char* listFilePath, int pathLen, const char* filename) {
    // we need the path and the filename in one string
    int totalLen = pathLen + strlen(filename) + 1;
    char* absolute = new char[totalLen];
    // copy path in the new filename
    strncpy(absolute, listFilePath, pathLen);
    // and copy the name itself
    strcpy(absolute + pathLen, filename);
    return absolute;
}





#ifdef _WIN32
void getCursorXY(int &x, int &y) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        x = csbi.dwCursorPosition.X;
        y = csbi.dwCursorPosition.Y;
    }
}

void setCursorXY(int x, int y) {
    COORD p = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
}
#else
void resetTerminalMode() {
    tcsetattr(0, TCSANOW, &origTermios);
}

void setConioTerminalMode() {
    struct termios newTermios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &origTermios);
    memcpy(&newTermios, &origTermios, sizeof(newTermios));

    /* register cleanup handler, and set the new terminal mode */
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

    if(hit) {
        char c = getch();

        if(c >= '0' && c <= '9') {
            // got a number, return it
            return c - '0';
        } else if(c == 8) {
            // backspace, simulate earth button (reset number)
            return INPUT_EARTH_DOWN;
        } else if(c == 's') {
            // s, simulate horn (swap output)
            return INPUT_HORN_SWAP;
        } else if(c == 'q') {
            // q, return quit
            return INPUT_END;
        } else if (c == 't') {
            // t, testing purposes
            return INPUT_TEST;
        }
    }

    return INPUT_NONE;
}


int selection(vector<string> options) {
    int curSelection = 0;
    int x, y;
    getCursorXY(x, y);
    do {
        setCursorXY(x, y);
        int i = 0;
        for (vector<string>::iterator it = options.begin(); it != options.end(); it++) {
            if (i++ == curSelection) {
                setColors(BLACK, WHITE);
            }else {
                setColors(WHITE, BLACK);
            }
            printf("%s\n", (*it).data());
        }
        resetColors();

        int c = getKey();
        if (c == 13) {
            break;
        }
        if (c == 328) {
            // up arrow
            curSelection = max(--curSelection, 0);
        }else if (c == 336) {
            // down arrow
            curSelection = min(++curSelection, (int)options.size() - 1);
        }
    }while (true);
    return curSelection;
}
