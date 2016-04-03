#include "Tools.h"


char *trimLeft(char *toTrim) {
    while (*toTrim && !isgraph(*toTrim)) {
        toTrim++;
    }
    return toTrim;
}

char *trimRight(char *toTrim) {
    char *tmp = toTrim;
    toTrim += strlen(toTrim) + 1;
    while (--toTrim >= tmp && !isgraph(*(toTrim)));
    *(toTrim + 1) = 0;
    return tmp;
}

char *trim(char *toTrim) {
    return trimLeft(trimRight(toTrim));
}

// trim from start
std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

char *getAbsolutePath(const char *listFilePath, int pathLen, const char *filename) {
    // we need the path and the filename in one string
    int totalLen = pathLen + strlen(filename) + 1;
    char *absolute = new char[totalLen];
    // copy path in the new filename
    strncpy(absolute, listFilePath, pathLen);
    // and copy the name itself
    strcpy(absolute + pathLen, filename);
    return absolute;
}

void filesystemSafe(char *str) {
    // replace + with spaces in title
    const char *filechars = "~!@#^&()-_+=[]{};'., ";
    for (; *str; str++) {
        if (!((*str >= 'A' && *str <= 'Z') || (*str >= 'a' && *str <= 'z') || (*str >= '0' && *str <= '9') || strchr(filechars, *str))) {
            *str = ' ';
        }
    }
}

void filesystemSafe(string &str) {
    // replace + with spaces in title
    const char *filechars = "~!@#^&()-_+=[]{};'., ";
    for (std::string::iterator it = str.begin(); it != str.end(); it++) {
        if (!((*it >= 'A' && *it <= 'Z') || (*it >= 'a' && *it <= 'z') || (*it >= '0' && *it <= '9') || strchr(filechars, *it))) {
            str.erase(it);
            it--;
        }
    }
}

std::shared_ptr<FILE> cmdasync(const string &cmd) {
#ifdef __WIN32
    shared_ptr<FILE> cmdptr(_popen(cmd.c_str(), "r"), _pclose);
#else
    shared_ptr<FILE> cmdptr(popen(cmd.c_str(), "r"), pclose);
#endif // __WIN32
    if (!cmdptr) {
        throw "Could not open command asynchronously";
    }
    return cmdptr;
}



#ifndef __WIN32__
struct termios orig_termios;

void reset_terminal_mode() {
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode() {
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    // Register cleanup handlers
    atexit(reset_terminal_mode);
    set_terminate(reset_terminal_mode);

    // Actually set the terminal mode
    cfmakeraw(&new_termios);
    new_termios.c_lflag &= (~ICANON);
    tcsetattr(0, TCSANOW, &new_termios);
}


int getch() {
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        c = 0;
    }
    return c;
}

int kbhit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(0, &fds);
}
#endif // __WIN32__



void toolsInit() {
#ifndef __WIN32__
    set_conio_terminal_mode();
#endif // __WIN32__
}

void toolsDeinit() {
#ifndef __WIN32__
    reset_terminal_mode();
#endif // __WIN32__
    }
