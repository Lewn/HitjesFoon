#ifndef INPUTPROCESSOR_H
#define INPUTPROCESSOR_H

enum InputVal {
    INPUT_EARTH_UP = 11,
    INPUT_EARTH_DOWN,
    INPUT_EARTH_SWAP,
    INPUT_HORN_UP,
    INPUT_HORN_DOWN,
    INPUT_HORN_SWAP,
    INPUT_END,
    INPUT_UPDATE,
    INPUT_TEST,
    INPUT_NONE
};

enum ProcessType {
    PROCESS_LINEAR,
    PROCESS_DIRECT,
    PROCESS_SWAP,
    PROCESS_LINEAR_SHUFFLE
};

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread>
#endif // __WIN32

#include "AudioPlayer.h"
#include "AudioPlayerEventListener.h"
#include "ConfigAudioMenu.h"
#include "Tools.h"
#include <list>

class InputProcessor : public AudioPlayerEventListener {
public:
    InputProcessor(AudioList* hitjesList, const char *configMenuPath);
    virtual ~InputProcessor();

    AudioList *getHitjesList();
    void resetInput();
    void process(int input);
    void requestInput();
    void audioPlayerEvent(Event evt, AudioPlayer* audioPlayer);
protected:
    AudioList *hitjesList;
    ProcessType processType;
    int curNumber;
    int numberCount;
    bool phoneOutput;
    bool earthDown;
    AudioMenu *curAudioMenu;

    AudioPlayer *phoneAudioPlayer;
    AudioPlayer *speakerAudioPlayer;

    list<int> hitjesQueue;

    void processAudioMenu(int input);
    void processNum(int input);
    void processAlt(int input);

    void playAudio(int curNumber);
    void playQueued();

    void setEarthDown(bool down);
    void setHornDown(bool down);
    void setOutput(bool phone);
    void toggleOutput();

    bool threadRunning();
    void threadUpdate();

private:
    char* configMenuPath;
};

#endif // INPUTPROCESSOR_H
