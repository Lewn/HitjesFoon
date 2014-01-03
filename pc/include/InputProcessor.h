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
    INPUT_TEST,
    INPUT_NONE
};

enum ProcessType {
    PROCESS_LINEAR,
    PROCESS_DIRECT,
    PROCESS_SWAP
};

#include "AudioPlayer.h"
#include "AudioPlayerEventListener.h"
#include "ConfigAudioMenu.h"
#include "Tools.h"
#include <list>

class InputProcessor : public AudioPlayerEventListener {
public:
    InputProcessor(AudioList* hitjesList);
    virtual ~InputProcessor();

    void resetInput();
    void process(int input);
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

    void requestInput();
    void playAudio(int curNumber);
    void playQueued();

    void setEarthDown(bool down);
    void setHornDown(bool down);
    void setOutput(bool phone);
    void toggleOutput();
private:
};

#endif // INPUTPROCESSOR_H
