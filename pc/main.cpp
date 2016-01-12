

#include "USBConnection.h"
#include "Tools.h"
#include "YoutubeAPI.h"


using namespace std;


extern PRINT_LEVEL msglevel;
PRINT_LEVEL msglevel = PRINT_LEVEL::LINFO;

int main() {
#ifndef _WIN32
    setConioTerminalMode();
#endif

    try {
        char *readBuf = new char[1024];
        char *trimBuf;
        char *hitjesListFile = new char[1024];
        char *hitjesPath = new char[1024];
        char *configMenuPath = new char[1024];
        FILE *configFile = fopen("config.txt", "r");

        if(!configFile) {
            SAFE_DELETE_ARRAY(readBuf);
            SAFE_DELETE_ARRAY(hitjesListFile);
            throw "Couldn't open config file";
        }

        // first read the hitjeslist config
        fgets(hitjesListFile, 1024, configFile);

        // read the path to use when saving music
        fgets(hitjesPath, 1024, configFile);

        // read the path where the configMenu is saved
        fgets(configMenuPath, 1024, configFile);

        // read Phone audio device from input
        if(fgets(readBuf, 1024, configFile) != NULL) {
            trimBuf = trim(readBuf);

            if(strlen(trimBuf)) {
                VLC::setPhoneDevice(trimBuf);
            }
        }

        // and the speaker device
        if(fgets(readBuf, 1024, configFile) != NULL) {
            trimBuf = trim(readBuf);

            if(strlen(trimBuf)) {
                VLC::setSpeakerDevice(trimBuf);
            }
        }

        fclose(configFile);
        SAFE_DELETE_ARRAY(readBuf);

        // instantiate usb and vlc, cause we will need them
        USBConnection connection;
        VLC::getInstance();

        // create the list with all hitjes from file and put it in the processor
        AudioList *hitjesList = new AudioList(trim(hitjesListFile), trim(hitjesPath));
        SAFE_DELETE_ARRAY(hitjesListFile);
        SAFE_DELETE_ARRAY(hitjesPath);

        // create an input processor for processing keyboard and usb input
        InputProcessor processor(hitjesList, trim(configMenuPath));
        SAFE_DELETE_ARRAY(configMenuPath);

        while(true) {
            // read user input for selecting hitje, also read from usb
            int c = readKeyboard();

            if (c == INPUT_NONE) {
                // nothing read from keyboard, check usb
                c = connection.read();
            }

            if (c == INPUT_TEST) {
                // test with t
            } else if (c == INPUT_END) {
                // end at q
                break;
            }

            processor.process(c);
        }

        SAFE_DELETE(hitjesList);

    } catch(const char *ex) {
        printlevel(LERROR, "%s\n", ex);
    }

#ifndef _WIN32
    resetTerminalMode();
#endif

    printlevel(LINFO, "\n\n");
    VLC::deleteInstance();
}

