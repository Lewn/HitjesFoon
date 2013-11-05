#include "Tools.h"


char* trimLeft(char* toTrim) {
    while (!isgraph(*toTrim)) {
        toTrim++;
    }
    return toTrim;
}

char* trimRight(char* toTrim) {
    char* tmp = toTrim;
    toTrim += strlen(toTrim);
    while (!isgraph(*(--toTrim))) {
        *toTrim = 0;
    }
    return tmp;
}

char* trim(char* toTrim) {
    return trimLeft(trimRight(toTrim));
}
