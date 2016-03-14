#include "widgets/WFilledTemplate.h"

WFilledTemplate::WFilledTemplate(WContainerWidget *parent) : WTemplate(parent) {

}

WFilledTemplate::WFilledTemplate(const WString &text, WContainerWidget *parent) : WTemplate(text, parent) {

}

WFilledTemplate::~WFilledTemplate() {

}

void WFilledTemplate::handleUnresolvedVariable(const string &varName, const vector<WString> &args, ostream &result) {
    string var;
    WApplication *app = WApplication::instance();
    if (!app->localizedStrings()->resolveKey(varName, var)) {
        // key varName not found in locale, skip it (let Wt handle)
        return;
    }

    if (varName.substr(0, 8) == "strings-") {
        format(result, var);
    }else if (varName.substr(0, 9) == "template-") {
        renderTemplateText(result, var);
    }
}
