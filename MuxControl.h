#ifndef MUXCONTROL_H
#define MUXCONTROL_H

#include <Arduino.h>

class MuxControl {
public:
    MuxControl(byte s0, byte s1, byte s2, byte s3);
    void selectChannel(byte channel);
private:
    byte _s0, _s1, _s2, _s3;
};

#endif
