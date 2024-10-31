#include "MuxControl.h"

MuxControl::MuxControl(byte s0, byte s1, byte s2, byte s3) {
    _s0 = s0;
    _s1 = s1;
    _s2 = s2;
    _s3 = s3;
    pinMode(_s0, OUTPUT);
    pinMode(_s1, OUTPUT);
    pinMode(_s2, OUTPUT);
    pinMode(_s3, OUTPUT);
}

void MuxControl::selectChannel(byte channel) {
    digitalWrite(_s0, bitRead(channel, 0));
    digitalWrite(_s1, bitRead(channel, 1));
    digitalWrite(_s2, bitRead(channel, 2));
    digitalWrite(_s3, bitRead(channel, 3));
}
