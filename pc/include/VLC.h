#ifndef VLC_H
#define VLC_H

#define PHONE_DEVICE ""
#define SPEAKER_DEVICE ""

#include "vlc/vlc.h"
#include "Tools.h"
#include "Config.h"

#include <string.h>
#include <regex>

using namespace std;

class VLC {
public:
    static VLC *getInstance();
    static void deleteInstance();

    static void setConfig(Config *config);

    string getDefaultPhoneOutput();
    string getDefaultSpeakerOutput();
    string getDefaultPhoneDevice();
    string getDefaultSpeakerDevice();

    libvlc_media_t *newMediaFromPath(const char *path);
    libvlc_media_list_t *newMediaList();
    libvlc_media_player_t *newMediaPlayer();
    libvlc_media_list_player_t *newMediaListPlayer();

    static void release(libvlc_media_t *media);
    static void release(libvlc_media_list_t *mediaList);
    static void release(libvlc_media_player_t *mediaPlayer);
    static void release(libvlc_media_list_player_t *mediaListPlayer);

protected:
    libvlc_instance_t *libvlcInstance;
    struct deviceoutput {
        string output;
        string device;
        string deviceDescription;
    };

    vector<deviceoutput> getDeviceOutputs(vector<string> &outputNames);
    void tryConfig(const vector<deviceoutput> &outputs);
    VLC();
    virtual ~VLC();
private:
    string phoneDevice;
    string speakerDevice;
    string phoneOutput;
    string speakerOutput;

    static Config *config;
    static VLC *instance;

    VLC(const VLC &that) = delete;
};

#endif // VLC_H
