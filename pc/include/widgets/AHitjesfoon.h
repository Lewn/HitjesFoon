#ifndef AHITJESFOON_H
#define AHITJESFOON_H

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WNavigationBar>
#include <Wt/WStackedWidget>
#include <Wt/WMenu>
#include <Wt/WVBoxLayout>
#include <Wt/WLink>

#include "Tools.h"
#include "widgets/WHome.h"
#include "widgets/WHitjesList.h"
#include "widgets/W404.h"

using namespace std;
using namespace boost;
using namespace Wt;

class AHitjesfoon : public WApplication {
public:
    AHitjesfoon(const WEnvironment& env, GUI &gui, Persistence &persistence);
    virtual ~AHitjesfoon();

    void buildWidget();
    void onPathChange();
protected:
    GUI &gui;
    Persistence &persistence;

    WHome *home = NULL;
    WHitjesList *hitjesList = NULL;
    W404 *w404 = NULL;
    WMenu *menu = NULL;
    WMenuItem *homeItem = NULL;
    WMenuItem *hitjesListItem = NULL;
private:
};

#endif // AHITJESFOON_H
