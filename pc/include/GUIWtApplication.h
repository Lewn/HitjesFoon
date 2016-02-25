#ifndef GUIWTAPPLICATION_H
#define GUIWTAPPLICATION_H

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WContainerWidget>
#include <Wt/WTemplate>
#include <Wt/WCssTheme>
#include <Wt/WBootstrapTheme>

using namespace Wt;

class GUIWtApplication : public WApplication {
public:
    GUIWtApplication(const WEnvironment& env);
    virtual ~GUIWtApplication();

protected:
private:
};

#endif // GUIWTAPPLICATION_H
