#include "WConfiguration.h"

WConfiguration::WConfiguration(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WConfiguration(gui, persistence, WString::tr("template-hitjesfoon-configuration"), parent) {}

WConfiguration::WConfiguration(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {
}

WConfiguration::~WConfiguration() {
}


void WConfiguration::buildWidget() {

    WPushButton *refreshBtn = new WPushButton(WString::tr("strings-configuration-refresh"));
    bindWidget("refresh", refreshBtn);
    WPushButton *applicationRestartBtn = new WPushButton(WString::tr("strings-configuration-restart"));
    bindWidget("application-restart", applicationRestartBtn);
    WPushButton *applicationExitBtn = new WPushButton(WString::tr("strings-configuration-exit"));
    bindWidget("application-exit", applicationExitBtn);

    refreshBtn->clicked().connect(std::bind([ & ] () {
        gui.addInput(INPUT_REFRESH);
    }));
    applicationRestartBtn->clicked().connect(std::bind([ & ] () {
        gui.addInput(INPUT_RESTART);
    }));
    applicationExitBtn->clicked().connect(std::bind([ & ] () {
        gui.addInput(INPUT_EXIT);
    }));
}
