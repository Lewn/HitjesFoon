#ifndef YOUTUBEDL_H
#define YOUTUBEDL_H

#include <regex>

#include "Structs.h"
#include "Tools.h"
#include "retrieve/downloader/Downloader.h"
#include "gui/GUI.h"
#include "retrieve/HTTPTransfer.h"
#include "JsonParser.h"

using namespace std;

class YoutubeDL : public Downloader {
public:
    using Downloader::download;
    YoutubeDL(GUI &gui);
    virtual ~YoutubeDL();

    bool download(Hitje &hitje, YoutubeScraperData &scraperData) override;
protected:
    void performCmd(Hitje &hitje, const string &cmd);
    float reformat(const string &val, const string &type);
private:
};

#endif // YOUTUBEDL_H
