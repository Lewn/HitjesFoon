

//#define GUI_CURSES
#define GUI_WT


#if defined(GUI_CURSES) && defined(GUI_WT)
#error "Can define only a single gui"
#endif // defined

#include "USBConnection.h"
#include "Tools.h"
#include "YoutubeAPI.h"
#include "GUI.h"

#ifdef GUI_CURSES
#include "GUICurses.h"
#elif defined(GUI_WT)
#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WException>
#include "GUINull.h"
#include "GUIWt.h"

using namespace std;

GUIWt *guiref = NULL;
Wt::WApplication *createApplication(const Wt::WEnvironment& env) {
    return guiref->createApplication(env);
}

#else
#error "Must define at least one gui"
#endif

#include <regex>


int main(int argc, char **argv) {
#ifdef GUI_CURSES
    GUICurses gui = GUICurses(LINFO);
#else
    GUIWt gui = GUIWt(LINFO);
    guiref = &gui;
#endif
    gui.printlevel(LINFO, "Starting hitjesfoon application...\n");
    try {
        Config config(&gui, "config.txt");
        gui.printlevel(LDEBUG, "Read config file\n");

        VLC::setConfig(&config);

        // instantiate usb and vlc, cause we will need them
        USBConnection connection(&gui);
        gui.printlevel(LDEBUG, "Instantiated usb connection\n");
        VLC::setGUI(&gui);
        VLC::getInstance();
        gui.printlevel(LDEBUG, "Instantiated vlc\n");

        // create the list with all hitjes from file and put it in the processor
        AudioList hitjesList(&gui, &config);
        gui.printlevel(LDEBUG, "Read hitjeslist into memory\n");

#ifdef GUI_CURSES
        // For a curses gui, we need a continuous loop
        // create an input processor for processing keyboard and usb input
        InputProcessor processor(&gui, &hitjesList, &config);

        while (true) {
            // read user input for selecting hitje, also read from usb
            int c = gui.getInput();
            if (c == INPUT_NONE) {
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
#elif defined(GUI_WT)
        // For a Witty gui, all should be handled by signals
        Wt::WServer server(argv[0]);

        server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
        server.addEntryPoint(Wt::Application, createApplication);

        if (server.start()) {
            Wt::WServer::waitForShutdown();
            server.stop();
        }
    } catch (Wt::WException ex) {
        gui.printlevel(LERROR, "WException caught:\n");
        gui.confirm(LERROR, "%s", ex.what());
#endif
    } catch (const char *ex) {
        gui.confirm(LERROR, "%s\n", ex);
    }
    VLC::deleteInstance();
}

