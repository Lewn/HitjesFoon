#ifndef INPUTPROCESSOR_H
#define INPUTPROCESSOR_H

enum ProcessType {
    PROCESS_LINEAR,             // Keep a queue of hitjes, add new hitjes to the back, play new hitjes from the front
    PROCESS_DIRECT,             // When a new hitje is added, directly play it at speaker
    PROCESS_SWAP,               // When a new hitje is added, swap between phone and speaker (useless?)
    PROCESS_LINEAR_RANDOM,      // Keep a queue of hitjes in a linear way, when empty, add a random hitje
    PROCESS_SHUFFLE_RANDOM,     // Keep a queue of hitjes, insert new hitjes at a random spot, when queue empty add a random hitje
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

    void terminate(int status = 0);
    bool hasTerminated();
    int getExitStatus();

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
    int exitStatus = 0;
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
