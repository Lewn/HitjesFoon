#include "gui/GUINull.h"

GUINull::GUINull(PRINT_LEVEL msglevel) : GUI(msglevel) {
}

GUINull::~GUINull() {
}

void GUINull::printlevel(PRINT_LEVEL level, const char *format, va_list args) {
    char buf[1000];
    vsnprintf(buf, sizeof(buf), format, args);

    if (level == LERROR) {
        cerr << "ERR : " << buf;
    } else if (level == LWARNING) {
        cerr << "WARN: " << buf;
    } else {
        cout << buf;
    }
}

void GUINull::printlevel(PRINT_LEVEL level, const char *format, ...) {
    if (level > msglevel) {
        return;
    }
    va_list args;
    va_start(args, format);
    printlevel(level, format, args);
    va_end(args);
}

void GUINull::confirm(PRINT_LEVEL level, const char *format, ...) {
    if (level > msglevel) {
        return;
    }
    va_list args;
    va_start(args, format);
    printlevel(level, format, args);
    va_end(args);
    cout << "\n";
    printlevel(level, "Press any key to continue...\n");
    getchar();
}

int GUINull::getInput() {
    // Still require a reliable cross platform solution
    // TODO windows getch return 0 or 0xE0 for arrow or special keys
    // linux returns a string of multiple chars
    if (kbhit()) {
        int input = getch();
        if (input == EOF) {
            // Shouldn't happen, but the only reason why getchar() returns an int
            return 0;
        }
        return input;
    }
    return 0;
}

int GUINull::selection(vector<string> options) {
    printlevel(LWARNING, "function selection not implemented yet\n");
    return 0;
}
