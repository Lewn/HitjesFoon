#ifndef WHITJESLIST_H
#define WHITJESLIST_H

#include <Wt/WContainerWidget>
#include <Wt/WText>

#include "gui/GUI.h"
#include "widgets/WFilledTemplate.h"
#include "widgets/WHitje.h"
#include "persistence/Persistence.h"

using namespace std;
using namespace boost;
using namespace Wt;

class WHitjesList : public WFilledTemplate {
public:
    WHitjesList(GUI &gui, Persistence &persistence, WContainerWidget *parent = NULL);
    WHitjesList(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent = NULL);
    virtual ~WHitjesList();

    void buildWidget();
    void onPersistenceChange(const string &key);
protected:
    GUI &gui;
    Persistence &persistence;

    WContainerWidget *hitjesContainer;

private:
};

#endif // WHITJESLIST_H
