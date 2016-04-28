#include "InputProcessor.h"


InputProcessor::InputProcessor(GUI &gui, Config &config) : gui(gui), hitjesList(gui, config), retriever(gui) {
    processType = PROCESS_LINEAR_RANDOM;
    phoneAudioPlayer = new AudioPlayer(gui, AudioPlayer::PHONE, hitjesList);
    speakerAudioPlayer = new AudioPlayer(gui, AudioPlayer::SPEAKER, hitjesList);
    phoneOutput = false;
    earthDown = false;
    curAudioMenu = NULL;

    configMenuPath = config.getConfigMenuPath();

    // Send initial values
    gui.setSpeakerVolume(speakerAudioPlayer->getVolume());
    sendHitjesQueue(0);

    // Listen to gui events such that it can change audio values
    gui.events().phoneNum(boost::bind(&InputProcessor::inputNum, this, _1));
    gui.events().phoneAlt(boost::bind(&InputProcessor::processAlt, this, _1));
    gui.events().speakerVolume(boost::bind(&AudioPlayer::setVolume, speakerAudioPlayer, _1));
    gui.events().phoneVolume(boost::bind(&AudioPlayer::setVolume, phoneAudioPlayer, _1));
    gui.events().playback(boost::bind(&InputProcessor::playbackChangeEvent, this, _1));

    // Listen to audio player events
    speakerAudioPlayer->onEnd(std::bind(&InputProcessor::playQueued, this));

    resetInput();
    requestInput();
}

InputProcessor::~InputProcessor() {
    terminate();
    // Join threads to make sure they terminate
    gui.printlevel(LDEBUG, "Joining %d threads\n", threads.size());
    for (thread &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    gui.printlevel(LDEBUG, "Child threads finished\n");

    SAFE_DELETE(phoneAudioPlayer);
    SAFE_DELETE(speakerAudioPlayer);
    SAFE_DELETE(curAudioMenu);
}


void InputProcessor::registerInputRaw(std::function<int ()> input) {
// Raw input, convert before processing
    registerInput([ &, input ]() -> int {
        return convertInput(input());
    });
}

void InputProcessor::registerInput(std::function<int ()> input) {
    // Thread is automatically terminated when vector goes out of scope
    // ie when InputProcessor is destroyed, so are all child threads
    threads.emplace_back([ &, input ] {
        // Continuously loop for getting input
        while (!terminated) {
            int c = input();
            process(c);
            // If we are polling, add a delay to not hog the processor
            this_thread::sleep_for(chrono::milliseconds(200));
        }
    });
}

int InputProcessor::convertInput(int c) {
    if (c >= '0' && c <= '9') {
        // got a number, return it
        return c - '0';
    } else if (c == 8 || c == 'd') {
        // backspace, simulate earth button (reset number)
        return INPUT_EARTH_DOWN;
    } else if (c == 's') {
        // s, simulate horn (swap output)
        return INPUT_HORN_SWAP;
    } else if (c == 'q') {
        // q, return quit
        return INPUT_EXIT;
    } else if (c == 'u') {
        // u, update hitjes list
        return INPUT_UPDATE;
    } else if (c == 'r') {
        // r, refresh hitjes list
        return INPUT_REFRESH;
    } else if (c == 't') {
        // t, testing purposes
        return INPUT_TEST;
    }
    return INPUT_NONE;
}


void InputProcessor::terminate(int status) {
    if (!terminated) {
        // Terminate allows for a graceful shutdown from any source
        terminated = true;
        exitStatus = status;
        resetInput();
    }
}

bool InputProcessor::hasTerminated() {
    return terminated;
}

int InputProcessor::getExitStatus() {
    return exitStatus;
}

void InputProcessor::resetInput() {
    curNumber = numberCount = 0;
    gui.setPhoneNum(curNumber);
}

void InputProcessor::requestInput() {
    if (!curAudioMenu) {
        gui.printlevel(LBGINFO, "\nWelk hitje moet gedraaid worden? ");
        if (numberCount) {
            gui.printlevel(LBGINFO, "%0*d", numberCount, curNumber);
        }
    } else {
        gui.printlevel(LBGINFO, "\nWelk menuitem wil je kiezen? ");
    }
}

void InputProcessor::process(string input) {
    if (input.length() == 1) {
        int c = convertInput(input[0]);
        if (c >= 0 && c <= 9) {
            // got a number, input it
            inputNum(c);
            return;
        }
        processAlt(c);
    } else {
        int c = stoi(input);
        inputNum(c);
    }
}

void InputProcessor::process(int input) {
    if (input >= 0 && input <= 9) {
        processNum(input);
    } else {
        processAlt(input);
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
            resetInput();
        }
        requestInput();
    }
}

