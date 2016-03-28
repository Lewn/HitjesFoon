
//#define GUI_CURSES
#define GUI_WT


#if defined(GUI_CURSES) && defined(GUI_WT)
#error "Can define only a single gui"
#endif // defined

#include "USBConnection.h"
#include "Tools.h"
#include "gui/GUI.h"
#include "retrieve/Retriever.h"
#include "retrieve/HTTPTransfer.h"

#ifdef GUI_CURSES
#include "gui/GUICurses.h"
#elif defined(GUI_WT)
#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WException>

#include "gui/GUINull.h"
#include "gui/GUIWt.h"

using namespace std;

Wt::WApplication *createApplication(const Wt::WEnvironment &env, GUIWt &gui) {
    try {
        return gui.createApplication(env);
    } catch (Wt::WException e) {
        gui.printlevel(LERROR, "WException caught: %s\n", e.what());
    } catch (const std::ios_base::failure &e) {
        gui.printlevel(LERROR, "IOS base error caught: %s\n", e.what());
    } catch (const std::logic_error &e) {
        gui.printlevel(LERROR, "Logic error caught: %s\n", e.what());
    } catch (const std::runtime_error &e) {
        gui.printlevel(LERROR, "Runtime error caught: %s\n", e.what());
    } catch (const std::exception &e) {
        gui.printlevel(LERROR, "Exception caught: %s\n", e.what());
    } catch (const char *e) {
        gui.printlevel(LERROR, "Unknown exception: %s\n", e);
    }
    return NULL;
}

#else
#error "Must define at least one gui"
#endif

#include <regex>


int main(int argc, char **argv) {
    try {
#ifdef GUI_CURSES
        GUICurses gui(LINFO);
#else
        GUIWt gui(LINFO);
#endif
        try {
#if defined(GUI_WT)
            // For a Witty gui, all should be handled by signals
            // First start the server
            // TODO memory errors in boost::configuration and boost::random here
            // TODO memory errors in Wt Logger (WLogEntry) everywhere...
            Wt::WServer server(argc, argv, WTHTTP_CONFIGURATION);
            gui.setServer(server);

            server.addEntryPoint(Wt::Application, boost::bind(createApplication, _1, boost::ref(gui)));
#endif

            gui.printlevel(LINFO, "Starting hitjesfoon application...\n");

            Config config(gui, "config.txt");
            gui.printlevel(LDEBUG, "Read config file\n");
//
            VLC::setConfig(&config);
//
//            // instantiate usb and vlc, cause we will need them
            USBConnection connection(gui);
            gui.printlevel(LDEBUG, "Instantiated usb connection\n");
            VLC::setGUI(&gui);
            VLC::getInstance();
            gui.printlevel(LDEBUG, "Instantiated vlc\n");

            // create an input processor for processing keyboard and usb input
            InputProcessor processor(gui, config);

#if defined(GUI_WT)
            if (server.start()) {
#endif
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
#if defined(GUI_WT)
                server.stop();
            } else {
                gui.printlevel(LERROR, "Couldn't start server\n");
            }
        } catch (Wt::WException ex) {
            gui.printlevel(LERROR, "WException caught:\n");
            gui.confirm(LERROR, "%s", ex.what());
#endif
        } catch (const std::ios_base::failure &e) {
            gui.printlevel(LERROR, "IOS base error caught: %s\n", e.what());
        } catch (const std::logic_error &e) {
            gui.printlevel(LERROR, "Logic error caught: %s\n", e.what());
        } catch (const std::runtime_error &e) {
            gui.printlevel(LERROR, "Runtime error caught: %s\n", e.what());
        } catch (const std::exception &e) {
            gui.printlevel(LERROR, "Exception caught: %s\n", e.what());
        } catch (const char *ex) {
            printf("%s\n", ex);
            //gui.confirm(LERROR, "%s\n", ex);
        }
    } catch (...) {
        printf("Something went terribly wrong...\n");
    }
    VLC::deleteInstance();
    return 0;
}

