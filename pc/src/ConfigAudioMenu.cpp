#include "ConfigAudioMenu.h"

ConfigAudioMenu::ConfigAudioMenu(GUI *gui, string configMenuPath) : AudioMenu(gui) {
    //fromPath("D:\\Dropbox\\Documenten\\projects\\github\\HitjesFoon\\AudioMenus\\Config\\");
    //fromPath("C:\\Users\\Leon\\Documents\\GitHub\\HitjesFoon\\AudioMenus\\Config\\");
    fromPath(configMenuPath);
}