void InputProcessor::processNum(int input) {
    // got a number input, save it
    gui.printlevel(LBGINFO, "%c", input + '0');
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
        case INPUT_UPDATE:
            // Download and update hitjes
            doUpdate();
            break;
        case INPUT_REFRESH:
            // Perform an update without downloading
            hitjesList.update();
            break;
        case INPUT_EXIT:
            // Exit the program with normal exit status
            terminate();
            break;
        case INPUT_RESTART:
            // Exit the program with exit status 1, forces restart
            terminate(1);
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
            case 0:
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
        gui.printlevel(LBGINFO, "\n");
        gui.printlevel(LWARNING, "Hitje %d does not exist\n", curNumber);
        return;
    }
    switch (processType) {
        case PROCESS_LINEAR_RANDOM:
        case PROCESS_LINEAR:
            gui.printlevel(LBGINFO, "\nProcessing linear");
            if (phoneOutput) {
                // play song at phoneAudioPlayer
                gui.printlevel(LINFO, "\nPlaying hitje %d at phone", curNumber);
                phoneAudioPlayer->playAudio(curNumber);
            } else {
                // list song for playing at speakerAudioPlayer
                // Audio index is also set when paused (isPlaying() returns false in that case)
                if (!speakerAudioPlayer->isBusy()) {
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

void InputProcessor::playbackChangeEvent(const PlaybackState state) {
    switch (state) {
        case PLAY:
            gui.printlevel(LBGINFO, "Set playback to resume\n");
            speakerAudioPlayer->resume();
            break;
        case PAUSE:
            gui.printlevel(LBGINFO, "Set playback to pause\n");
            speakerAudioPlayer->pause();
            break;
        case STOP:
            gui.printlevel(LBGINFO, "Set playback to stop\n");
            speakerAudioPlayer->stop();
            break;
        case REVERSE:
            break;
    }
}

void InputProcessor::playQueued() {
    // TODO make smarter (correctly implement playback modes)
    if (hitjesQueue.empty() && (processType == PROCESS_LINEAR_RANDOM || processType == PROCESS_SHUFFLE_RANDOM)) {
        // Add in a new random hitje
        hitjesQueue.push_back(hitjesList.getRandom().hitIndex);
    }
    // TODO implement playback modes in different class
    // TODO thread safety of playback
    gui.printlevel(LDEBUG, "Got playback end event, playing queued\n");
    if (!hitjesQueue.empty()) {
        int audioIndex = hitjesQueue.front();
        hitjesQueue.pop_front();
        gui.printlevel(LDEBUG, "Going to play %d\n", audioIndex);
        if (speakerAudioPlayer->playAudio(audioIndex)) {
            gui.printlevel(LINFO, "Playing queued hitje %d\n", audioIndex);
            // wait for the player to actually start
            while (!speakerAudioPlayer->isBusy());
            sendHitjesQueue(audioIndex);
            return;
        }
    }
    gui.printlevel(LDEBUG, "None to play\n");
    sendHitjesQueue(0);
}

void InputProcessor::sendHitjesQueue(int current) {
    vector<int> queueCpy;
    if (current == 0 && speakerAudioPlayer->isBusy()) {
        current = speakerAudioPlayer->getAudioIndex();
    }
    if (current != 0) {
        // TODO shouldn't need current index?
        gui.printlevel(LDEBUG, "Using current\n");
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
                gui.printlevel(LBGINFO, "%c %c", 8, 8);    // clear output
            }
            resetInput();
        } else {
            if (phoneOutput) {
                phoneAudioPlayer->stop();
            } else {
                speakerAudioPlayer->stop();
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
            gui.printlevel(LBGINFO, "\nSwapped phone and speaker output");
            audioIndex = phoneAudioPlayer->getAudioIndex();
            phoneAudioPlayer->swapWith(speakerAudioPlayer);
            sendHitjesQueue(audioIndex);
            break;
        default:
            if (phone) {
            } else {
                if (phoneOutput) {
                    switch (processType) {
                        case PROCESS_LINEAR_RANDOM:
                        case PROCESS_LINEAR:
                            // get song playing at phoneAudioPlayer and list it for speakerAudioPlayer
                            audioIndex = phoneAudioPlayer->getAudioIndex();
                            if (audioIndex) {
                                gui.printlevel(LBGINFO, "\nQueued hitje %d for speaker that was playing on phone", audioIndex);
                                hitjesQueue.push_back(audioIndex);
                                phoneAudioPlayer->stop();
                            }
                            break;
                        case PROCESS_DIRECT:
                            // get song playing at phoneAudioPlayer and play it directly at speakerAudioPlayer
                            audioIndex = phoneAudioPlayer->getAudioIndex();
                            if (audioIndex) {
                                gui.printlevel(LBGINFO, "\nPlaying hitje %d at speaker that was playing on phone", audioIndex);
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


void InputProcessor::doUpdate() {
    try {
        for (int i = 1; i < 1000; i++) {
            const Hitje &hitje = hitjesList.getHitje(i);
            if (hitje.canDownload()) {
                gui.printlevel(LBGINFO, "\nDownloading one more hitje");
                if (retriever.retrieve(hitje)) {
                    // Stop when a new hitje is downloaded
                    break;
                }
            }
        }
        requestInput();
    } catch (const char *ex) {
        gui.printlevel(LERROR, "%s\n", ex);
    }
}
