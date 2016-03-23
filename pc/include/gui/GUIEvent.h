#ifndef GUIEVENT_H
#define GUIEVENT_H

#include <boost/signals2/signal.hpp>

#include "Hitje.h"

using namespace boost;

enum PlaybackState {
    PLAY,
    PAUSE,
    STOP,
    REVERSE
};

class GUIEvent {
    friend class GUI;
    friend class GUIWt;
public:
    typedef boost::function<void (const int num)> InputPhoneNumCallback;
    typedef boost::function<void (const int alt)> InputPhoneAltCallback;
    typedef boost::function<void (const int volume)> SpeakerVolumeCallback;
    typedef boost::function<void (const int volume)> PhoneVolumeCallback;
    typedef boost::function<void (const PlaybackState state)> PlaybackCallback;
    typedef boost::function<void (const Hitje &hitje)> HitjeChangeCallback;

    GUIEvent();
    virtual ~GUIEvent();

    void phoneNum(const InputPhoneNumCallback &callback);
    void phoneAlt(const InputPhoneAltCallback &callback);
    void speakerVolume(const SpeakerVolumeCallback &callback);
    void phoneVolume(const PhoneVolumeCallback &callback);
    void playback(const PlaybackCallback &callback);
    void hitjeChange(const HitjeChangeCallback &callback);
protected:
    boost::signals2::signal<void (const int num)> inputPhoneNumSig;
    boost::signals2::signal<void (const int alt)> inputPhoneAltSig;
    boost::signals2::signal<void (const int volume)> speakerVolumeSig;
    boost::signals2::signal<void (const int volume)> phoneVolumeSig;
    boost::signals2::signal<void (const PlaybackState &state)> playbackSig;
    boost::signals2::signal<void (const Hitje &hitje)> hitjeChangeSig;
private:
};

#endif // GUIEVENT_H
