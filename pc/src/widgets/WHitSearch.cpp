#include "widgets/WHitSearch.h"

WHitSearch::WHitSearch(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WFilledTemplate(WString::tr("template-hitjesfoon-home-input"), parent), gui(gui), persistence(persistence), searchSP(NULL) {
    buildWidget();
}

WHitSearch::WHitSearch(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence), searchSP(NULL) {
    buildWidget();
}

WHitSearch::~WHitSearch() {

}

void WHitSearch::buildWidget() {
    searchInputText = new WLineEdit();
    WLengthValidator *lenValidator = new WLengthValidator(1, 1000);
    searchInputText->setValidator(lenValidator);

    WPushButton *searchOkBtn = new WPushButton(WString::tr("strings-searchinput-ok"));
    searchHelpText = new WText();

    initSearchSuggestions();
    searchSP->forEdit(searchInputText);

    bindWidget("searchinput", searchInputText);
    bindWidget("searchok", searchOkBtn);
    bindWidget("searchhelp", searchHelpText);

    searchInputText->keyPressed().connect(std::bind([ = ] (const WKeyEvent & e) {
        if (e.key() == Key_Enter) {
            processSearchInput();
        }
    }, std::placeholders::_1));
    searchOkBtn->clicked().connect(this, &WHitSearch::processSearchInput);

    // Listen to all persistence changes
    persistence.onChangeCallback(boost::bind(&WHitSearch::onPersistenceChange, this, _1));
}

void WHitSearch::onPersistenceChange(const string &key) {
}


void WHitSearch::processSearchInput() {
    string ser = searchInputText->text().toUTF8();
    if (searchInputText->validate() == WValidator::Valid) {
        try {
            int num = stoi(ser);
            gui.inputPhoneNum(num);
            searchInputText->setText("");
        } catch (...) {
            string helpText = "Input '" + ser + "' is not a valid input (must start with number)";
            searchHelpText->setText(helpText);
            gui.printlevel(LERROR, "%s\n", helpText.c_str());
        }
    } else {
        string helpText = "Invalid input, '" + ser + "'. Cannot request too large.";
        searchHelpText->setText(helpText);
        gui.printlevel(LERROR, "Invalid input, '%s'\nCannot request too large\n", ser.c_str());
    }
}


void WHitSearch::initSearchSuggestions() {
    if (searchSP == NULL) {
        WSuggestionPopup::Options searchOptions;
        searchOptions.highlightBeginTag = "<span class=\"highlight\">";
        searchOptions.highlightEndTag = "</span>";
        searchOptions.whitespace = " \\n";
        searchOptions.wordSeparators = "-., \"@\\n;";

        searchSP = new WSuggestionPopup(
            WSuggestionPopup::generateMatcherJS(searchOptions),
            WSuggestionPopup::generateReplacerJS(searchOptions),
            searchInputText);

        searchSP->setMaximumSize(WLength::Auto, WLength(200, WLength::Unit::Pixel));

        for (unsigned int i = 1; i < 1000; i++) {
            // loop all hitjes
            Hitje hitje = persistence.getHitjeData().getVal(to_string(i));
            if (hitje) {
                searchSP->addSuggestion(hitje.toString());
            }
        }
    }
}
