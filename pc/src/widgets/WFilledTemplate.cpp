#include "WFilledTemplate.h"

WFilledTemplate::WFilledTemplate(WContainerWidget *parent) : WTemplate(parent) {

}

WFilledTemplate::WFilledTemplate(const WString &text, WContainerWidget *parent) : WTemplate(text, parent) {

}

WFilledTemplate::~WFilledTemplate() {

}

void WFilledTemplate::handleUnresolvedVariable(const string &varName, const vector<WString> &args, ostream &result) {
    if (varName.substr(0, 8) == "strings-") {
        format(result, WString::tr(varName));
    }
}
