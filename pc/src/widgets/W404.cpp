#include "widgets/W404.h"


W404::W404(GUI &gui, Persistence &persistence, WContainerWidget *parent) : W404(gui, persistence, WString::tr("template-hitjesfoon-404"), parent) {}

W404::W404(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {
}

W404::~W404() {

}


void W404::buildWidget() {
}
