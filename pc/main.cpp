

#include "AudioPlayer.h"
#include "USBConnection.h"
#include "Tools.h"

//#ifdef _WIN32
#include "conio.h"
//#endif // _WIN32

using namespace std;

int consoleDataIndex = 0;
char* consoleDataBuffer = new char[256];
int consoleDataAvailable = 0;


//#ifdef _WIN32
int readKeyboard() {
    if (kbhit()) {
        char c;
        if ((c = getch()) != 13) {
            // read keyboard
            printf("%c", c);
            consoleDataBuffer[consoleDataIndex++] = c;
        } else {
            consoleDataBuffer[consoleDataIndex] = 0;
            consoleDataIndex = 0;
            return 1;
        }
    }
    return 0;
}
//#endif // _WIN32

AudioList* hitjesList;

void playSong(int songIndex, AudioPlayer* player) {
    player->playAudio(songIndex);
}

int main() {
    try {
        char* readBuf = new char[1024];
        char* hitjesListFile = new char[1024];
        FILE* configFile = fopen("config.txt", "r");
        if (!configFile) {
            throw "Couldn't open config file";
        }
        // first read the hitjeslist config
        fgets(hitjesListFile, 1024, configFile);

        // read Phone audio device from input
        if (fgets(readBuf, 1024, configFile) != NULL && strlen(readBuf)) {
            VLC::setPhoneDevice(trim(readBuf));
        }
        // and the speaker device
        if (fgets(readBuf, 1024, configFile) != NULL && strlen(readBuf)) {
            VLC::setSpeakerDevice(trim(readBuf));
        }

        fclose(configFile);
        delete[] readBuf;

        // instantiate usb and vlc, cause we will need them
        USBConnection connection;
        VLC::getInstance();


        // create the list with all hitjes from file
        hitjesList = new AudioList(trim(hitjesListFile));
        delete[] hitjesListFile;

        // create two players, so we can seamlessly swap between them
        AudioPlayer phoneAudioPlayer(AudioPlayer::PHONE, hitjesList);
        AudioPlayer speakerAudioPlayer(AudioPlayer::SPEAKER, hitjesList);

        AudioPlayer* curPlayer = &phoneAudioPlayer;
        int songIndex = 0;
        int swapPlayer = 0;
        printf("Welk hitje moet gedraaid worden? ");
        while (true) {
            // read user input for selecting hitje, also read from usb
            if (readKeyboard()) {
                // read a string (until enter) from console input, parse it
                if (sscanf(consoleDataBuffer, "%d", &songIndex) != 1 || songIndex <= 0 || songIndex > 1000) {
                    printf("Invalid input, %s", consoleDataBuffer);
                    songIndex = 0;
                }
                if (songIndex == 1000) {
                    swapPlayer = 1;
                    songIndex = 0;
                }
            } else if (connection.read()) {
                // read a string from usb input, parse it
                songIndex = connection.getDialNumber();
                if (connection.isHornDown()) {
                    if (curPlayer == &phoneAudioPlayer) {
                        swapPlayer = 1;
                    }
                } else {
                    if (curPlayer == &speakerAudioPlayer) {
                        swapPlayer = 1;
                    }
                }
            }

            if (swapPlayer) {
                if (curPlayer == &phoneAudioPlayer) {
                    curPlayer = curPlayer->swapWith(&speakerAudioPlayer);
                } else {
                    curPlayer = curPlayer->swapWith(&phoneAudioPlayer);
                }
                swapPlayer = 0;
                if (!songIndex) {
                    printf("\nWelk hitje moet gedraaid worden? ");
                }
            }
            if (songIndex) {
                playSong(songIndex, curPlayer);
                printf("\nWelk hitje moet gedraaid worden? ");
                songIndex = 0;
            }
        }

        VLC::deleteInstance();
    } catch (const char* ex) {
        printf("\n\nERROR: %s\n", ex);
    }
    delete hitjesList;
    delete[] consoleDataBuffer;
}
