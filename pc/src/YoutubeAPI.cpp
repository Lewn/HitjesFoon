#include "YoutubeAPI.h"



YoutubeAPI::YoutubeAPI() {
}

YoutubeAPI::~YoutubeAPI() {
}


char* YoutubeAPI::searchVid(const char *query) {
    // we only need the total amount of results and each results id (kind and channelId/videoId)
    JsonParser result = makeRequest(SEARCH, "id", "pageInfo/totalResults,items/id", 5, query);

    char *fileName = NULL;
    for (string videoId : result.getVideoIds()) {
        char *videoInfo = getVideoInfo(videoId.data());
        SAFE_DELETE_ARRAY(fileName);
        fileName = getFileFromVideoInfo(videoInfo);
        SAFE_DELETE_ARRAY(videoInfo);
        if (fileName) {
            break;
        }
        // wait 10 seconds, if user typed y, go to next video id
        printf("\nNo valid video found, search next? (y)\n");
        for (int i = 0; i < 10000; i++) {
            if (kbhit()) {
                char c = getch();
                if (c == 'y') {
                    goto outer;
                } else {
                    break;
                }
            }
            Sleep(1);
            if (i % 1000 == 0) {
                printf("%d seconds  \r", 10 - i / 1000);
            }
        }
        return NULL;
outer:;
    }

    return fileName;
}

JsonParser YoutubeAPI::makeRequest(RequestType requestType, char* part, char* fields, unsigned char maxResults, const char* extra) {
    const char* typeStr;
    char* extraArgs = new char[200];

    switch(requestType) {
        case SEARCH: {
                typeStr = "search";
                char *urlEscaped = transfer.escape(extra);
                sprintf(extraArgs, "q=%s&type=video&", urlEscaped);
                transfer.free(urlEscaped);
            }
            break;
        default:
            throw "Unknown request type";
    }

    char baseUrl[1024];
    char fieldsStr[100];

    if(fields == 0) {
        // no fields parameter given, omit it from request
        fieldsStr[0] = 0;
    } else {
        // add fields parameters to fields= field
        sprintf(fieldsStr, "&fields=%s", fields);
    }

    sprintf(baseUrl, "https://www.googleapis.com/youtube/v3/%s?%spart=%s%s&key=%s&maxResults=%d", typeStr, extraArgs, part, fieldsStr, DEVELOPER_KEY, maxResults);
    SAFE_DELETE_ARRAY(extraArgs);
    extraArgs = NULL;

    printf("\n%s\n", baseUrl);
    char* jsonString = transfer.get(baseUrl);
    printf("%s", jsonString);
    JsonParser parser;
    parser.parse(jsonString);
    SAFE_DELETE_ARRAY(jsonString);
    return parser;
}

char *YoutubeAPI::getVideoInfo(const char *videoId) {
    char baseUrl[1024];

    sprintf(baseUrl, "http://www.youtube.com/get_video_info?video_id=%s&asv=3&el=detailpage&hl=en_US", videoId);
    char* videoInfo = transfer.get(baseUrl);

    FILE* file = fopen("videoInfo.txt", "wb");
    fputs(videoInfo, file);
    fclose(file);
    return videoInfo;
}

