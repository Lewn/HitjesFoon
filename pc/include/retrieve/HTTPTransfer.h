#ifndef HTTPTRANSFER_H
#define HTTPTRANSFER_H

#include <chrono>
#include <thread>
#include "gui/GUI.h"
#include <curl/curl.h>
#include "Tools.h"

class HTTPTransfer {
public:
    HTTPTransfer(GUI &gui);
    HTTPTransfer(const HTTPTransfer &that);
    ~HTTPTransfer();

    void sendReceive();
    string get(const char *url);
    void get(const char *url, const char *filename);
    void get(const char *url, FILE *file);
    long getStatusCode();

    string escape(const char *toEscape);
protected:
    GUI &gui;
    CURL *curl = NULL;

    void perform();

    static size_t dataReceived(char *ptr, size_t dataSize, size_t dataLen, char **receivedDataPtr);
    static int progress(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
private:
};

#endif // HTTPTRANSFER_H
