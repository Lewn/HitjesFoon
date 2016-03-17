#ifndef WHITJESLIST_H
#define WHITJESLIST_H


#include "gui/GUI.h"
#include "widgets/WFilledTemplate.h"
#include "Persistence.h"

using namespace std;
using namespace boost;
using namespace Wt;

class WHitjesList : public WFilledTemplate {
public:
    WHitjesList(GUI &gui, Persistence &persistence, WContainerWidget *parent = NULL);
    WHitjesList(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent = NULL);
    virtual ~WHitjesList();
protected:
    GUI &gui;
    Persistence &persistence;

    void buildWidget();
private:
};

#endif // WHITJESLIST_H
