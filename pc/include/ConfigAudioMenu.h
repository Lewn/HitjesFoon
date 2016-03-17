#ifndef CONFIGAUDIOMENU_H
#define CONFIGAUDIOMENU_H

#include "AudioMenu.h"
#include "Tools.h"

class ConfigAudioMenu : public AudioMenu {
public:
    ConfigAudioMenu(GUI &gui, string configMenuPath);
protected:
private:
    ConfigAudioMenu(const ConfigAudioMenu &that) = delete;
};

#endif // CONFIGAUDIOMENU_H
