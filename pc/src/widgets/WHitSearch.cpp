#include "widgets/WHitSearch.h"

WHitSearch::WHitSearch(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WHitSearch(gui, persistence, WString::tr("template-hitjesfoon-home-input"), parent) {}

WHitSearch::WHitSearch(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence), searchSP(NULL) {
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

    searchInputText->keyPressed().connect([ & ] (const WKeyEvent & e) {
        if (e.key() == Key_Enter) {
            processSearchInput();
        }
    });
    searchSP->activated().connect(this, &WHitSearch::processSearchInput);
    searchOkBtn->clicked().connect(this, &WHitSearch::processSearchInput);

    // Listen to all persistence changes
    persistence.onChangeCallback(boost::bind(&WHitSearch::onPersistenceChange, this, _1));
}

void WHitSearch::onPersistenceChange(const string &key) {
    try {
        // Try to convert to integer
        stoi(key);
        // Try to retrieve as hitje
        persistence.getHitjeData().getVal(key);
    } catch (std::invalid_argument e) {
        // Couldn't convert, just ignore
        return;
    } catch (const char *e) {
        // Wasn't a hitje, ignore
        return;
    }
    // TODO can do this without clearing? (Maybe extend class)
    searchSP->clearSuggestions();
    addSuggestions();
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
        searchOptions.listSeparator = 0;

        searchSP = new WSuggestionPopup(
            WSuggestionPopup::generateMatcherJS(searchOptions),
            WSuggestionPopup::generateReplacerJS(searchOptions),
            searchInputText);

        searchSP->setMaximumSize(WLength::Auto, WLength(200, WLength::Unit::Pixel));

        addSuggestions();
    }
}

void WHitSearch::addSuggestions() {
    for (unsigned int i = 1; i < 1000; i++) {
        // loop all hitjes
        const Hitje &hitje = persistence.getHitjeData().getVal(to_string(i));
        if (hitje) {
            searchSP->addSuggestion(hitje.toString());
        }
    }
}
