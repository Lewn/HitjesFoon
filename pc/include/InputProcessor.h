#ifndef INPUTPROCESSOR_H
#define INPUTPROCESSOR_H

enum ProcessType {
    PROCESS_LINEAR,
    PROCESS_DIRECT,
    PROCESS_SWAP,
    PROCESS_LINEAR_SHUFFLE
};


#include <thread>
#include <chrono>
#include <list>
#include <algorithm>

#include "Tools.h"
#include "Hitje.h"
#include "Config.h"
#include "gui/GUI.h"
#include "gui/GUIEvent.h"
#include "AudioList.h"
#include "AudioPlayer.h"
#include "AudioPlayerEventListener.h"
#include "ConfigAudioMenu.h"

using namespace std;

class InputProcessor : public AudioPlayerEventListener {
public:
    InputProcessor(GUI &gui, Config &config);
    virtual ~InputProcessor();

    AudioList *getHitjesList();
    void resetInput();
    void requestInput();
    void process(string input);
    void process(int input);
    void inputNum(int input);

    void audioPlayerEvent(Event evt, AudioPlayer *audioPlayer);
    void playbackChangeEvent(const PlaybackState state);

protected:
    GUI &gui;
    AudioList hitjesList;
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
    void sendHitjesQueue(int current);

    void setEarthDown(bool down);
    void setHornDown(bool down);
    void setOutput(bool phone);
    void toggleOutput();

    void doUpdate();
    bool threadRunning();
    void threadUpdate();

private:
    string configMenuPath;
};

#endif // INPUTPROCESSOR_H
