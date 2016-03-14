#ifndef WFILLEDTEMPLATE_H
#define WFILLEDTEMPLATE_H

#include <Wt/WTemplate>
#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/WLocalizedStrings>
#include <Wt/WApplication>

#include <string>
#include <vector>

using namespace std;
using namespace Wt;


class WFilledTemplate : public WTemplate {
public:
    WFilledTemplate(WContainerWidget *parent = NULL);
    WFilledTemplate(const WString &text, WContainerWidget *parent = NULL);
    virtual ~WFilledTemplate();

    void handleUnresolvedVariable(const string &varName, const vector<WString> &args, ostream &result);
protected:
private:
};

#endif // WFILLEDTEMPLATE_H
