#include "retrieve/HTTPTransfer.h"

HTTPTransfer::HTTPTransfer(GUI &gui) : gui(gui) {
    // TODO make memory safe on exceptions (like when internet is down?)
    curl = curl_easy_init();
    if (curl == NULL) {
        throw "Could not initialize curl";
    }
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &gui);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
}

HTTPTransfer::HTTPTransfer(const HTTPTransfer &that) : HTTPTransfer(that.gui) {
    // TODO use shared ptr instead?
    // constructs a new curl object
}

HTTPTransfer::~HTTPTransfer() {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

string HTTPTransfer::get(const char *url) {
    char *receivedData = NULL;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receivedData);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dataReceived);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    perform();
    string dataStr = receivedData;
    SAFE_DELETE_ARRAY(receivedData);
    return dataStr;
}

void HTTPTransfer::get(const char *url, const char *filename) {
    FILE *downloadFile = fopen(filename, "wb");
    get(url, downloadFile);
    SAFE_CLOSE(downloadFile);
}

void HTTPTransfer::get(const char *url, FILE *file) {
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    perform();
}

void HTTPTransfer::perform() {
    CURLcode res = curl_easy_perform(curl);
    // Check for errors
    if (res != CURLE_OK) {
        gui.printlevel(LERROR, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        throw "curl_easy_perform() failed";
    }
}

long HTTPTransfer::getStatusCode() {
    long statusCode;
    CURLcode res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
    if (res != CURLE_OK) {
        gui.printlevel(LERROR, "curl_easy_getinfo() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        throw "couldn't retrieve status code";
    }
    return statusCode;
}

string HTTPTransfer::escape(const char *toEscape) {
    char *escaped = curl_easy_escape(curl, toEscape, 0);
    string escStr = string(escaped);
    curl_free(escaped);
    return escStr;
}

size_t HTTPTransfer::dataReceived(char *newData, size_t dataSize, size_t dataLen, char **receivedDataPtr) {
    char *receivedData = *receivedDataPtr;
    size_t receivedLen;
    if (receivedData) {
        receivedLen = strlen(receivedData);
    } else {
        receivedLen = 0;
    }

    char *newBuf = new char[receivedLen + dataSize  *dataLen + 1];
    if (newBuf == NULL) {
        return 0;
    }
    if (receivedData) {
        strcpy(newBuf, receivedData);
    }
    strncpy(newBuf + receivedLen, newData, dataSize  *dataLen);
    newBuf[receivedLen + dataSize  *dataLen] = '\0';
    SAFE_DELETE_ARRAY(receivedData);
    receivedData = newBuf;
    (*receivedDataPtr) = receivedData;
    return dataSize *dataLen;
}

int HTTPTransfer::progress(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    GUI *gui = (GUI *)clientp;
    if (dltotal) {
        gui->printlevel(LBGINFO, "Downloaded %3.0f%% \r", (double)dlnow / dltotal  *100);
    } else if (ultotal) {
        gui->printlevel(LBGINFO, "Uploaded %3.0f%% \r", (double)ulnow / ultotal  *100);
    }
    return 0;
}
