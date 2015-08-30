#ifndef TOOLS_H
#define TOOLS_H

#define USE_YOUTUBE_DL


#define SAFE_DELETE(x) delete(x); x = NULL
#define SAFE_DELETE_ARRAY(x) delete[](x); x = NULL

#include "ScreenCommands.h"
#include <vector>
#include <string>
#include <ctype.h>
#include <stdio.h>

using namespace std;

char* trimLeft(char* toTrim);
char* trimRight(char* toTrim);
char* trim(char* toTrim);
char* getAbsolutePath(const char* listFilePath, int pathLen, const char* filename);
bool strmatch(char* str, char* pattern);

#ifdef _WIN32
#include "conio.h"
#include <windows.h> // WinApi header
#else
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
struct termios origTermios;

void resetTerminalMode();
void setConioTerminalMode();
int kbhit();
int getch();
#endif
void getCursorXY(int *x, int *y);
void setCursorXY(int x, int y);

enum PRINT_LEVEL {
    LERROR,
    LWARNING,
    LINFO,
    LBGINFO,
    LDEBUG
};

extern PRINT_LEVEL msglevel;

void printlevel(PRINT_LEVEL level, char *format, ...);
int getKey();
int readKeyboard();
int selection(vector<string> options);

#endif // TOOLS_H
