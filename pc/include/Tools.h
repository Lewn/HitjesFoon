#ifndef TOOLS_H
#define TOOLS_H

#define USE_YOUTUBE_DL


#define SAFE_DELETE(x) delete(x); x = NULL
#define SAFE_DELETE_ARRAY(x) delete[](x); x = NULL
#define SAFE_CLOSE(x) if (x != NULL) {fclose(x); x = NULL;}

#include <vector>
#include <string>
#include <cstring>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <memory>

#ifdef __WIN32
#include <windows.h>
#else
#include <cstdio>
#endif

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

std::shared_ptr<FILE> cmdasync(string cmd);

#endif // TOOLS_H
