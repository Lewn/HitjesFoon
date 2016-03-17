#ifndef WHITSEARCH_H
#define WHITSEARCH_H

#include <Wt/WLengthValidator>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WSuggestionPopup>
#include <Wt/WText>
#include <Wt/WEvent>

#include "gui/GUI.h"
#include "widgets/WFilledTemplate.h"
#include "Persistence.h"
#include "InputProcessor.h"

using namespace std;
using namespace boost;
using namespace Wt;


class WHitSearch : public WFilledTemplate {
public:
    WHitSearch(GUI &gui, Persistence &persistence, WContainerWidget *parent = NULL);
    WHitSearch(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent = NULL);
    ~WHitSearch();

    void onPersistenceChange(const string &key);
    void processSearchInput();

protected:
    GUI &gui;
    Persistence &persistence;

    WLineEdit *searchInputText;
    WText *searchHelpText;
    WSuggestionPopup *searchSP;

    void buildWidget();

    void initSearchSuggestions();
private:
};

#endif // WHITSEARCH_H
