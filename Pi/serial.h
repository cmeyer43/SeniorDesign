#ifndef SERIAL_H
#define SERIAL_H

#include "stateMachine.h"

class serial
{
public:
    serial(char *device);

    int sendPacket(uint8_t* packet, int size);

    int recvPacket(uint8_t* packet, int size);

    control_state_t updateCoasterState();
private:
    int dev;
};

#endif
