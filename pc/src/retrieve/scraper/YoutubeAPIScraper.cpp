#include "retrieve/scraper/YoutubeAPIScraper.h"

YoutubeAPIScraper::YoutubeAPIScraper(GUI &gui) : Scraper(gui) {
}

YoutubeAPIScraper::~YoutubeAPIScraper() {
}


bool YoutubeAPIScraper::scrape(YoutubeScraperData &scraperData) {
    // we only need the total amount of results and each results id (kind and channelId/videoId)
    gui.printlevel(LBGINFO, "Searching youtube api\n");
    string json = makeRequest("id,snippet", "pageInfo/totalResults,items(id,snippet/title)", 1, scraperData.query.c_str());
    JsonParser result(gui);
    result.parse(json.c_str());
    if (result.getTotalResults() == 0) {
        gui.printlevel(LWARNING, "Nothing found while searching\n");
    }

    string title = result.getVideoTitles().at(0);
    gui.printlevel(LBGINFO, "Done searching and found %s\n", title.c_str());

    scraperData.videoId = result.getVideoIds().at(0);
    return true;
}

string YoutubeAPIScraper::makeRequest(const char *part, const char *fields, unsigned char maxResults, const char *extra) {
    string baseUrl = "https://www.googleapis.com/youtube/v3/search";
    baseUrl += "?q=" + transfer.escape(extra) + "&type=video";
    baseUrl += "&part=";
    baseUrl += part;
    if (fields != NULL) {
        baseUrl += "&fields=";
        baseUrl += fields;
    }
    baseUrl += "&key=";
    baseUrl += DEVELOPER_KEY;
    baseUrl += "&maxResults=";
    baseUrl += to_string(maxResults);

    gui.printlevel(LDEBUG, "\n%s\n", baseUrl.c_str());
    string jsonString = transfer.get(baseUrl.c_str());
    gui.printlevel(LDEBUG, "%s", jsonString.c_str());
    return jsonString;
}
