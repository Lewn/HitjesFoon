#ifndef TOOLS_H
#define TOOLS_H


#define SAFE_DELETE(x) delete(x); x = NULL
#define SAFE_DELETE_ARRAY(x) delete[](x); x = NULL
#define SAFE_CLOSE(x) if (x != NULL) {fclose(x); x = NULL;}

#ifdef __WIN32__
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#endif
#include <vector>
#include <string>
#include <cstring>
#include <ctype.h>
#include <stdio.h>
#include <algorithm>
#include <memory>

using namespace std;

char *trimLeft(char *toTrim);
char *trimRight(char *toTrim);
char *trim(char *toTrim);

string &ltrim(string &s);
string &rtrim(string &s);
string &trim(string &s);

char *getAbsolutePath(const char *listFilePath, int pathLen, const char *filename);

void filesystemSafe(char *str);
void filesystemSafe(string &str);

std::shared_ptr<FILE> cmdasync(const string &cmd);


#ifndef __WIN32__
// Define windows defaults
int getch();
int kbhit();
#endif // __WIN32__

void toolsInit();
void toolsDeinit();

#endif // TOOLS_H
