#include "HTTPTransfer.h"

HTTPTransfer::HTTPTransfer() {
    //curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (!curl) {
        throw "Could not initialize curl";
    }
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
}

HTTPTransfer::~HTTPTransfer() {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

char* HTTPTransfer::get(const char* url) {
    char* receivedData = NULL;
    char** receivedDataPtr = &receivedData;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, receivedDataPtr);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dataReceived);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    perform();
    return *receivedDataPtr;
}

void HTTPTransfer::get(const char* url, const char* filename) {
    FILE* downloadFile = fopen(filename, "wb");
    get(url, downloadFile);
    fclose(downloadFile);
}

void HTTPTransfer::get(const char* url, FILE* file) {
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    perform();
}

void HTTPTransfer::perform() {
    CURLcode res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK) {
        printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        throw "curl_easy_perform() failed";
    }
}

long HTTPTransfer::getStatusCode() {
    long statusCode;
    CURLcode res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
    if (res != CURLE_OK) {
        printf("curl_easy_getinfo() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        throw "couldn't retrieve status code";
    }
    return statusCode;
}

char *HTTPTransfer::escape(const char *toEscape) {
    return curl_easy_escape(curl, toEscape, 0);
}

void HTTPTransfer::free(char *escaped) {
    return curl_free(escaped);
}

size_t HTTPTransfer::dataReceived(char* newData, size_t dataSize, size_t dataLen, char** receivedDataPtr) {
    char* receivedData = *receivedDataPtr;
    size_t receivedLen;
    if (receivedData) {
        receivedLen = strlen(receivedData);
    } else {
        receivedLen = 0;
    }

    char* newBuf = new char[receivedLen + dataSize * dataLen + 1];
    if (newBuf == NULL) {
        return 0;
    }
    if (receivedData) {
        strcpy(newBuf, receivedData);
    }
    strncpy(newBuf + receivedLen, newData, dataSize * dataLen);
    newBuf[receivedLen + dataSize * dataLen] = '\0';
    SAFE_DELETE_ARRAY(receivedData);
    receivedData = newBuf;
    (*receivedDataPtr) = receivedData;
    return dataSize * dataLen;
}

int HTTPTransfer::progress(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    if (dltotal) {
        printf("Downloaded %3.0f\%\r", (double)dlnow / dltotal * 100);
    } else if (ultotal) {
        printf("Uploaded %3.0f\%\r", (double)ulnow / ultotal * 100);
    }
    return 0;
}
