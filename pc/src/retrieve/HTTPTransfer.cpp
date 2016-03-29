#include "retrieve/HTTPTransfer.h"

HTTPTransfer::HTTPTransfer(GUI &gui) : gui(gui) {
}

HTTPTransfer::~HTTPTransfer() {
}

string HTTPTransfer::get(const char *url) {
    string ret;
    char buf[1024];
    string cmd = "curl \"";
    cmd += url;
    // Pipe progress away
#ifdef __WIN32
    cmd += "\" 2> nul";
#else
    cmd += "\" 2> /dev/null";
#endif // __WIN32
    auto cmdptr = cmdasync(cmd);
    while (!feof(cmdptr.get())) {
        if (fgets(buf, sizeof(buf), cmdptr.get()) != NULL) {
            // Append buffer to return string
            ret += buf;
        }
    }
    return ret;
}

void HTTPTransfer::get(const char *url, const char *filename) {
    char buf[1024];
    string cmd = "curl -o ";
    cmd += filename;
    cmd += " \"";
    cmd += url;
#ifdef __WIN32
    cmd += "\" 2> nul";
#else
    cmd += "\" 2> /dev/null";
#endif // __WIN32
    auto cmdptr = cmdasync(cmd);
    while (!feof(cmdptr.get())) {
        if (fgets(buf, sizeof(buf), cmdptr.get()) != NULL) {
            // Ignore output
        }
    }
}

string HTTPTransfer::escape(const char *toEscape) {
    string value(toEscape);
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;
    for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        string::value_type c = (*i);
        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }
        // Any other characters are percent-encoded
        escaped << uppercase;
        escaped << '%' << setw(2) << int((unsigned char) c);
        escaped << nouppercase;
    }
    return escaped.str();
}
