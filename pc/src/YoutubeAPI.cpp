#include "YoutubeAPI.h"

YoutubeAPI::YoutubeAPI() {
}

YoutubeAPI::~YoutubeAPI() {
}


char* YoutubeAPI::searchVid(const char *query, const char *dllocation) {
    // we only need the total amount of results and each results id (kind and channelId/videoId)
    JsonParser result = makeRequest(SEARCH, "id,snippet", "pageInfo/totalResults,items(id,snippet/title)", 5, query);
    if (result.getTotalResults() == 0) {
        throw "Nothing found while searching";
    }

    char *fileName = NULL;
    vector<string> videoIds = result.getVideoIds();
    vector<string> titles = result.getVideoTitles();

#if defined (USE_YOUTUBE_DL)

    fileName = downloadYoutubeDL(dllocation, videoIds[0].data(), titles[0].data());

#else

    char *videoInfo = getVideoInfo(videoIds[0].data());
    fileName = getFileFromVideoInfo(videoInfo);
    SAFE_DELETE_ARRAY(videoInfo);

#endif
    while (!fileName && videoIds.size() > 1) {
        videoIds.erase(videoIds.begin());
        titles.erase(titles.begin());

        // wait 10 seconds, if user typed y, go to next video id
        printlevel(LINFO, "\nNo valid video found, which alternative to download? (1-%d)\n", titles.size());
        for (int titleIndex = 0; titleIndex < titles.size(); titleIndex++) {
            printlevel(LINFO, " %d: %s\n", titleIndex + 1, titles[titleIndex].data());
        }
        int i;
        for (i = 0; i < 1000; i++) {
            if (kbhit()) {
                char c = getch();
                int index = c - '0' - 1;
                if (index >= 0 && index < titles.size()) {
#if defined (USE_YOUTUBE_DL)
                    fileName = downloadYoutubeDL(dllocation, videoIds[0].data(), titles[0].data());
#else
                    char *videoInfo = getVideoInfo(videoIds[0].data());
                    fileName = getFileFromVideoInfo(videoInfo);
                    SAFE_DELETE_ARRAY(videoInfo);
#endif
                }
                break;
            }
            Sleep(10);
            if (i % 100 == 0) {
                printlevel(LINFO, "%d seconds  \r", 10 - i / 100);
            }
        }
        if (i == 1000) {
            break;
        }
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

    printlevel(LDEBUG, "\n%s\n", baseUrl);
    char* jsonString = transfer.get(baseUrl);
    printlevel(LDEBUG, "%s", jsonString);
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
    if (!fmtEnd) {
        fmtEnd = strchr(fmt, '\0');
    }

    char *titleStart = strstr(videoInfo, "title=") + 6;
    if (!titleStart) {
        throw "No title start found";
    }
    char *titleEnd = strchr(titleStart, '&');
    if (!titleEnd) {
        titleEnd = strchr(titleStart, '\0');
    } else {
        *(titleEnd) = '\0';
    }
    char encoded[fmtEnd - fmt + 1];
    strncpy(encoded, fmt, fmtEnd - fmt);
    encoded[fmtEnd - fmt] = 0;

    fmt = fmtEnd = NULL;

    char* decoded = urlDecode(encoded);
    FILE *file = fopen("decoded.txt", "wb");
    fputs(decoded, file);
    fclose(file);

    char *urlStart = NULL, *urlEnd = NULL;
    char *itagStart = NULL, *itagEnd = NULL;

    //map<int, char*> urls;
    const int priority[] = {38, 37, 46, 22, 45, 44, 35, 18, 34, 43, 6, 5, 36, 17, 13, 0};
    int highestPriority = 1000;
    char *highestUrl = NULL;
    char *farEnd = strchr(decoded, ',');

    char sig[300];
    sig[0] = '\0';

    //TODO: strlen eruit halen!!
    int len = strlen(decoded);
    for (char *itr = decoded; itr <= len + decoded; itr++) {
        switch(*itr) {
            case 'u':
                // check for url
                if (!strncmp(itr, "url=", 4)) {
                    printlevel(LDEBUG, "url found \n");
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
                {

                    bool decrypt = !strncmp(itr, "s=", 2);
                    if (decrypt || !strncmp(itr, "sig=", 4)) {
                        printlevel(LDEBUG, "sig found \n");
                        if (sig[0]) {
                            throw "two sigs found";
                        }
                        char *sigEnd = strchr(itr, '&');
                        if ((farEnd && (farEnd < sigEnd)) || !sigEnd) {
                            // if last character of this decoded youtube information is earlier than the end, end it there
                            sigEnd = farEnd;
                        }
                        if (decrypt) {
                            // encrypted signature, not fully implemented yet
                            // TODO: implement player url extraction
                            decryptSignature(itr + 2, sigEnd - (itr + 2), sig);
                            if (!sig[0]) {
                                printlevel(LDEBUG, "couldn't decrypt sig\n");
                            }
                        } else {
                            strncpy(sig, itr + 4, sigEnd - (itr + 4));
                            sig[sigEnd - (itr + 4)] = '\0';
                        }
                        itr = sigEnd - 1;
                    }
                    break;
                }
            case 'i':
                // check for signature
                if (!strncmp(itr, "itag=", 5)) {
                    printlevel(LDEBUG, "itag found\n");
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
                printlevel(LDEBUG, "Found comma\n");
                // split on comma, save only when url and sig found
                if (urlStart && urlEnd && sig[0] && itagStart && itagEnd) {
                    // only parse if all information found
                    int itag;
                    sscanf(itagStart, "%d", &itag);

                    int i;
                    for (i = 0; priority[i] && priority[i] != itag; i++);
                    if (priority[i] && i <= highestPriority) {

                        *urlEnd = 0;
                        char *decodedUrl = urlDecode(urlStart);

                        SAFE_DELETE_ARRAY(highestUrl);

                        int urlLen = strlen(decodedUrl);
                        highestUrl = new char[urlLen + 7 + titleEnd - titleStart + 11 + strlen(sig) + 1];

                        strncpy(highestUrl, decodedUrl, urlLen);
                        strncpy(highestUrl + urlLen, "&title=", 7);
                        strncpy(highestUrl + urlLen + 7, titleStart, titleEnd - titleStart);
                        strncpy(highestUrl + urlLen + 7 + (titleEnd - titleStart), "&signature=", 11);
                        strcpy(highestUrl + urlLen + 7 + (titleEnd - titleStart) + 11, sig);
                        highestUrl[urlLen + 7 + (titleEnd - titleStart) + 11 + strlen(sig)] = 0;

                        SAFE_DELETE_ARRAY(decodedUrl);

                        highestPriority = i;

                        printlevel(LBGINFO, "\nNew url inserted, %d: %s\n", itag, highestUrl);
                    } else {
                        printlevel(LBGINFO, "\nData with lower quality found\n");
                    }
                } else {
                    printlevel(LWARNING, "\nNot all information found\n");
                }

                urlStart = urlEnd = itagStart = itagEnd = NULL;
                sig[0] = '\0';
                if (itr != len + decoded) {
                    farEnd = strchr(itr + 1, ',');
                    if (!farEnd) {
                        farEnd = strchr(itr + 1, '\0');
                    }
                }
        }
    }

    char *fileName = NULL;
    if (highestUrl) {
        printlevel(LBGINFO, "Best url found: \n%s\n", highestUrl);
        fileName = downloadEncodedUrl(highestUrl, titleStart);
        SAFE_DELETE_ARRAY(highestUrl);
    }

    SAFE_DELETE_ARRAY(decoded);

    return fileName;
}

char *YoutubeAPI::downloadEncodedUrl(const char *url, const char *title) {
    int titleLen = strlen(title);
    char fileName[titleLen + 4 + 1];
    strncpy(fileName, title, titleLen);
    strncpy(fileName + titleLen, ".mp4", 4);
    fileName[titleLen + 4] = 0;

    char *decodedFilename = urlDecode(fileName);
    printlevel(LBGINFO, "filename address '%p'", decodedFilename);

    // replace + with spaces in title
    for (char *itr = decodedFilename; *itr; itr++) {
        if (*itr == '+') {
            *itr = ' ';
        } else if (!((*itr >= 'A' && *itr <= 'Z') || (*itr >= 'a' && *itr <= 'z') || (*itr >= '0' && *itr <= '9') || *itr == '(' || *itr == ')' || *itr == '-' || *itr == '_' || *itr == '#' || *itr == '.')) {
            *itr = ' ';
        }
    }

    printlevel(LBGINFO, "\nStarted downloading '%s'\n", decodedFilename);
    transfer.get(url, decodedFilename);
    printlevel(LBGINFO, "Downloading done!\n");
    return decodedFilename;
}

char* YoutubeAPI::downloadYoutubeDL(const char *dllocation, const char* videoId, const char* title) {
    int titleLen = strlen(title);
    char fileName[titleLen + 4 + 1];
    strncpy(fileName, title, titleLen);
    strncpy(fileName + titleLen, ".mp4", 4);
    fileName[titleLen + 4] = 0;

    char *decodedFilename = urlDecode(fileName);
    printlevel(LDEBUG, "filename address '%p'", decodedFilename);

    filesystemSafe(decodedFilename);
    char *absFilename = getAbsolutePath(dllocation, strlen(dllocation), decodedFilename);
    SAFE_DELETE_ARRAY(decodedFilename);

    printlevel(LBGINFO, "\nStarted downloading '%s' using youtube-dl\n", absFilename);

    char* buffer = new char[200];
    if (msglevel < PRINT_LEVEL::LBGINFO) {
        sprintf(buffer, "youtube-dl.exe --quiet --no-progress --extract-audio --audio-format mp3 -o \"%s\" -- %s", absFilename, videoId);
    }else {
        sprintf(buffer, "youtube-dl.exe --extract-audio --audio-format mp3 -o \"%s\" -- %s", absFilename, videoId);
    }
    if(!system(buffer)) {
        printlevel(LBGINFO, "Downloading done!\n");
        // set extension to mp3
        char *extension = strrchr(absFilename, '.');
        if (extension) {
            // add .mp3 extension to filename
            sprintf(extension, ".mp3");
        } else {
            printlevel(LBGINFO, "Filename: %s\n", absFilename);
            throw "Couldn't change extension";
        }
        return absFilename;
    } else {
        printlevel(LWARNING, "Downloading failed!\n");
        SAFE_DELETE_ARRAY(absFilename);
        return NULL;
    }
}

char *YoutubeAPI::urlDecode(char *src) {
    int len = strlen(src);
    char* ret = new char[len + 1];
    unsigned int i, j;
    char num[3];
    num[2] = 0;
    for (j = i = 0; i < len; i++, j++) {
        if (src[i] == '%') {
            num[0] = src[i + 1];
            num[1] = src[i + 2];
            unsigned int decoded;
            sscanf(num, "%x", &decoded);
            *(ret + j) = (char)decoded;
            i = i + 2;
        } else {
            ret[j] = src[i];
        }
    }
    ret[j] = 0;
    return (ret);
}


void YoutubeAPI::decryptSignature(char *sigstart, int siglen, char *decrypted) {
    return;
    printlevel(LDEBUG, "decrypted sig length %d\n", siglen);
    switch (siglen) {
        case 93:
            reverse_copy(sigstart + 30, sigstart + 87, decrypted);      // 56 chars
            decrypted[56] = sigstart[88];                               // 57 chars
            reverse_copy(sigstart + 6, sigstart + 29, decrypted + 57);  // 87 chars
            decrypted[87] = '\0';
            break;
        case 92:
            decrypted[0] = sigstart[25];                                // 1 char
            copy(sigstart + 3, sigstart + 25, decrypted + 1);           // 23 chars
            decrypted[23] = sigstart[0];                                // 24 chars
            copy(sigstart + 26, sigstart + 42, decrypted + 24);         // 40 chars
            decrypted[40] = sigstart[79];                               // 41 chars
            copy(sigstart + 43, sigstart + 79, decrypted + 41);         // 77 chars
            decrypted[77] = sigstart[91];                               // 78 chars
            copy(sigstart + 80, sigstart + 83, decrypted + 78);         // 81 chars
            decrypted[81] = '\0';
            break;
        case 91:
            reverse_copy(sigstart + 28, sigstart + 85, decrypted);      // 57 chars
            decrypted[57] = sigstart[86];                               // 58 chars
            reverse_copy(sigstart + 6, sigstart + 27, decrypted + 58);  // 79 chars
            decrypted[79] = '\0';
            break;
        case 85:
            copy(sigstart + 3, sigstart + 11, decrypted);               // 8 chars
            decrypted[8] = sigstart[0];                                 // 9 chars
            copy(sigstart + 12, sigstart + 55, decrypted + 9);          // 52 chars
            decrypted[52] = sigstart[84];                               // 53 chars
            copy(sigstart + 56, sigstart + 84, decrypted + 53);         // 81 chars
            decrypted[81] = '\0';
            break;
        case 84:
            reverse_copy(sigstart + 71, sigstart + 79, decrypted);      // 8 chars
            decrypted[8] = sigstart[14];                                // 9 chars
            reverse_copy(sigstart + 38, sigstart + 70, decrypted + 9);  // 41 chars
            decrypted[41] = sigstart[70];                               // 42 chars
            reverse_copy(sigstart + 15, sigstart + 37, decrypted + 42); // 64 chars
            decrypted[64] = sigstart[80];                               // 65 chars
            reverse_copy(sigstart + 0, sigstart + 14, decrypted + 65);  // 79 chars
            decrypted[79] = '\0';
        case 83:
            reverse_copy(sigstart + 64, sigstart + 81, decrypted);      // 17 chars
            decrypted[17] = sigstart[0];                                // 18 chars
            reverse_copy(sigstart + 1, sigstart + 63, decrypted + 18);  // 80 chars
            decrypted[80] = sigstart[63];                               // 81 chars
            decrypted[81] = '\0';
            break;
    }
    printlevel(LDEBUG, "after decryption: %s\n", decrypted);
    //exit(1);
//elif len(s) == 90:
//    return s[25] + s[3: 25] + s[2] + s[26: 40] + s[77] + s[41: 77] + s[89] + s[78: 81]
//       elif len(s) == 89:
//       return s[84: 78: -1] + s[87] + s[77: 60: -1] + s[0] + s[59: 3: -1]
//              elif len(s) == 88:
//                  return s[7: 28] + s[87] + s[29: 45] + s[55] + s[46: 55] + s[2] + s[56: 87] + s[28]
//                     elif len(s) == 87:
//                     return s[6: 27] + s[4] + s[28: 39] + s[27] + s[40: 59] + s[2] + s[60: ]
//                            elif len(s) == 86:
//                            return s[80: 72: -1] + s[16] + s[71: 39: -1] + s[72] + s[38: 16: -1] + s[82] + s[15:: -1]
//                                                   elif len(s) == 82:
//                                                   return s[80: 37: -1] + s[7] + s[36: 7: -1] + s[0] + s[6: 0: -1] + s[37]
//                                                           elif len(s) == 81:
//                                                           return s[56] + s[79: 56: -1] + s[41] + s[55: 41: -1] + s[80] + s[40: 34: -1] + s[0] + s[33: 29: -1] + s[34] + s[28: 9: -1] + s[29] + s[8: 0: -1] + s[9]
//                                                                   elif len(s) == 80:
//                                                                       return s[1: 19] + s[0] + s[20: 68] + s[19] + s[69: 80]
//                                                                           elif len(s) == 79:
//                                                                           return s[54] + s[77: 54: -1] + s[39] + s[53: 39: -1] + s[78] + s[38: 34: -1] + s[0] + s[33: 29: -1] + s[34] + s[28: 9: -1] + s[29] + s[8: 0: -1] + s[9]
//
//                                                                                   else:
//                                                                                           raise ExtractorError(u'Unable to decrypt signature, key length %d not supported; retrying might work' % (len(s)))
}
