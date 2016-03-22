#include "InputProcessor.h"


InputProcessor::InputProcessor(GUI &gui, Config &config) : gui(gui), hitjesList(gui, config) {
    processType = PROCESS_LINEAR_SHUFFLE;
    phoneAudioPlayer = new AudioPlayer(gui, AudioPlayer::PHONE, hitjesList);
    speakerAudioPlayer = new AudioPlayer(gui, AudioPlayer::SPEAKER, hitjesList);
    phoneOutput = false;
    earthDown = false;
    curAudioMenu = NULL;

    configMenuPath = config.getConfigMenuPath();

    // listen to end event on phoneAudioPlayer, for queued processing
    //speakerAudioPlayer->attachEventListener(this);


    gui.setSpeakerVolume(speakerAudioPlayer->getVolume());

    // listen to gui events such that it can change audio values
    gui.events().phoneNum(boost::bind(&InputProcessor::inputNum, this, _1));
    gui.events().phoneAlt(boost::bind(&InputProcessor::processAlt, this, _1));
    gui.events().speakerVolume(boost::bind(&AudioPlayer::setVolume, speakerAudioPlayer, _1));
    gui.events().phoneVolume(boost::bind(&AudioPlayer::setVolume, phoneAudioPlayer, _1));
    gui.events().playback(boost::bind(&InputProcessor::playbackChangeEvent, this, _1));

    resetInput();
    requestInput();
}

InputProcessor::~InputProcessor() {
    SAFE_DELETE(phoneAudioPlayer);
    SAFE_DELETE(speakerAudioPlayer);
    SAFE_DELETE(curAudioMenu);
}

void InputProcessor::resetInput() {
    curNumber = numberCount = 0;
    gui.setPhoneNum(curNumber);
}

void InputProcessor::requestInput() {
    if (!curAudioMenu) {
        gui.printlevel(LINFO, "\nWelk hitje moet gedraaid worden? ");
        if (numberCount) {
            gui.printlevel(LINFO, "%0*d", numberCount, curNumber);
        }
    } else {
        gui.printlevel(LINFO, "\nWelk menuitem wil je kiezen? ");
    }
}

void InputProcessor::process(string input) {
    int c;
    if (input.length() == 1) {
        c = input[0];
        if (c >= '0' && c <= '9') {
            // got a number, return it
            c = c - '0';
            inputNum(c);
            return;
        } else if (c == 8 || c == 'd') {
            // backspace, simulate earth button (reset number)
            c = INPUT_EARTH_DOWN;
        } else if (c == 's') {
            // s, simulate horn (swap output)
            c = INPUT_HORN_SWAP;
        } else if (c == 'q') {
            // q, return quit
            c = INPUT_END;
        } else if (c == 'u') {
            c = INPUT_UPDATE;
        } else if (c == 't') {
            // t, testing purposes
            c = INPUT_TEST;
        } else {
            // Invalid input
            return;
        }
        processAlt(c);
    } else {
        c = stoi(input);
        inputNum(c);
    }
}

void InputProcessor::process(int input) {
    if (input >= 0 && input <= 9) {
        processNum(input);
    } else {
        processAlt(input);
    }
    // TODO: in een event knallen zodat deze niet permanent aangeroepen wordt?
    if (!speakerAudioPlayer->isPlaying()) {
        // always check on update wether we want to do a queued action
        if (!hitjesQueue.empty()) {
            // done playing, play next in queue
            playQueued();
            requestInput();
        } else if (processType == PROCESS_LINEAR_SHUFFLE) {
            // shuffle a random number in the player

        }
    }
}

void InputProcessor::processAudioMenu(int input) {
    // leave input to audio menu if it is open
    if (curAudioMenu->process(input)) {
        libvlc_media_t *toPlay = curAudioMenu->getMedia();
        if (toPlay) {
            if (phoneOutput) {
                phoneAudioPlayer->playAudio(toPlay);
            } else {
                speakerAudioPlayer->playAudio(toPlay);
                while (!speakerAudioPlayer->isPlaying());
                sendHitjesQueue(0);
            }
        }
        if (curAudioMenu->isEnded()) {
            SAFE_DELETE(curAudioMenu);
            curAudioMenu = NULL;
            resetInput();
        }
        requestInput();
    }
}

