#ifndef AUDIOPLAYEREVENTLISTENER_H
#define AUDIOPLAYEREVENTLISTENER_H

#include "AudioPlayer.h"
#include "Tools.h"

class AudioPlayerEventListener {
public:
    enum Event {
        DONE
    };

    virtual void audioPlayerEvent(Event evt, AudioPlayer* audioPlayer) = 0;
};

#endif
