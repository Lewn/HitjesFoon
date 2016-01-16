

#include "USBConnection.h"
#include "Tools.h"
#include "YoutubeAPI.h"

#include <regex>

using namespace std;


extern PRINT_LEVEL msglevel;
PRINT_LEVEL msglevel = PRINT_LEVEL::LINFO;

int main() {
#ifndef _WIN32
    setConioTerminalMode();
#endif

    try {
        Config config("config.txt");

        VLC::setConfig(&config);

        // instantiate usb and vlc, cause we will need them
        USBConnection connection;
        VLC::getInstance();

        // create the list with all hitjes from file and put it in the processor
        AudioList *hitjesList = new AudioList(&config);
//
//        // create an input processor for processing keyboard and usb input
        InputProcessor processor(hitjesList, &config);

        while (true) {
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
    } catch (const char *ex) {
        printlevel(LERROR, "%s\n", ex);
    }

#ifndef _WIN32
    resetTerminalMode();
#endif

    printlevel(LINFO, "\n\n");
    VLC::deleteInstance();
 }

