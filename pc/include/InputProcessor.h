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


#include <thread>
#include <list>
#include <algorithm>

#include "gui/GUI.h"
#include "AudioList.h"
#include "AudioPlayer.h"
#include "AudioPlayerEventListener.h"
#include "ConfigAudioMenu.h"
#include "Tools.h"
#include "Config.h"

using namespace std;

class InputProcessor : public AudioPlayerEventListener {
public:
    static InputProcessor *getInstance();
    static InputProcessor *getInstance(GUI *gui, AudioList *hitjesList, Config *config);
    static void deleteInstance();

    AudioList *getHitjesList();
    void resetInput();
    void requestInput();
    void process(string input);
    void process(int input);
    void inputNum(int input);
    void audioPlayerEvent(Event evt, AudioPlayer *audioPlayer);

    AudioPlayer *getPhoneAudioPlayer();
    AudioPlayer *getSpeakerAudioPlayer();
protected:
    static InputProcessor *instance;

    GUI *gui;
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

    InputProcessor(GUI *gui, AudioList *hitjesList, Config *config);
    virtual ~InputProcessor();

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

    InputProcessor(const InputProcessor &that) = delete;
};

#endif // INPUTPROCESSOR_H
