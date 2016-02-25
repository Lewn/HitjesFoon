#include "GUIWtApplication.h"

GUIWtApplication::GUIWtApplication(const WEnvironment& env) : WApplication(env) {
    setTitle("HitjesFoon");

    app->messageResourceBundle().use(app->appRoot() + "resources/language/strings.xml");
    app->messageResourceBundle().use(app->appRoot() + "resources/templates/main.xml");

    new WPushButton("Greet me.", root());
//    setCssTheme("polished");
    Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(this);
    bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
    bootstrapTheme->setResponsive(true);
    setTheme(bootstrapTheme);

    // load the default bootstrap3 (sub-)theme
    useStyleSheet("resources/themes/bootstrap/3/bootstrap-theme.min.css");

    messageResourceBundle().use(appRoot() + "language/text");

    WTemplate *t = new WTemplate(WString::tr("template-hitjesfoon-home"), root());
}

GUIWtApplication::~GUIWtApplication() {

}


