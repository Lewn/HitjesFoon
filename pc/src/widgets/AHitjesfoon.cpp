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

    WStackedWidget *contentsStack = new WStackedWidget();
    contentsStack->addWidget(w404);

    // Setup a Left-aligned menu.
    menu = new WMenu(contentsStack);
    navigation->addMenu(menu);

    homeItem = new WMenuItem("Home", home);
    homeItem->setPathComponent("/");
    menu->addItem(homeItem);
    hitjesListItem = new WMenuItem("Hitjes list", hitjesList);
    hitjesListItem->setPathComponent("/hitjeslist");
    menu->addItem(hitjesListItem);
    w404Item = new WMenuItem("404", w404);
    w404Item->setPathComponent("/404");
//    menu->setItemHidden(w404Item, true);
    menu->addItem(w404Item);

    WVBoxLayout *layout = new WVBoxLayout(root());
    layout->addWidget(navigation);
    layout->addWidget(contentsStack, 1);
//    layout->setContentsMargins(0, 0, 0, 0);

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

    persistence.onChangeCallback(std::bind([ = ] (const string & key) {
        triggerUpdate();
    }, std::placeholders::_1));
}

AHitjesfoon::~AHitjesfoon() {
    SAFE_DELETE(w404);
    SAFE_DELETE(hitjesList);
    SAFE_DELETE(home);
}


void AHitjesfoon::onPathChange() {
    printf("Changed internal path to %s\n", internalPath().c_str());
    if (internalPath() == "/" || internalPath() == "/home") {
        // Swap page for homepage
        menu->select(homeItem);
    } else if (internalPath() == "/hitjeslist") {
        // Swap page for hitjeslist page
        menu->select(hitjesListItem);
    } else {
        // Show 404
        printf("Found 404 path\n");
        menu->select(w404Item);
    }
}