void InputProcessor::processNum(int input) {
    // got a number input, save it
    gui.printlevel(LINFO, "%c", input + '0');
    curNumber = curNumber  * 10 + input;
    if (++numberCount == 3) {
        // play audio per three digit number
        inputNum(curNumber);
        resetInput();
        requestInput();
    } else {
        gui.setPhoneNum(curNumber);
    }
}

void InputProcessor::processAlt(int input) {
    if (curAudioMenu) {
        processAudioMenu(input);
        return;
    }
    switch (input) {
        case INPUT_EARTH_UP:
            setEarthDown(false);
            break;
        case INPUT_EARTH_DOWN:
            setEarthDown(true);
            break;
        case INPUT_EARTH_SWAP:
            setEarthDown(!earthDown);
            break;
        case INPUT_HORN_UP:
            setHornDown(false);
            break;
        case INPUT_HORN_DOWN:
            setHornDown(true);
            break;
        case INPUT_HORN_SWAP:
            setHornDown(phoneOutput);
            break;
        case INPUT_END:
            resetInput();
            break;
        case INPUT_UPDATE:
            if (!threadRunning()) {
                threadUpdate();
            } else {
                gui.printlevel(LWARNING, "Update thread already running\n");
            }
            break;
        default:
            break;
    }
}

void InputProcessor::inputNum(int num) {
    if (curAudioMenu) {
        processAudioMenu(num);
    } else {
        switch (num) {
            case 999:
                // Play the config menu
                SAFE_DELETE(curAudioMenu);
                curAudioMenu = new ConfigAudioMenu(gui, configMenuPath);
                break;
            default:
                playAudio(num);
        }
    }
}

void InputProcessor::playAudio(int curNumber) {
    if (!hitjesList.getHitje(curNumber)) {
        // don't process non-existing hitjes
        gui.printlevel(LINFO, "\n");
        gui.printlevel(LWARNING, "Hitje %d does not exist\n", curNumber);
        return;
    }
    switch (processType) {
        case PROCESS_LINEAR_SHUFFLE:
        case PROCESS_LINEAR:
            gui.printlevel(LINFO, "\nProcessing linear");
            if (phoneOutput) {
                // play song at phoneAudioPlayer
                gui.printlevel(LINFO, "\nPlaying hitje %d at phone", curNumber);
                phoneAudioPlayer->playAudio(curNumber);
            } else {
                // list song for playing at speakerAudioPlayer
                if (!speakerAudioPlayer->isPlaying()) {
                    gui.printlevel(LINFO, "\nPlaying hitje %d at speakers", curNumber);
                    if (!speakerAudioPlayer->playAudio(curNumber)) {
                        throw "Could not play audio";
                    }
                    sendHitjesQueue(curNumber);
                } else {
                    gui.printlevel(LINFO, "\nQueued hitje %d", curNumber);
                    hitjesQueue.push_back(curNumber);
                    sendHitjesQueue(0);
                }
            }
            break;
        case PROCESS_DIRECT:
            if (phoneOutput) {
                // play song at phoneAudioPlayer for listening first
                gui.printlevel(LINFO, "\nPlaying hitje at phone %d", curNumber);
                phoneAudioPlayer->playAudio(curNumber);
            } else {
                // play song directly at speakerAudioPlayer
                gui.printlevel(LINFO, "\nPlaying hitje at speaker %d", curNumber);
                speakerAudioPlayer->playAudio(curNumber);
                sendHitjesQueue(curNumber);
            }
            break;
        case PROCESS_SWAP:
            // always play at phone output first
            gui.printlevel(LINFO, "\nPlaying hitje at phone %d", curNumber);
            phoneAudioPlayer->playAudio(curNumber);
            break;
    }
}

void InputProcessor::audioPlayerEvent(Event evt, AudioPlayer  *audioPlayer) {
    gui.printlevel(LDEBUG, "Event %d == %d", evt, DONE);
    switch (evt) {
        case DONE:
            // listen to done event on speakerAudioPlayer
            if (!hitjesQueue.empty()) {
                playQueued();
            }
            break;
    }
    requestInput();
}

void InputProcessor::playbackChangeEvent(const PlaybackState state) {
    switch (state) {
        case PLAY:
            speakerAudioPlayer->resume();
            gui.printlevel(LINFO, "Set playback to resume\n");
            break;
        case PAUSE:
            speakerAudioPlayer->pause();
            gui.printlevel(LINFO, "Set playback to pause\n");
            break;
        case STOP:
            speakerAudioPlayer->stop();
            sendHitjesQueue(0);
            gui.printlevel(LINFO, "Set playback to stop\n");
            break;
        case REVERSE:
            break;
    }
}

