#ifndef YOUTUBEAPI_H
#define YOUTUBEAPI_H

#define DEVELOPER_KEY "AIzaSyCHwvjNTHiHoPC8tW6_ibs5MYbakDE8kTg"

#define _WIN32_WINNT 0x0601

#include <stdio.h>
#include <stdarg.h>
#include <algorithm>

#include "HTTPTransfer.h"
#include "Tools.h"
#include "JsonParser.h"

using namespace std;


class YoutubeAPI {
public:
    enum RequestType {
        SEARCH
    };

    YoutubeAPI();
    ~YoutubeAPI();

    char* searchVid(const char *query);
protected:
    HTTPTransfer transfer;

    char* urlDecode(char* src);

    JsonParser makeRequest(RequestType requestType, char* part, char* fields, unsigned char maxResults, const char* extra);
    char *getVideoInfo(const char *videoId);
    char *getFileFromVideoInfo(char *videoInfo);
    char *downloadEncodedUrl(const char *url, const char *title);
    void decryptSignature(char *sigstart, int siglen, char *decrypted);

    char* downloadYoutubeDL(const char* videoId, const char* title);
private:
};

#endif // YOUTUBEAPI_H
