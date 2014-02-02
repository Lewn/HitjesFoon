#ifndef VLC_H
#define VLC_H

#define PHONE_DEVICE ""
#define SPEAKER_DEVICE ""

#include "vlc/vlc.h"
#include "Tools.h"
#include <string.h>
#include <conio.h>

using namespace std;

class VLC {
public:
    static VLC* getInstance();
    static void deleteInstance();
    static void setPhoneDevice(const char* phoneDevice);
    static void setSpeakerDevice(const char* speakerDevice);

    VLC();
    virtual ~VLC();

    const char* getDefaultPhoneOutput();
    const char* getDefaultSpeakerOutput();
    const char* getDefaultPhoneDevice();
    const char* getDefaultSpeakerDevice();

    libvlc_media_t* newMediaFromPath(const char* path);
    libvlc_media_list_t* newMediaList();
    libvlc_media_player_t* newMediaPlayer();
    libvlc_media_list_player_t* newMediaListPlayer();

    static void release(libvlc_media_t* media);
    static void release(libvlc_media_list_t* mediaList);
    static void release(libvlc_media_player_t* mediaPlayer);
    static void release(libvlc_media_list_player_t* mediaListPlayer);

protected:
    libvlc_instance_t* libvlcInstance;
private:
    char* phoneOutput;
    char* speakerOutput;

    static char* phoneDevice;
    static char* speakerDevice;
    static VLC* instance;
};

#endif // VLC_H
