#include "gui/GUIEvent.h"

GUIEvent::GUIEvent() {

}

GUIEvent::~GUIEvent() {

}


void GUIEvent::phoneNum(const InputPhoneNumCallback &callback) {
    inputPhoneNumSig.connect(callback);
}

void GUIEvent::phoneAlt(const InputPhoneAltCallback &callback) {
    inputPhoneAltSig.connect(callback);
}

void GUIEvent::speakerVolume(const SpeakerVolumeCallback &callback) {
    speakerVolumeSig.connect(callback);
}

void GUIEvent::phoneVolume(const PhoneVolumeCallback &callback) {
    phoneVolumeSig.connect(callback);
}

void GUIEvent::playback(const PlaybackCallback &callback) {
    playbackSig.connect(callback);
}

