#include "InputProcessor.h"

InputProcessor::InputProcessor(AudioList* hitjesList) : hitjesList(hitjesList) {
    processType = PROCESS_LINEAR_SHUFFLE;
    phoneAudioPlayer = new AudioPlayer(AudioPlayer::PHONE, hitjesList);
    speakerAudioPlayer = new AudioPlayer(AudioPlayer::SPEAKER, hitjesList);
    phoneOutput = false;
    earthDown = false;
    curAudioMenu = NULL;

    // listen to end event on phoneAudioPlayer, for queued processing
    //speakerAudioPlayer->attachEventListener(this);

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
}

void InputProcessor::requestInput() {
    if (!curAudioMenu) {
        printf("\nWelk hitje moet gedraaid worden? ");
        if (numberCount) {
            printf("%0*d", numberCount, curNumber);
        }
    } else {
        printf("\nWelk menuitem wil je kiezen? ");
    }
}

void InputProcessor::process(int input) {
    if (curAudioMenu) {
        processAudioMenu(input);
    } else {
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
            }else if (processType == PROCESS_LINEAR_SHUFFLE) {
                // shuffle a random number in the player

            }
        }
    }
}

void InputProcessor::processAudioMenu(int input) {
    // leave input to audio menu if it is open
    if (curAudioMenu->process(input)) {
        libvlc_media_t* toPlay = curAudioMenu->getMedia();
        if (toPlay) {
            if (phoneOutput) {
                phoneAudioPlayer->playAudio(toPlay);
            } else {
                speakerAudioPlayer->playAudio(toPlay);
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
    printf("%c", input + '0');
    curNumber = curNumber * 10 + input;
    if (++numberCount == 3) {
        // play audio per three digit number
        switch (curNumber) {
            case 999:
                // Play the config menu
                SAFE_DELETE(curAudioMenu);
                curAudioMenu = new ConfigAudioMenu();
                break;
            default:
                playAudio(curNumber);
        }
        resetInput();
        requestInput();
    }
}

void InputProcessor::processAlt(int input) {
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
    }
}

void InputProcessor::playAudio(int curNumber) {
    if (!hitjesList->getAudio(curNumber)) {
        // don't process non-existing hitjes
        printf("\nHitje %d does not exist", curNumber);
        return;
    }
    switch (processType) {
        case PROCESS_LINEAR_SHUFFLE:
        case PROCESS_LINEAR:
            printf("\nProcessing linear");
            if (phoneOutput) {
                // play song at phoneAudioPlayer
                printf("\nPlaying hitje %d at phone", curNumber);
                phoneAudioPlayer->playAudio(curNumber);
            } else {
                // list song for playing at speakerAudioPlayer
                if (!speakerAudioPlayer->isPlaying()) {
                    printf("\nPlaying hitje %d at speakers", curNumber);
                    speakerAudioPlayer->playAudio(curNumber);
                } else {
                    printf("\nQueued hitje %d", curNumber);
                    hitjesQueue.push_back(curNumber);
                }
            }
            break;
        case PROCESS_DIRECT:
            if (phoneOutput) {
                // play song at phoneAudioPlayer for listening first
                printf("\nPlaying hitje at phone %d", curNumber);
                phoneAudioPlayer->playAudio(curNumber);
            } else {
                // play song directly at speakerAudioPlayer
                printf("\nPlaying hitje at speaker %d", curNumber);
                speakerAudioPlayer->playAudio(curNumber);
            }
            break;
        case PROCESS_SWAP:
            // always play at phone output first
            printf("\nPlaying hitje at phone %d", curNumber);
            phoneAudioPlayer->playAudio(curNumber);
            break;
    }
}

void InputProcessor::audioPlayerEvent(Event evt, AudioPlayer * audioPlayer) {
    printf("Event %d == %d", evt, DONE);
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

void InputProcessor::playQueued() {
    int audioIndex = hitjesQueue.front();
    hitjesQueue.pop_front();
    if (speakerAudioPlayer->playAudio(audioIndex)) {
        printf("\nPlaying queued hitje %d", audioIndex);
        // wait for the player to actually start
        while (!speakerAudioPlayer->isPlaying());
    }
}

void InputProcessor::setEarthDown(bool down) {
    earthDown = down;
    if (earthDown) {
        if (numberCount) {
            for (int i = 0; i < numberCount; i++) {
                printf("%c %c", 8, 8);    // clear output
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
        printf("\nPut horn down\n");
    } else {
        printf("\nGot horn up\n");
    }
    setOutput(!down);
}

void InputProcessor::setOutput(bool phone) {
    switch(processType) {
        case PROCESS_SWAP:
            // swap the output directly between the two at press
            printf("\nSwapped phone and speaker output");
            phoneAudioPlayer->swapWith(speakerAudioPlayer);
            break;
        default:
            if (phone) {
            } else {
                if (phoneOutput) {
                    int audioIndex;
                    switch (processType) {
                        case PROCESS_LINEAR:
                            // get song playing at phoneAudioPlayer and list it for speakerAudioPlayer
                            audioIndex = phoneAudioPlayer->getAudioIndex();
                            if (audioIndex) {
                                printf("\nQueued hitje %d for speaker that was playing on phone", audioIndex);
                                hitjesQueue.push_back(audioIndex);
                                phoneAudioPlayer->stop();
                            }
                            break;
                        case PROCESS_DIRECT:
                            // get song playing at phoneAudioPlayer and play it directly at speakerAudioPlayer
                            audioIndex = phoneAudioPlayer->getAudioIndex();
                            if (audioIndex) {
                                printf("\nPlaying hitje %d at speaker that was playing on phone", audioIndex);
                                speakerAudioPlayer->playAudio(audioIndex);
                                phoneAudioPlayer->stop();
                            }
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