char *YoutubeAPI::getFileFromVideoInfo(char *videoInfo) {

    char *fmt = strstr(videoInfo, "url_encoded_fmt_stream_map=");
    if (!fmt) {
        throw "fmt not found";
    }
    char *fmtEnd = strchr(fmt, '&');

    char *titleStart = strstr(videoInfo, "title=") + 6;
    char *titleEnd = strchr(titleStart, '&');
    if (!titleEnd) {
        titleEnd = strchr(titleStart, '\0');
    }
    *(titleEnd) = '\0';

    char *encoded = new char[fmtEnd - fmt + 1];
    strncpy(encoded, fmt, fmtEnd - fmt);
    encoded[fmtEnd - fmt] = 0;

    fmt = fmtEnd = NULL;

    char* decoded = urlDecode(encoded);
    FILE *file = fopen("decoded.txt", "wb");
    fputs(decoded, file);
    fclose(file);
    SAFE_DELETE_ARRAY(encoded);
    encoded = NULL;

    char *urlStart = NULL, *urlEnd = NULL;
    char *sigStart = NULL, *sigEnd = NULL;
    char *itagStart = NULL, *itagEnd = NULL;

    //map<int, char*> urls;
    const int priority[] = {38, 37, 46, 22, 45, 44, 35, 18, 34, 43, 6, 5, 36, 17, 13, 0};
    int highestPriority = 1000;
    char* highestUrl = NULL;
    char* farEnd = strchr(decoded, ',');

    for (char* itr = decoded; * (itr - 1); itr++) {
        switch(*itr) {
            case 'u':
                // check for url
                if (!strncmp(itr, "url=", 4)) {
                    printf("url found \n");
                    if (urlStart || urlEnd) {
                        throw "two urls found";
                    }
                    urlEnd = strchr(itr, '&');
                    if ((farEnd && (farEnd < urlEnd)) || !urlEnd) {
                        // if last character of this decoded youtube information is earlier than the end, end it there
                        urlEnd = farEnd;
                    }
                    urlStart = itr + 4;
                    itr = urlEnd - 1;
                }
                break;
            case 's':
                // check for signature
                if (!strncmp(itr, "sig=", 4)) {
                    printf("sig found \n");
                    if (sigStart || sigEnd) {
                        throw "two sigs found";
                    }
                    sigEnd = strchr(itr, '&');
                    if ((farEnd && (farEnd < sigEnd)) || !sigEnd) {
                        // if last character of this decoded youtube information is earlier than the end, end it there
                        sigEnd = farEnd;
                    }
                    sigStart = itr + 4;
                    itr = sigEnd - 1;
                }
                break;
            case 'i':
                // check for signature
                if (!strncmp(itr, "itag=", 5)) {
                    printf("itag found\n");
                    if (itagStart || itagEnd) {
                        throw "two itag found";
                    }
                    itagEnd = strchr(itr, '&');
                    if ((farEnd && (farEnd < itagEnd)) || !itagEnd) {
                        // if last character of this decoded youtube information is earlier than the end, end it there
                        itagEnd = farEnd;
                    }
                    itagStart = itr + 5;
                    itr = itagEnd - 1;
                }
                break;
            case '\0':
            case ',':
                printf("Found comma\n");
                // split on comma, save only when url and sig found
                if (urlStart && urlEnd && sigStart && sigEnd && itagStart && itagEnd) {
                    // only parse if all information found
                    int itag;
                    sscanf(itagStart, "%d", &itag);

                    int i;
                    for (i = 0; priority[i] && priority[i] != itag; i++);
                    if (priority[i] && i <= highestPriority) {

                        *urlEnd = 0;
                        char* decodedUrl = urlDecode(urlStart);

                        int urlLen = strlen(decodedUrl);
                        char* url = new char[urlLen + 7 + titleEnd - titleStart + 11 + sigEnd - sigStart  + 1];

                        strncpy(url, decodedUrl, urlLen);
                        strncpy(url + urlLen, "&title=", 7);
                        strncpy(url + urlLen + 7, titleStart, titleEnd - titleStart);
                        strncpy(url + urlLen + 7 + (titleEnd - titleStart), "&signature=", 11);
                        strncpy(url + urlLen + 7 + (titleEnd - titleStart) + 11, sigStart, sigEnd - sigStart);
                        url[urlLen + 7 + (titleEnd - titleStart) + 11 + (sigEnd - sigStart)] = 0;

                        highestPriority = i;
                        SAFE_DELETE_ARRAY(highestUrl);
                        highestUrl = url;

                        printf("\nNew url inserted, %d: %s\n", itag, url);
                    } else {
                        printf("\nData with lower quality found\n");
                    }
                } else {
                    printf("\nNot all information found\n");
                    if (!(urlStart && urlEnd && itagStart && itagEnd)) {
                        return NULL;
                    }
                }

                urlStart = urlEnd = sigStart = sigEnd = itagStart = itagEnd = NULL;
                farEnd = strchr(itr + 1, ',');
                if (!farEnd) {
                    farEnd = strchr(itr + 1, '\0');
                }
        }
    }

    char *fileName = NULL;
    if (highestUrl) {
        printf("Best url found: \n%s\n", highestUrl);
        fileName = downloadEncodedUrl(highestUrl, titleStart);
        SAFE_DELETE_ARRAY(highestUrl);
        printf("deleted1\n");
    }

    SAFE_DELETE_ARRAY(decoded);
    printf("deleted2\n");

    return fileName;
}

char *YoutubeAPI::downloadEncodedUrl(const char *url, const char *title) {
    int titleLen = strlen(title);
    char fileName[titleLen + 4 + 1];
    strncpy(fileName, title, titleLen);
    strncpy(fileName + titleLen, ".mp4", 4);
    fileName[titleLen + 4] = 0;

    char *decodedFilename = urlDecode(fileName);

    // replace + with spaces in title
    for (char* itr = decodedFilename; *itr; itr++) {
        if (*itr == '+') {
            *itr = ' ';
        } else if (!((*itr >= 'A' && *itr <= 'Z') || (*itr >= 'a' && *itr <= 'z') || (*itr >= '0' && *itr <= '9') || *itr == '(' || *itr == ')' || *itr == '-' || *itr == '_' || *itr == '#' || *itr == '.')) {
            *itr = ' ';
        }
    }

    printf("\nStarted downloading '%s'\n", decodedFilename);
    transfer.get(url, decodedFilename);
    printf("Downloading done!\n");
    return decodedFilename;
}

char *YoutubeAPI::urlDecode(char *src) {
    int len = strlen(src);
    char* ret = new char[len];
    char ch;
    int i, j;
    char num[3];
    num[2] = 0;
    for (j = i = 0; i < len; i++, j++) {
        if (src[i] == '%') {
            num[0] = src[i + 1];
            num[1] = src[i + 2];
            sscanf(num, "%x", ret + j);
            i = i + 2;
        } else {
            ret[j] = src[i];
        }
    }
    ret[j] = 0;
    return (ret);
}


/*
void YoutubeAPI::makeRequest(RequestType requestType, char* part) {
    //makeRequest(requestType, part, 0, 5);
}

void YoutubeAPI::makeRequest(RequestType requestType, char* part, unsigned char maxResults) {
    //return makeRequest(requestType, part, 0, maxResults);
}

void YoutubeAPI::makeRequest(RequestType requestType, char* part, char* fields) {
    //return makeRequest(requestType, part, fields, 5);
}*/
