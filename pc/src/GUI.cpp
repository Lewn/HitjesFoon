#include "GUI.h"

GUI::GUI(PRINT_LEVEL msglevel) : msglevel(msglevel) {
}

void GUI::setMsglevel(PRINT_LEVEL msglevel) {
    this->msglevel = msglevel;
}

PRINT_LEVEL GUI::getMsglevel() {
    return msglevel;
}
