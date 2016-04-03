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
#include <functional>

#include "Tools.h"
#include "Hitje.h"
#include "Config.h"
#include "gui/GUI.h"
#include "gui/GUIEvent.h"
#include "AudioList.h"
#include "AudioPlayer.h"
#include "ConfigAudioMenu.h"
#include "retrieve/Retriever.h"

using namespace std;

class InputProcessor {
public:
    InputProcessor(GUI &gui, Config &config);
    virtual ~InputProcessor();

    void registerInputRaw(std::function<int ()> input);
    void registerInput(std::function<int ()> input);
    int convertInput(int c);

    void terminate();
    bool hasTerminated();

    AudioList *getHitjesList();
    void resetInput();
    void requestInput();
    void process(string input);
    void process(int input);
    void inputNum(int input);

    void playQueued();

    void playbackChangeEvent(const PlaybackState state);

protected:
    GUI &gui;
    AudioList hitjesList;
    Retriever retriever;
    ProcessType processType;
    bool terminated = false;
    vector<thread> threads;

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
    void sendHitjesQueue(int current);

    void setEarthDown(bool down);
    void setHornDown(bool down);
    void setOutput(bool phone);
    void toggleOutput();

    void doUpdate();

private:
    string configMenuPath;
};

#endif // INPUTPROCESSOR_H
