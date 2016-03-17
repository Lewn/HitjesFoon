#ifndef YOUTUBEAPI_H
#define YOUTUBEAPI_H

#define DEVELOPER_KEY "AIzaSyCHwvjNTHiHoPC8tW6_ibs5MYbakDE8kTg" //Hitjesfoon
//#define DEVELOPER_KEY "AIzaSyBghZHHsanJEnahktJ5LmWbEYBv0WsgyVc" //Leon

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <string>
#include <fstream>
#include <streambuf>
#include <stdio.h>
#include <stdarg.h>
#include <algorithm>
#include <memory>
#include <regex>

#include "structs.h"
#include "Tools.h"
#include "gui/GUI.h"
#include "HTTPTransfer.h"
#include "JsonParser.h"

using namespace std;


class YoutubeAPI {
public:
    enum RequestType {
        SEARCH
    };

    YoutubeAPI(GUI &gui);
    ~YoutubeAPI();

    string searchVid(int id, const char *query, const char *dllocation);
protected:
    GUI &gui;
    HTTPTransfer *transfer;

    string urlDecode(const char *src);

    string makeRequest(RequestType requestType, const char *part, const char *fields, unsigned char maxResults, const char *extra);
    string getVideoInfo(const char *videoId);
#if !defined(USE_YOUTUBE_DL)
    string getFileFromVideoInfo(const char *videoInfo);
#endif
    string downloadEncodedUrl(const char *url, const char *title);
    void decryptSignature(char *sigstart, int siglen, char *decrypted);

    string downloadYoutubeDL(int id, const char *dllocation, const char *videoId, const char *title);
    void performCmd(int id, string cmd);
    float reformat(string val, string type);
private:
    YoutubeAPI(const YoutubeAPI &that) = delete;
};

#endif // YOUTUBEAPI_H
