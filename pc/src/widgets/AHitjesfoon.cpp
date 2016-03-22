#include "AHitjesfoon.h"

AHitjesfoon::AHitjesfoon(const WEnvironment& env, GUI &gui, Persistence &persistence) : WApplication(env), gui(gui), persistence(persistence) {
    setTitle("Hitjesfoon");
    enableUpdates(true);

    home = new WHome(gui, persistence);
    hitjesList = new WHitjesList(gui, persistence);
    w404 = new W404(gui, persistence);


    WNavigationBar *navigation = new WNavigationBar();
    navigation->addStyleClass("main-nav");

    navigation->setTitle("Hitjesfoon", WLink(WLink::InternalPath, "/"));
    navigation->setResponsive(true);

    // TODO no stacked widget (only has maximum height?)
    WStackedWidget *contentsStack = new WStackedWidget();

    // Setup a Left-aligned menu.
    menu = new WMenu(contentsStack);
    navigation->addMenu(menu);

    homeItem = new WMenuItem("Home", home);
    homeItem->setPathComponent("/");
    homeItem->setLink(WLink(WLink::InternalPath, "/"));
    menu->addItem(homeItem);
    hitjesListItem = new WMenuItem("Hitjes list", hitjesList);
    hitjesListItem->setPathComponent("/hitjeslist");
    hitjesListItem->setLink(WLink(WLink::InternalPath, "/hitjeslist"));
    menu->addItem(hitjesListItem);

    root()->addWidget(navigation);
    root()->addWidget(contentsStack);
    root()->addWidget(w404);

    // TODO add as default menu
    menu->addStyleClass("navbar-nav");
    for (WMenuItem *item : menu->items()) {
        if (internalPath() == item->pathComponent()) {
            // we opened at this item, set it as active
            item->addStyleClass("active");
            break;
        }
    }
    navigation->resolveWidget("title-link")->addStyleClass("navbar-brand");
    navigation->resolveWidget("collapse-button")->addStyleClass("navbar-toggle");
    navigation->resolveWidget("expand-button")->addStyleClass("navbar-toggle");
    navigation->resolveWidget("contents")->setHidden(false);

    internalPathChanged().connect(this, onPathChange);
    onPathChange();
}

AHitjesfoon::~AHitjesfoon() {
    SAFE_DELETE(w404);
    SAFE_DELETE(hitjesList);
    SAFE_DELETE(home);
}


void AHitjesfoon::onPathChange() {
    bool notfound = false;
    if (internalPath() == "/" || internalPath() == "/home") {
        // Swap page for homepage
        setInternalPath("/");
        menu->select(homeItem);
    } else if (internalPath() == "/hitjeslist") {
        // Swap page for hitjeslist page
        menu->select(hitjesListItem);
    } else {
        // Show 404
        menu->select(0);
        notfound = true;
    }
    w404->setHidden(notfound);
    menu->contentsStack()->setHidden(!notfound);
}
