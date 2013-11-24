#ifndef HTTPTRANSFER_H
#define HTTPTRANSFER_H

#include <curl/curl.h>
#include "Tools.h"

class HTTPTransfer {
public:
    HTTPTransfer();
    ~HTTPTransfer();

    void sendReceive();
    char* get(const char* url);
    void get(const char* url, const char* filename);
    void get(const char* url, FILE* file);
    long getStatusCode();

    char *escape(const char *toEscape);
    void free(char *escaped);
protected:
    CURL* curl;

    void perform();

    static size_t dataReceived(char *ptr, size_t dataSize, size_t dataLen, char** receivedDataPtr);
    static int progress(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
private:
};

#endif // HTTPTRANSFER_H
