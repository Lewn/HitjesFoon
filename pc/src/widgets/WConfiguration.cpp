#include "widgets/WConfiguration.h"

WConfiguration::WConfiguration(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WConfiguration(gui, persistence, WString::tr("template-hitjesfoon-configuration"), parent) {}

WConfiguration::WConfiguration(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {
}

WConfiguration::~WConfiguration() {
}


void WConfiguration::buildWidget() {
    WPushButton *applicationRestartBtn = new WPushButton(WString::tr("strings-configuration-restart"));
    bindWidget("application-restart", applicationRestartBtn);
    WPushButton *applicationExitBtn = new WPushButton(WString::tr("strings-configuration-exit"));
    bindWidget("application-exit", applicationExitBtn);

    WPushButton *refreshBtn = new WPushButton(WString::tr("strings-configuration-refresh"));
    bindWidget("refresh", refreshBtn);
    WPushButton *resetListBtn = new WPushButton(WString::tr("strings-configuration-reset-list"));
    bindWidget("reset-list", resetListBtn);

    applicationRestartBtn->clicked().connect(std::bind([ & ] () {
        gui.addInput(INPUT_RESTART);
    }));
    applicationExitBtn->clicked().connect(std::bind([ & ] () {
        gui.addInput(INPUT_EXIT);
    }));

    resetListBtn->clicked().connect(std::bind([ & ] () {
        gui.addInput(INPUT_RESET_LIST);
    }));
    refreshBtn->clicked().connect(std::bind([ & ] () {
        gui.addInput(INPUT_REFRESH);
    }));
}
