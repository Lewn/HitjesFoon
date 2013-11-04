#ifndef __TELEPHONE_DATA_H__
#define __TELEPHONE_DATA_H__

#include "../telephoneProtocol.h"

#include <stdint.h>

/**
 * 4 bits dial data
 * 1 bit hoorn
 * 1 bit aardknop
 * 1 bit reserved data
 * 1 bit for when data is changed (internal use only)
 */
extern volatile uint8_t tData;

void setDialData(uint8_t dialData);
void setHornData(uint8_t hornData);
void setEarthData(uint8_t earthData);
uint8_t isDataChanged(void);
uint8_t getData(void);

#endif