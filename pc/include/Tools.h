#ifndef TOOLS_H
#define TOOLS_H

#define USE_YOUTUBE_DL


#define SAFE_DELETE(x) delete(x); x = NULL
#define SAFE_DELETE_ARRAY(x) delete[](x); x = NULL
#define SAFE_CLOSE(x) fclose(x); x = NULL;

#include "ScreenCommands.h"
#include <vector>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>

using namespace std;

char *trimLeft(char *toTrim);
char *trimRight(char *toTrim);
char *trim(char *toTrim);

std::string &ltrim(std::string &s);
std::string &rtrim(std::string &s);
std::string &trim(std::string &s);


char *getAbsolutePath(const char *listFilePath, int pathLen, const char *filename);
bool strmatch(const char *str, const char *pattern);

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
void getCursorXY(short *x, short *y);
void setCursorXY(short x, short y);

enum PRINT_LEVEL {
    LERROR,
    LWARNING,
    LINFO,
    LBGINFO,
    LDEBUG,
    LDEBUGSLOW
};

extern PRINT_LEVEL msglevel;

void printlevel(PRINT_LEVEL level, const char *format, ...);
int getKey();
int readKeyboard();
int selection(vector<string> options);
void filesystemSafe(char *str);
void filesystemSafe(string &str);

#endif // TOOLS_H
