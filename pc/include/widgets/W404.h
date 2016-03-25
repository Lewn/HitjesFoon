#ifndef W404_H
#define W404_H

#include "gui/GUI.h"
#include "widgets/WFilledTemplate.h"
#include "persistence/Persistence.h"

using namespace std;
using namespace boost;
using namespace Wt;

class W404 : public WFilledTemplate {
public:
    W404(GUI &gui, Persistence &persistence, WContainerWidget *parent = NULL);
    W404(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent = NULL);
    virtual ~W404();
protected:
    GUI &gui;
    Persistence &persistence;

private:
};

#endif // W404_H
