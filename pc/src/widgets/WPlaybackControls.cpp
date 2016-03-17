#include "widgets/WPlaybackControls.h"

WPlaybackControls::WPlaybackControls(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WFilledTemplate(WString::tr("template-hitjesfoon-home-playback-controls"), parent), gui(gui), persistence(persistence) {
    buildWidget();
}

WPlaybackControls::WPlaybackControls(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {
    buildWidget();
}

WPlaybackControls::~WPlaybackControls() {
}


void WPlaybackControls::buildWidget() {
    volumeSlider = new WSlider();
    volumeSlider->setTickInterval(1);
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(100);
    volumeSlider->setValue(persistence.getIntData().getVal("volume-speaker"));

    stopBtn = new WPushButton(WString::tr("strings-playback-stop"));
    playBtn = new WPushButton(WString::tr("strings-playback-play"));
    pauseBtn = new WPushButton(WString::tr("strings-playback-pause"));

    bindWidget("playback-volume", volumeSlider);
    bindWidget("playback-stop", stopBtn);
    bindWidget("playback-play", playBtn);
    bindWidget("playback-pause", pauseBtn);
    updatePlayPause();

    // Process on value change
    volumeSlider->valueChanged().connect(boost::bind(WPlaybackControls::processVolumeChange, this, _1));
    // Also directly process on slide event (too many triggers, feels jerky)
//    volumeSlider->sliderMoved().connect(boost::bind(WPlaybackControls::processVolumeChange, this, _1));

    stopBtn->clicked().connect(this, &WPlaybackControls::processStop);
    playBtn->clicked().connect(this, &WPlaybackControls::processPlay);
    pauseBtn->clicked().connect(this, &WPlaybackControls::processPause);

    // Listen to all persistence changes
    persistence.onChangeCallback(boost::bind(&WPlaybackControls::onPersistenceChange, this, _1));
}

void WPlaybackControls::updatePlayPause() {
    bool playing = persistence.getIntData().getVal("playback-state") == PlaybackState::PLAY;
    pauseBtn->setHidden(!playing);
    playBtn->setHidden(playing);
}

void WPlaybackControls::onPersistenceChange(const string &key) {
    if (key == "volume-speaker") {
        volumeSlider->setValue(persistence.getIntData().getVal("volume-speaker"));
    } else if (key == "volume-phone") {

    } else if (key == "playback-state") {
        updatePlayPause();
    }
}


void WPlaybackControls::processVolumeChange(int volume) {
    gui.setSpeakerVolume(volume);
}

void WPlaybackControls::processStop() {
    gui.setPlaybackState(STOP);
}

void WPlaybackControls::processPlay() {
    gui.setPlaybackState(PLAY);
}

void WPlaybackControls::processPause() {
    gui.setPlaybackState(PAUSE);
}

