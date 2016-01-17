#ifndef TOOLS_H
#define TOOLS_H

#define USE_YOUTUBE_DL


#define SAFE_DELETE(x) delete(x); x = NULL
#define SAFE_DELETE_ARRAY(x) delete[](x); x = NULL
#define SAFE_CLOSE(x) fclose(x); x = NULL;

#include <vector>
#include <string>
#include <cstring>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>

#ifdef _WIN32
#include <curses.h>
#else
#include <ncurses.h>
#endif

using namespace std;

char *trimLeft(char *toTrim);
char *trimRight(char *toTrim);
char *trim(char *toTrim);

std::string &ltrim(std::string &s);
std::string &rtrim(std::string &s);
std::string &trim(std::string &s);


char *getAbsolutePath(const char *listFilePath, int pathLen, const char *filename);

#ifndef _WIN32
#include <unistd.h>
#include <sys/select.h>
#endif


enum PRINT_LEVEL {
    LERROR,
    LWARNING,
    LINFO,
    LBGINFO,
    LDEBUG,
    LDEBUGSLOW
};

extern PRINT_LEVEL msglevel;

void initScr();
void deinitScr();
int getchsilent();
int getchar();
void printlevel(PRINT_LEVEL level, const char *format, ...);
int selection(vector<string> options);

int getKey();
int readKeyboard();
void filesystemSafe(char *str);
void filesystemSafe(string &str);

#endif // TOOLS_H
