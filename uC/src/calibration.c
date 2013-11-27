#include "common.h"
#if defined(_TINY_)
#include "calibration.h"

#define abs(x) ((x) > 0 ? (x) : (-x))

void calibrate(void) {
    cli();
    int frameLength, targetLength = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);
    int bestDeviation = targetLength;
    uint8_t trialCal, step, region, bestCal = OSCCAL;

    // do a binary search in regions 0-127 and 128-255 to get optimum OSCCAL
    for(region = 0; region <= 1; region++) {
        frameLength = 0;
        trialCal = (region == 0) ? 0 : 128;

        for(step = 64; step > 0; step >>= 1) {
            if(frameLength < targetLength) { // true for initial iteration
                trialCal += step;    // frequency too low
            } else {
                trialCal -= step;    // frequency too high
            }

            OSCCAL = trialCal;
            frameLength = usbMeasureFrameLength();

            if(abs(frameLength - targetLength) < bestDeviation) {
                bestCal = trialCal; // new optimum found
                bestDeviation = abs(frameLength - targetLength);
            }
        }
    }

    OSCCAL = bestCal;
    sei();
}
#endif
