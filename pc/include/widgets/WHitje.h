#ifndef WHITJE_H
#define WHITJE_H

#include <sstream>
#include <iomanip>

#include <Wt/WApplication>
#include <Wt/WDialog>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WProgressBar>
#include <Wt/WIntValidator>
#include <Wt/WValidator>

#include "Structs.h"
#include "Hitje.h"
#include "gui/GUI.h"
#include "persistence/Persistence.h"
#include "widgets/WFilledTemplate.h"
#include "retrieve/Retriever.h"

using namespace std;
using namespace boost;
using namespace Wt;

class WHitje : public WFilledTemplate {
public:
    WHitje(GUI &gui, Persistence &persistence, int hitIndex, WContainerWidget *parent = NULL);
    WHitje(GUI &gui, Persistence &persistence, const WString &text, int hitIndex, WContainerWidget *parent = NULL);
    virtual ~WHitje();

    void buildWidget();
    void onPersistenceChange(const string &key);
    void showDialog(bool artist);
    bool download();
protected:
    GUI &gui;
    Persistence &persistence;
    WDialog *editDialog = NULL;
    mutex dlmutex;

    int hitIndex;
    WText *artistText;
    WText *titleText;
    WText *infoText;
    WPushButton *downloadBtn;
    WProgressBar *pgBar;

    WLineEdit *editIndex, *editArtist, *editTitle;

    void updateHitje();
private:
};

#endif // WHITJE_H
