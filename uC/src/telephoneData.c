#include "telephoneData.h"
#include "macros.h"

#define dataChanged() SETBIT(tData, CHANGE_DATA_SHIFT)

volatile uint8_t tData = 0;

void setDialData(uint8_t dialData) {
    WRITEMASKEDBYTE(tData, dialData << DIAL_DATA_SHIFT, DIAL_DATA_MASK);
    dataChanged();
}

void setHornData(uint8_t hornData) {
    if (BITVAL(tData, HORN_DATA_SHIFT) ^ hornData) {
        WRITEMASKEDBYTE(tData, hornData << HORN_DATA_SHIFT, HORN_DATA_MASK);
        dataChanged();
    }
}

void setEarthData(uint8_t earthData) {
    if (BITVAL(tData, EARTH_DATA_SHIFT) ^ earthData) {
        WRITEMASKEDBYTE(tData, earthData << EARTH_DATA_SHIFT, EARTH_DATA_MASK);
        dataChanged();
    }
}

uint8_t isDataChanged() {
    return BITVAL(tData, CHANGE_DATA_SHIFT);
}

uint8_t getData() {
    CLEARBIT(tData, CHANGE_DATA_SHIFT);
    uint8_t retData = tData;
    CLEARBITS(tData, DIAL_DATA_MASK);
    return retData;
}