void InputProcessor::playQueued() {
    int audioIndex = hitjesQueue.front();
    hitjesQueue.pop_front();
    if (speakerAudioPlayer->playAudio(audioIndex)) {
        gui.printlevel(LINFO, "\nPlaying queued hitje %d", audioIndex);
        // wait for the player to actually start
        while (!speakerAudioPlayer->isPlaying());
        sendHitjesQueue(audioIndex);
    }
}

void InputProcessor::sendHitjesQueue(int current) {
    vector<int> queueCpy;
    if (current == 0 && speakerAudioPlayer->isPlaying()) {
        current = speakerAudioPlayer->getAudioIndex();
    }
    if (current != 0) {
        queueCpy.push_back(current);
    }
    copy(hitjesQueue.begin(), hitjesQueue.end(), back_inserter(queueCpy));
    gui.setPlaylist(queueCpy);
}

void InputProcessor::setEarthDown(bool down) {
    earthDown = down;
    if (earthDown) {
        if (numberCount) {
            for (int i = 0; i < numberCount; i++) {
                gui.printlevel(LINFO, "%c %c", 8, 8);    // clear output
            }
            resetInput();
        } else {
            if (phoneOutput) {
                phoneAudioPlayer->stop();
            } else {
                speakerAudioPlayer->stop();
                sendHitjesQueue(0);
            }
        }
    }
}

void InputProcessor::setHornDown(bool down) {
    if (down) {
        gui.printlevel(LINFO, "\nPut horn down\n");
    } else {
        gui.printlevel(LINFO, "\nGot horn up\n");
    }
    setOutput(!down);
}

void InputProcessor::setOutput(bool phone) {
    switch (processType) {
            int audioIndex;
        case PROCESS_SWAP:
            // swap the output directly between the two at press
            gui.printlevel(LINFO, "\nSwapped phone and speaker output");
            audioIndex = phoneAudioPlayer->getAudioIndex();
            phoneAudioPlayer->swapWith(speakerAudioPlayer);
            sendHitjesQueue(audioIndex);
            break;
        default:
            if (phone) {
            } else {
                if (phoneOutput) {
                    switch (processType) {
                        case PROCESS_LINEAR_SHUFFLE:
                        case PROCESS_LINEAR:
                            // get song playing at phoneAudioPlayer and list it for speakerAudioPlayer
                            audioIndex = phoneAudioPlayer->getAudioIndex();
                            if (audioIndex) {
                                gui.printlevel(LINFO, "\nQueued hitje %d for speaker that was playing on phone", audioIndex);
                                hitjesQueue.push_back(audioIndex);
                                phoneAudioPlayer->stop();
                                sendHitjesQueue(0);
                            }
                            break;
                        case PROCESS_DIRECT:
                            // get song playing at phoneAudioPlayer and play it directly at speakerAudioPlayer
                            audioIndex = phoneAudioPlayer->getAudioIndex();
                            if (audioIndex) {
                                gui.printlevel(LINFO, "\nPlaying hitje %d at speaker that was playing on phone", audioIndex);
                                speakerAudioPlayer->playAudio(audioIndex);
                                phoneAudioPlayer->stop();
                                sendHitjesQueue(audioIndex);
                            }
                            break;
                        case PROCESS_SWAP:
                            break;
                    }
                }
            }
    }
    if (phone != phoneOutput) {
        requestInput();
        phoneOutput = phone;
    }
}

void InputProcessor::toggleOutput() {
    setOutput(!phoneOutput);
}


thread *updateThread = NULL;
void InputProcessor::doUpdate() {
    try {
        do {
            gui.printlevel(LINFO, "\nDownloading one more hitje");
            requestInput();
        } while (hitjesList.update(1) && false);
        requestInput();
    } catch (const char *ex) {
        gui.printlevel(LERROR, "%s\n", ex);
    }
    updateThread = NULL;
}

bool InputProcessor::threadRunning() {
    return updateThread != NULL;
}

void InputProcessor::threadUpdate() {
    doUpdate();
    //updateThread = new thread(doUpdate, this);
}
