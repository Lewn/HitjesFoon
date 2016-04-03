#ifndef VLC_H
#define VLC_H


#include "vlc/vlc.h"
#include "Tools.h"
#include "gui/GUI.h"
#include "Config.h"

#include <string.h>
#include <regex>



#define VLC_EVENTS_ALL          VLC_EVENTS_MEDIA, VLC_EVENTS_MEDIAPLAYER, VLC_EVENTS_MEDIALIST, VLC_EVENTS_LISTPLAYER, VLC_EVENTS_DISCOVER, VLC_EVENTS_VLM

#define VLC_EVENTS_MEDIA        libvlc_MediaMetaChanged, libvlc_MediaSubItemAdded, libvlc_MediaDurationChanged, libvlc_MediaParsedChanged, libvlc_MediaFreed, libvlc_MediaStateChanged, libvlc_MediaSubItemTreeAdded, libvlc_MediaPlayerPausableChanged, libvlc_MediaPlayerScrambledChanged
#define VLC_EVENTS_MEDIAPLAYER  libvlc_MediaPlayerMediaChanged, libvlc_MediaPlayerNothingSpecial, libvlc_MediaPlayerOpening, libvlc_MediaPlayerBuffering, libvlc_MediaPlayerPlaying, libvlc_MediaPlayerPaused, libvlc_MediaPlayerStopped, libvlc_MediaPlayerForward, libvlc_MediaPlayerBackward, libvlc_MediaPlayerEndReached, libvlc_MediaPlayerEncounteredError, libvlc_MediaPlayerTimeChanged, libvlc_MediaPlayerPositionChanged, libvlc_MediaPlayerSeekableChanged, libvlc_MediaPlayerTitleChanged, libvlc_MediaPlayerSnapshotTaken, libvlc_MediaPlayerLengthChanged, libvlc_MediaPlayerVout
#define VLC_EVENTS_MEDIALIST    libvlc_MediaListItemAdded, libvlc_MediaListWillAddItem, libvlc_MediaListItemDeleted, libvlc_MediaListWillDeleteItem, libvlc_MediaListViewItemAdded, libvlc_MediaListViewWillAddItem, libvlc_MediaListViewItemDeleted, libvlc_MediaListViewWillDeleteItem
#define VLC_EVENTS_LISTPLAYER   libvlc_MediaListPlayerPlayed, libvlc_MediaListPlayerNextItemSet, libvlc_MediaListPlayerStopped
#define VLC_EVENTS_DISCOVER     libvlc_MediaDiscovererStarted, libvlc_MediaDiscovererEnded
#define VLC_EVENTS_VLM          libvlc_VlmMediaAdded, libvlc_VlmMediaRemoved, libvlc_VlmMediaChanged, libvlc_VlmMediaInstanceStarted, libvlc_VlmMediaInstanceStopped, libvlc_VlmMediaInstanceStatusInit, libvlc_VlmMediaInstanceStatusOpening, libvlc_VlmMediaInstanceStatusPlaying, libvlc_VlmMediaInstanceStatusPause, libvlc_VlmMediaInstanceStatusEnd, libvlc_VlmMediaInstanceStatusError


using namespace std;

class VLC {
public:
    static VLC *getInstance();
    static void deleteInstance();

    static void setGUI(GUI *gui);
    static void setConfig(Config *config);

    string getDefaultPhoneOutput();
    string getDefaultSpeakerOutput();
    string getDefaultPhoneDevice();
    string getDefaultSpeakerDevice();

    libvlc_media_t *newMediaFromPath(const char *path);
    libvlc_media_list_t *newMediaList();
    libvlc_media_player_t *newMediaPlayer();
    libvlc_media_list_player_t *newMediaListPlayer();

    static void retain(libvlc_media_t *media);
    static void retain(libvlc_media_list_t *mediaList);
    static void retain(libvlc_media_player_t *mediaPlayer);
    static void retain(libvlc_media_list_player_t *mediaListPlayer);

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

    static GUI *gui;
    static Config *config;
    static VLC *instance;

    VLC(const VLC &that) = delete;
};

#endif // VLC_H
