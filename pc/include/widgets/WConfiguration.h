#ifndef WCONFIGURATION_H
#define WCONFIGURATION_H

#include "Structs.h"
#include "gui/GUI.h"
#include "persistence/Persistence.h"
#include "widgets/WFilledTemplate.h"

#include <Wt/WPushButton>

using namespace std;
using namespace Wt;

class WConfiguration : public WFilledTemplate {
public:
    WConfiguration(GUI &gui, Persistence &persistence, WContainerWidget *parent = NULL);
    WConfiguration(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent = NULL);
    virtual ~WConfiguration();

    virtual void buildWidget() override;
protected:
    GUI &gui;
    Persistence &persistence;
private:
};

#endif // WCONFIGURATION_H
